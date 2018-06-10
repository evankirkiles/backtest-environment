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
BuyAndHoldStrategy::BuyAndHoldStrategy(HistoricalCSVDataHandler* i_bars, boost::ptr_vector<Event>*i_events) {
    
    // Set instance variables
    bars = i_bars;
    events = i_events;
    symbol_list = bars->symbol_list;
    
    // Create the bought dictionary
    bought = calculate_initial_bought();
}

// Placeholder initializer
BuyAndHoldStrategy::BuyAndHoldStrategy() { };

// Create map of bought symbols
map<string, bool> BuyAndHoldStrategy::calculate_initial_bought() {
    bought = {};
    for (int i=0; i < symbol_list->size(); i++) {
        bought[(*symbol_list)[i]] = false; }
    return bought;
}

// Update map of bought symbols
void BuyAndHoldStrategy::calculate_signals(MarketEvent i_event) {
    
    // LONG any symbol whose bar is updated by marketevent
    for (int i=0; i < symbol_list->size(); i++) {
        string symbol = (*symbol_list)[i];
        map<string, map<long, double>> newbars = bars->get_latest_bars(symbol, 1);
        cout << "Open for " << symbol << " at date " << newbars["open"].begin()->first << " is " <<  newbars["open"].begin()->second << "." << endl;
        if (!newbars.empty()) {
            if (bought[symbol] == false) {
                
                // (symbol, time, type=LONG, SHORT or EXIT)
                events->push_back(new SignalEvent(symbol, bars->latestDates[symbol][0], string("LONG"), 100.0));
                bought[symbol] = true;
            }
        }
    }
}
