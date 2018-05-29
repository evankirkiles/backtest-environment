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


map<string, map<string, double>> data;
vector<string> indices;

// Takes .csv data in date-OHLCAV format and makes it into a map of maps
MarketDataFrame::MarketDataFrame(char csv_file[FILENAME_MAX], string symbol) {
    ifstream csv(csv_file);
    string line;
    map<string, double> open;
    map<string, double> high;
    map<string, double> low;
    map<string, double> close;
    map<string, double> adj;
    map<string, double> volume;
    
    // Iterate through the csv file
    while(getline(csv, line)) {
        string date;
        
        // Get string stream
        replace(line.begin(), line.end(), ',', ' ');
        stringstream ss(line);
        
        // Get each datapoint
        map<string, double> dataRow;
        ss >> date;
        ss >> open[date];
        ss >> high[date];
        ss >> low[date];
        ss >> close[date];
        ss >> adj[date];
        ss >> volume[date];
        
        // Add the date to the indices
        indices.push_back(date);
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
};
