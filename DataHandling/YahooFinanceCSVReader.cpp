//
//  YahooFinanceCSVReader.cpp
//  Backtest Environment
//
//  Created by Evan Kirkiles on 5/26/18.
//  Copyright © 2018 Evan Kirkiles. All rights reserved.
//

#ifndef fstream
#include <fstream>
#endif /* <fstream> */
#ifndef iostream
#include <iostream>
#endif /* <iostream> */
#include <curl/curl.h>
#include <chrono>
#include <cmath>

#include "YahooFinanceCSVReader.hpp"

using namespace std;

// Custom write function
size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

// Retrieve cookies and crumb from yahoo finance site to download csv
const char* get_crumb_and_cookies(char *symbol, char cookiefilename[FILENAME_MAX], char crumbfilename[FILENAME_MAX]) {
    
    // Init necessary variables
    CURL *cookiecurl = curl_easy_init();
    FILE * crumbfile;
    char nline[256];
    const char *crumb = "";
    
    // Init curl session
    cookiecurl = curl_easy_init();
    
    // Create URL for the Yahoo Finance history for that stock
    string cookie_url = string("https://finance.yahoo.com/quote/") + string(symbol) + string("/history?p=") + string(symbol);
    
    // Set URL to go to finance.yahoo.com and prepare cookies file
    curl_easy_setopt(cookiecurl, CURLOPT_URL, cookie_url.c_str());
    curl_easy_setopt(cookiecurl, CURLOPT_COOKIEFILE, cookiefilename);
    curl_easy_setopt(cookiecurl, CURLOPT_COOKIELIST, "ALL");
    
    // Netscape format cookie 
    snprintf(nline, sizeof(nline), "%s\t%s\t%s\t%s\t%lu\t%s\t%s",
             ".example.com", "TRUE", "/", "FALSE",
             (unsigned long)time(nullptr) + 31337UL,
             "PREF", "hello example, i like you very much!");
    
    // Set file as cookie jar
    curl_easy_setopt(cookiecurl, CURLOPT_COOKIESESSION, true);
    curl_easy_setopt(cookiecurl, CURLOPT_COOKIEJAR, cookiefilename);
    curl_easy_setopt(cookiecurl, CURLOPT_COOKIELIST, nline);
    curl_easy_setopt(cookiecurl, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(cookiecurl, CURLOPT_HEADER, 0);
    
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
    
    // Search for crumb in newly created crumb file
    ifstream searchFile(crumbfilename);
    string str;
    while (getline(searchFile, str)) {
        if (str.find("CrumbStore\":{\"crumb\":\"") != str.npos){
            size_t pos = str.find("CrumbStore\":{\"crumb\":\"") + 22;
            crumb = str.substr(pos, 11).c_str();
            break;
        }
    }

    return crumb;
}

// Constructor for CVS Reader
YahooFinanceCSVReader:: YahooFinanceCSVReader(char *symbol, long startdate, long enddate, char outfilename[FILENAME_MAX], char cookiefilename[FILENAME_MAX], char crumbfilename[FILENAME_MAX]) : marketmovements() {
    
    // Dates formatted as YYYY-MM-DD
    auto interval = (char*)"1d";
    
    // Initialize variables
    CURL *curl;
    CURLcode res;
    FILE *fp;
    
    // Get crumb and use it to create the download url
    string down_url = string("https://query1.finance.yahoo.com/v7/finance/download/") + string(symbol) + string("?period1=") + to_string(startdate) + string("&period2=") + to_string(enddate) + string("&interval=") + string(interval) + string("&events=history&crumb=") + get_crumb_and_cookies(symbol, cookiefilename, crumbfilename);
    
    // Init the curl session
    curl = curl_easy_init();
    
    // Set cookies
    curl_easy_setopt(curl, CURLOPT_COOKIEFILE, cookiefilename);
    
    // Set URL to go to given URL
    curl_easy_setopt(curl, CURLOPT_URL, down_url.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_NOBODY, 0L);
    
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
    
    // Get the data from the .csv file
    marketmovements = MarketDataFrame(outfilename, string(symbol));
    
    // Cleanup curl
    curl_easy_cleanup(curl);
    curl_global_cleanup();
};
