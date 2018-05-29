//
//  data.hpp
//  Backtest Environment
//
//  Created by Evan Kirkiles on 5/29/18.
//  Copyright © 2018 Evan Kirkiles. All rights reserved.
//

#ifndef data_hpp
#define data_hpp
#ifndef YahooFinanceCSVReader
#include "YahooFinanceCSVReader.hpp"
#endif

#include <stdio.h>

#include "MarketDataFrame.hpp"
#include "events.hpp"

// Abstract base class so cannot be instantiated directly
class DataHandler {
public:
    // Handles data both historical and live
    // Virtual functions to be used by subclasses
    virtual map<string, MarketDataFrame> get_latest_bars(std::string symbol, int N=1);
    virtual void update_bars();
};

// Historical data handler that puts data onto an Event Queue
class HistoricalCSVDataHandler:DataHandler {
public:
    map<string, MarketDataFrame> symbol_data;
    map<string, MarketDataFrame> latest_data;
    bool continue_backtest;
    vector<Event> events;
    string csv_dir;
    vector<string> symbol_list;
    
    // PARAMS:
    // events: Event Queue on which to push new events
    // csv_dir: the path to the folder containing all the symbol csvs
    // symbol_list: list of all the symbols being traded
    HistoricalCSVDataHandler(vector<Event> events, string csv_dir, vector<string> symbol_list);
    
    // Format the symbol data
    void format_csv_data();
    
    // Creates input iterator for going through data
    map<string, double> get_new_bar(string symbol);
    
    // Parent DataHandler functions
    MarketDataFrame get_latest_bars();
    void update_bars();
};

#endif /* data_hpp */
