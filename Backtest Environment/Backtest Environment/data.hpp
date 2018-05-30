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
#ifndef boost
#include <boost/coroutine/all.hpp>
#endif

#include <stdio.h>

#include "MarketDataFrame.hpp"
#include "events.hpp"

// Abstract base class so cannot be instantiated directly
class DataHandler {
public:
    // Handles data both historical and live
    // Virtual functions to be used by subclasses
    virtual void /*MarketDataFrame*/ get_latest_bars(std::string symbol, int N=1)=0;
    virtual void update_bars() = 0;
};

// Historical data handler that puts data onto an Event Queue
class HistoricalCSVDataHandler:DataHandler {
public:
    map<string, map<string, map<double, double>>> symbol_data;
    map<string, map<string, map<double, double>>> latest_data;
    vector<double> allDates;
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
    //void get_new_bar(boost::coroutines::coroutine<tuple<double, double, double, double, double, double>>::push_type &sink, string symbol);
    
    // Parent DataHandler functions
    void /*MarketDataFrame*/ get_latest_bars(std::string symbol, int N);
    void update_bars();
    
    void append_to_dates(vector<double> new_dates);
};

#endif /* data_hpp */
