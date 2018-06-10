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
#include <boost/coroutine2/all.hpp>
#endif
#ifndef ptr_vector
#include <boost/ptr_container/ptr_vector.hpp>
#endif

#include <stdio.h>

#include "MarketDataFrame.hpp"
#include "events.hpp"

// Abstract base class so cannot be instantiated directly
class DataHandler {
public:
    // Handles data both historical and live
    // Virtual functions to be used by subclasses
    virtual map<string, map<long, double>> get_latest_bars(std::string symbol, int N=1)=0;
    virtual void update_bars() = 0;
};

// Historical data handler that puts data onto an Event Queue
class HistoricalCSVDataHandler:DataHandler {
public:
    map<string, map<string, map<long, double>>> symbol_data;
    map<string, map<string, map<long, double>>> latest_data;
    map<string, tuple<string, long, double, double, double, double, double, double>> previousbar;
    vector<long> allDates;
    map<string, vector<long>> latestDates;
    map<string, int> currentDatesIndex;
    bool continue_backtest;
    boost::ptr_vector<Event>* events;
    vector<string>* symbol_list;
    
    // PARAMS:
    // events: Event Queue on which to push new events
    // csv_dir: the path to the folder containing all the symbol csvs
    // symbol_list: list of all the symbols being traded
    HistoricalCSVDataHandler(boost::ptr_vector<Event>* events, vector<string>* symbol_list);
    
    // Placeholder initializer
    HistoricalCSVDataHandler();
    
    // Format the symbol data
    void format_csv_data();
    
    // Creates input iterator for going through data
    tuple<string, long, double, double, double, double, double, double> get_new_bar(string symbol);
    
    // Parent DataHandler functions
    map<string, map<long, double>> get_latest_bars(std::string symbol, int N);
    void update_bars();
    
    void append_to_dates(vector<long> new_dates);
};

#endif /* data_hpp */
