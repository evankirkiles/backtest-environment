//
//  YahooFinanceCSVReader.cpp
//  Backtest Environment
//
//  Created by Sam Kirkiles on 5/26/18.
//  Copyright © 2018 Evan Kirkiles. All rights reserved.
//

#include <iostream>
#include <curl/curl.h>

#include "YahooFinanceCSVReader.hpp"
#include "csvdatastruct.h"

using namespace std;

// Custom write function
size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

// Retrieve cookies and crumb from yahoo finance site to download csv
char get_crumb_and_cookies(char *symbol, char cookiefilename[FILENAME_MAX], char crumbfilename[FILENAME_MAX]) {
    
    // Init necessary variables
    CURL *cookiecurl = curl_easy_init();
    FILE * crumbfile;
    char nline[256];
    
    // Init curl session
    cookiecurl = curl_easy_init();
    
    // Create URL for the Yahoo Finance history for that stock
    string cookie_url = string("https://finance.yahoo.com/quote/") + string(symbol) + string("/history?p=") + string(symbol);

    // Set URL to go to finance.yahoo.com and prepare cookies file
    curl_easy_setopt(cookiecurl, CURLOPT_URL, cookie_url.c_str());
    curl_easy_setopt(cookiecurl, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(cookiecurl, CURLOPT_COOKIEFILE, cookiefilename);
    curl_easy_setopt(cookiecurl, CURLOPT_COOKIELIST, "ALL");
    
    // HTTP-header style cookie format
    snprintf(nline, sizeof(nline),
             "Set-Cookie: OLD_PREF=3d141414bf4209321; "
             "expires=Sun, 17-Jan-2038 19:14:07 GMT; path=/; domain=.example.com");
    
    // Set file as cookie jar
    curl_easy_setopt(cookiecurl, CURLOPT_COOKIEJAR, cookiefilename);
    curl_easy_setopt(cookiecurl, CURLOPT_COOKIELIST, nline);
    
    // Send all data to this function
    curl_easy_setopt(cookiecurl, CURLOPT_WRITEFUNCTION, write_data);
    
    // Open the file
    crumbfile = fopen(crumbfilename, "wb");
    if (crumbfile) {
        
        // Write the page body to this file handle
        curl_easy_setopt(cookiecurl, CURLOPT_WRITEDATA, crumbfile);
        
        // Performs blocking file transfer as defined
        curl_easy_perform(cookiecurl);
        
        // Close header file
        fclose(crumbfile);
    }
    
    // Sends all retrieved cookies to file
    curl_easy_cleanup(cookiecurl);
    curl_global_cleanup();
    
    char cx;
    return cx;
}


// Constructor for CVS Reader
YahooFinanceCSVReader:: YahooFinanceCSVReader(char *symbol, char *startdate, char *enddate, char *interval, char outfilename[FILENAME_MAX], char cookiefilename[FILENAME_MAX], char crumbfilename[FILENAME_MAX]) {
    
    // Initialize variables
    CURL *curl;
    CURLcode res;
    FILE *fp;
    
    // Get crumb and use it to create the download url
    char crumb = get_crumb_and_cookies(symbol, cookiefilename, crumbfilename);
    string down_url = string("https://query1.finance.yahoo.com/v7/finance/download/") + string(symbol) + string("?period1=") + string(startdate) + string("&period2=") + string(enddate) + string("&interval=") + string(interval) + string("&events=history&crumb=") + string(&crumb);
    
    // Init the curl session
    curl = curl_easy_init();
    
    // Set cookies
    curl_easy_setopt(curl, CURLOPT_COOKIEFILE, cookiefilename);
    
    // Set URL to go to given URL
    curl_easy_setopt(curl, CURLOPT_URL, down_url.c_str());
    
    // Send all data to this function
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    
    // Open the file
    fp = fopen(outfilename, "wb");
    if (fp) {
        
        // Write the page body to this file handle
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        
        // Performs blocking file transfer as defined
        res = curl_easy_perform(curl);
        
        // Close header file
        fclose(fp);
    }
    
    // Cleanup curl
    curl_easy_cleanup(curl);
    curl_global_cleanup();
};
