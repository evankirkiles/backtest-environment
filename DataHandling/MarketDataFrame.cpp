//
//  MarketDataFrame.cpp
//  Backtest Environment
//
//  Created by Evan Kirkiles on 5/27/18.
//  Copyright © 2018 Evan Kirkiles. All rights reserved.
//

#ifndef fstream
#include <fstream>
#endif /* <fstream> */
#ifndef iostream
#include <iostream>
#endif /* <iostream> */
#ifndef sstream
#include <sstream>
#endif /* <sstream> */
#include <stdio.h>
#include "MarketDataFrame.hpp"

map<string, map<long, double>> data;
vector<long> indices;

// Find epoch time from normal YYYY-MM-DD
long get_epoch_time(string date) {
    string delimiter = "-";
    string token;
    struct tm t = {0};
    size_t pos = 0;
    while ((pos = date.find(delimiter)) != std::string::npos) {
        token = date.substr(0, pos);
        date.erase(0, pos + delimiter.length());
        switch (pos) {
            case 4:
                t.tm_year = stoi(token);
                break;
            case 2:
                t.tm_mon = stoi(token);
                break;
        }
    }

    if (date == "Date") {
        t.tm_mday = 0;
    } else {
        t.tm_mday = stoi(date);
    }
        
    // Get time since 1900 epoch
    t.tm_year -= 1900;
    t.tm_mon--;
    t.tm_hour = 0;
    t.tm_min = 0;
    t.tm_sec = 0;
    time_t timeSinceEpoch = mktime(&t);
    
    return long(timeSinceEpoch);
}

// Find normal time YYYY-MM-DD from epoch time
string get_std_time(long epochtime) {
    time_t t = epochtime;
    char string[80];
    strftime(string, 80, "%Y-%m-%d", gmtime(&t));
    return string;
}

// Placeholder default constructor
MarketDataFrame::MarketDataFrame() {}
    
// Takes .csv data in date-OHLCAV format and makes it into a map of maps
MarketDataFrame::MarketDataFrame(char csv_file[FILENAME_MAX], string symbol) {
    ifstream csv(csv_file);
    string line;
    map<long, double> open;
    map<long, double> high;
    map<long, double> low;
    map<long, double> close;
    map<long, double> adj;
    map<long, double> volume;
    
    // Iterate through the csv file
    while(getline(csv, line)) {
        string date;
        
        // Get string stream
        replace(line.begin(), line.end(), ',', ' ');
        stringstream ss(line);
        
        // Get each datapoint
        map<string, double> dataRow;
        ss >> date;
        long epochdate = get_epoch_time((char*)date.c_str());
        ss >> open[epochdate];
        ss >> high[epochdate];
        ss >> low[epochdate];
        ss >> close[epochdate];
        ss >> adj[epochdate];
        ss >> volume[epochdate];
        
        // Add the date to the indices
        indices.push_back(epochdate);
    }
    
    // Enter data into the mock DataFrame
    data["open"] = open;
    data["high"] = high;
    data["low"] = low;
    data["close"] = close;
    data["adj"] = adj;
    data["volume"] = volume;
    
    // Pop out the index that says "Date"
    indices.erase(indices.begin());
};
