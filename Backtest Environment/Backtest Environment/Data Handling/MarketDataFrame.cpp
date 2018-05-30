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

map<string, map<double, double>> data;
vector<double> indices;

// Find epoch time from normal YYYY/MM/DD
double get_epoch_time(char* date) {
    string delimiter = "-";
    string token;
    struct tm t = {0};
    string sdate(date);
    size_t pos = 0;
    while ((pos = sdate.find(delimiter)) != std::string::npos) {
        token = sdate.substr(0, pos);
        sdate.erase(0, pos + delimiter.length());
        switch (pos) {
            case 4:
                t.tm_year = stoi(token);
                break;
            case 2:
                t.tm_mon = stoi(token);
                break;
        }
    }

    if (sdate == "Date") {
        t.tm_mday = 0;
    } else {
        t.tm_mday = stoi(sdate);
    }
        
    // Get time since 1900 epoch
    t.tm_year -= 1900;
    t.tm_mon--;
    t.tm_hour = 0;
    t.tm_min = 0;
    t.tm_sec = 0;
    time_t timeSinceEpoch = mktime(&t);
    
    return double(timeSinceEpoch);
}

// Placeholder default constructor
MarketDataFrame::MarketDataFrame() {}
    
// Takes .csv data in date-OHLCAV format and makes it into a map of maps
MarketDataFrame::MarketDataFrame(char csv_file[FILENAME_MAX], string symbol) {
    ifstream csv(csv_file);
    string line;
    map<double, double> open;
    map<double, double> high;
    map<double, double> low;
    map<double, double> close;
    map<double, double> adj;
    map<double, double> volume;
    
    // Iterate through the csv file
    while(getline(csv, line)) {
        string date;
        
        // Get string stream
        replace(line.begin(), line.end(), ',', ' ');
        stringstream ss(line);
        
        // Get each datapoint
        map<string, double> dataRow;
        ss >> date;
        double epochdate = get_epoch_time((char*)date.c_str());
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
    
    // Reverse the index vector so most recent date is 0
    reverse(indices.begin(), indices.end());
    
    // Pop out the index that says "Date"
    indices.pop_back();
};
