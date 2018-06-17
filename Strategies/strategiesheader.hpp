//
//  strategiesheader.hpp
//  Backtest Environment
//
//  Created by Evan Kirkiles on 5/31/18.
//  Copyright © 2018 Evan Kirkiles. All rights reserved.
//

#ifndef strategiesheader_hpp
#define strategiesheader_hpp
#ifndef DataHandler
#include "../Infrastructure/data.hpp"
#endif
#ifndef Event
#include "../Infrastructure/events.hpp"
#endif

#include <stdio.h>

using namespace std;

// Strategy class to be inherited by all strategies
class Strategy {
public:
    // Function that sends signals based on market data
    virtual void calculate_signals(MarketEvent event) = 0;
};

// Benchmark class that goes 100% long in SPY on the first MarketEvent and holds forever
// Mimics the setup of a real strategy to allow for easy editing
class Benchmark: Strategy {
public:
    HistoricalCSVDataHandler* bars;
    boost::ptr_vector<Event>* events;
    vector<string>* symbol_list;
    map<string, bool> bought;
    
    Benchmark();
    // Initialize instance of Buy and Hold strategy
    Benchmark(HistoricalCSVDataHandler* i_bars, boost::ptr_vector<Event>* i_events);
    // Add keys for all symbols in symbol_list to bought and sets them to false
    map<string, bool> calculate_initial_bought();
    // Trading logic in this function for new event
    void calculate_signals(MarketEvent event);
};

// TO MAKE A NEW STRATEGY, WRITE IT IN MAIN_CPP; IF YOU WANT TO SAVE ONE THEN DUPLICATE MAIN_CPP AND
// RENAME IT SO YOU CAN COPY + PASTE IT BACK IN
// ONLY MAINSTRATEGY_CPP IS INCLUDED IN BUILD PATH, SO IT IS THE ONE WHOSE STRATEGY WILL BE USED
class MainStrategy: Strategy {
public:
    HistoricalCSVDataHandler* bars;
    boost::ptr_vector<Event>* events;
    vector<string>* symbol_list;
    map<string, bool> bought;
    string title;
    
    MainStrategy();
    // Initialize instance of Buy and Hold strategy
    MainStrategy(HistoricalCSVDataHandler* i_bars, boost::ptr_vector<Event>* i_events);
    // Add keys for all symbols in symbol_list to bought and sets them to false
    map<string, bool> calculate_initial_bought();
    // Trading logic in this function for new event
    void calculate_signals(MarketEvent event);
};

#endif /* strategiesheader_hpp */