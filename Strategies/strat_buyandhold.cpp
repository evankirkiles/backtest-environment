//
//  strat_buyandhold.cpp
//  Backtest Environment
//
//  Created by Evan Kirkiles on 5/31/18.
//  Copyright © 2018 Evan Kirkiles. All rights reserved.
//

#include "strategiesheader.hpp"

#ifndef ptr_vector
#include <boost/ptr_container/ptr_vector.hpp>
#endif

// Initialize strategy
MainStrategy::MainStrategy(HistoricalCSVDataHandler* i_bars, boost::ptr_vector<Event>*i_events) {
    
    // Set instance variables
    bars = i_bars;
    events = i_events;
    *bars->symbol_list = {string("GS"),string("AAPL"), string("CAT")};
    symbol_list = bars->symbol_list;
    
    // Set custom strategy variables
    // Need to instantiate map bought in header file
    bought = calculate_initial_bought();
}

// Placeholder initializer
MainStrategy::MainStrategy()=default;

// Create map of bought symbols
map<string, bool> MainStrategy::calculate_initial_bought() {
    bought = {};
    for (int i=0; i < symbol_list->size(); i++) {
        bought[(*symbol_list)[i]] = false; }
    return bought;
}

// Update map of bought symbols
void MainStrategy::calculate_signals(MarketEvent i_event) {
    
    // LONG any symbol whose bar is updated by marketevent
    for (int i=0; i < symbol_list->size(); i++) {
        string symbol = (*symbol_list)[i];
        map<string, map<long, double>> newbars = bars->get_latest_bars(symbol, 1);
        if (!newbars.empty() && newbars["open"].begin()->second > 0.01) {
            if (!bought[symbol]) {
                
                // (symbol, time, type=LONG, SHORT or EXIT)
                events->push_back(new SignalEvent(symbol, newbars["open"].begin()->first, 0.1));
                bought[symbol] = true;
            }
        }
    }
}
