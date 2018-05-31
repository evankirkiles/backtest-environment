//
//  strat_buyandhold.cpp
//  Backtest Environment
//
//  Created by Evan Kirkiles on 5/31/18.
//  Copyright © 2018 Evan Kirkiles. All rights reserved.
//

#include "strategiesheader.hpp"

// Initialize strategy
BuyAndHoldStrategy::BuyAndHoldStrategy(HistoricalCSVDataHandler i_bars, vector<Event> i_events) {
    
    // Set instance variables
    bars = i_bars;
    events = i_events;
    symbol_list = bars.symbol_list;
    
    // Create the bought dictionary
    bought = calculate_initial_bought();
}

// Create map of bought symbols
map<string, bool> BuyAndHoldStrategy::calculate_initial_bought() {
    bought = {};
    for (int i=0; i < symbol_list.size(); i++) {
        bought[symbol_list[i]] = false; }
    return bought;
}

// Update map of bought symbols
void BuyAndHoldStrategy::calculate_signals(MarketEvent i_event) {
    
    // LONG any symbol whose bar is updated by marketevent
    if (i_event.type == "MARKET") {
        for (int i=0; i < symbol_list.size(); i++) {
            string symbol = symbol_list[i];
            map<string, map<long, double>> newbars = bars.get_latest_bars(symbol_list[i], 1);
            if (newbars.empty()) {
                if (bought[symbol] == false) {
                    // (symbol, time, type=LONG, SHORT or EXIT)
                    SignalEvent signal = SignalEvent(symbol, bars.latestDates[symbol][0], string("LONG"));
                    events.push_back(signal);
                    bought[symbol_list[i]] = true;
                }
            }
        }
    }
}
