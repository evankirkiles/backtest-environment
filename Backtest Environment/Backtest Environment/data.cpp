//
//  data.cpp
//  Backtest Environment
//
//  Created by Evan Kirkiles on 5/29/18.
//  Copyright © 2018 Evan Kirkiles. All rights reserved.
//

#include "data.hpp"
#ifndef boost
#include <boost/coroutine/all.hpp>
#endif

using namespace std;

// Gets data from Yahoo Finance CSV's and returns them in format
// An interface to allow for getting "latest"
HistoricalCSVDataHandler::HistoricalCSVDataHandler(vector<Event> i_events, string i_csv_dir, vector<string> i_symbol_list) {
    
    events = i_events;
    csv_dir = i_csv_dir;
    symbol_list = i_symbol_list;
    continue_backtest = true;
    
};

// Appends unique dates to master dates list
void HistoricalCSVDataHandler::append_to_dates(vector<double> new_dates) {
    if (allDates.empty()) {
        allDates = new_dates;
    } else {
        allDates.insert(allDates.end(), new_dates.begin(), new_dates.end());
        sort(allDates.begin(), allDates.end());
        auto last = unique(allDates.begin(), allDates.end());
        allDates.erase(last, allDates.end());
    }
}

// Format symbol data
void HistoricalCSVDataHandler::format_csv_data() {
    // Loop through symbols and load their data into a single frame
    for(int i=0;i<symbol_list.size();i++) {
        string symbol = symbol_list[i];
        
        // Get the data from Yahoo Finance
        // Access formula is symbol_data[SYMBOL].data[TYPE][DATE]
        MarketDataFrame moves = YahooFinanceCSVReader((char*)symbol.c_str(),
                                                      (char*)(string("/Users/samkirkiles/Desktop/Backtest Environment/Backtest Environment/Backtest Environment/Data Handling/CSV directory/") + symbol + string(".csv")).c_str(),
                                                      (char*)"/Users/samkirkiles/Desktop/Backtest Environment/Backtest Environment/Backtest Environment/Data Handling/cookies.txt",
                                                      (char*)"/Users/samkirkiles/Desktop/Backtest Environment/Backtest Environment/Backtest Environment/Data Handling/crumb.txt").marketmovements;
        symbol_data[symbol] = moves.data;
    }
}


// Gets input iterator for going through data
// Coroutine will return OHLCAV data
/*
void HistoricalCSVDataHandler::get_new_bar(boost::coroutines::coroutine<tuple<double, double, double, double, double, double>>::push_type &sink, string symbol) {
    
    // TODO: Learn how to use Boost Coroutine to get a Python yield-like interaction
    // That spits out a bar until there are no more bars to yield
    
    for (int i=0; i<symbol_data[symbol].indices.size();i++) {
        double date = symbol_data[symbol].indices.end()[-(i+1)];
        sink(make_tuple(symbol_data[symbol]));
    }
    
}
*/
// Get latest N bars
void /*MarketDataFrame*/ HistoricalCSVDataHandler::get_latest_bars(string symbol, int N=1) {
}
void HistoricalCSVDataHandler::update_bars() {}

