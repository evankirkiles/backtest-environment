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
#include "data.hpp"
#endif
#ifndef Event
#include "events.hpp"
#endif

#include <stdio.h>

#endif /* strategiesheader_hpp */

using namespace std;

// Strategy class to be inherited by all strategies
class Strategy {
public:
    // Function that sends signals based on market data
    virtual void calculate_signals(MarketEvent event) = 0;
};

// INCLUDE DEFINITION OF ALL STRATEGIES IN THIS FILE
// INITIALIZE THE FUNCTIONS OF EACH STRATEGY IN THEIR OWN .cpp

// Logic:
// Simple testing strategy that goes LONG all of the symbols whenever a new bar is received from the data handler
// Does not exit positions ever, test strategy class with this
class BuyAndHoldStrategy: Strategy {
public:
    HistoricalCSVDataHandler* bars;
    boost::ptr_vector<Event>* events;
    vector<string>* symbol_list;
    map<string, bool> bought;
    
    BuyAndHoldStrategy();
    // Initialize instance of Buy and Hold strategy
    BuyAndHoldStrategy(HistoricalCSVDataHandler* i_bars, boost::ptr_vector<Event>* i_events);
    // Add keys for all symbols in symbol_list to bought and sets them to false
    map<string, bool> calculate_initial_bought();
    // Trading logic in this function for new event
    void calculate_signals(MarketEvent event);
};
