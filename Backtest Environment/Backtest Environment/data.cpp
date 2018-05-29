//
//  data.cpp
//  Backtest Environment
//
//  Created by Evan Kirkiles on 5/29/18.
//  Copyright © 2018 Evan Kirkiles. All rights reserved.
//

#include "data.hpp"

// Gets data from Yahoo Finance CSV's and returns them in format
// An interface to allow for getting "latest"
HistoricalCSVDataHandler::HistoricalCSVDataHandler(vector<Event> i_events, string i_csv_dir, vector<string> i_symbol_list) {
    
    events = i_events;
    csv_dir = i_csv_dir;
    symbol_list = i_symbol_list;
    continue_backtest = true;
    
};

// Format symbol data
void HistoricalCSVDataHandler::format_csv_data() {
    // Loop through symbols and load their data into a single frame
    for(int i=0;i<symbol_list.size();i++) {
        string symbol = symbol_list[i];
        
        // Get the data from Yahoo Finance
        MarketDataFrame moves = YahooFinanceCSVReader((char*)symbol.c_str(),
                                                      (char*)(string("/Users/samkirkiles/Desktop/Backtest Environment/Backtest Environment/Backtest Environment/Data Handling/CSV directory/") + symbol_list[i] + string(".csv")).c_str(),
                                                      (char*)"/Users/samkirkiles/Desktop/Backtest Environment/Backtest Environment/Backtest Environment/Data Handling/cookies.txt",
                                                      (char*)"/Users/samkirkiles/Desktop/Backtest Environment/Backtest Environment/Backtest Environment/Data Handling/crumb.txt").marketmovements;
        
        // Access for a data point is symbol-data[SYMBOL][TYPE][DATE]
        symbol_data[symbol_list[i]] = moves;
    }
}

// Gets input iterator for going through data
map<string, double> get_new_bar(string symbol) {
    
    // TODO: Learn how to use Boost Coroutine to get a Python yield-like interaction
    // That spits out a bar until there are no more bars to yield
    
    return {};
}
