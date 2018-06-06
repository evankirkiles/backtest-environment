//
//  events.cpp
//  Backtest Environment
//
//  Created by Evan Kirkiles on 5/29/18.
//  Copyright © 2018 Evan Kirkiles. All rights reserved.
//

#include "events.hpp"
#ifndef iostream
#include <iostream>
#endif

using namespace std;

// Market event initialization
MarketEvent::MarketEvent() {
    type = "MARKET";
}

// Signal event initialization
SignalEvent::SignalEvent(string i_symbol, long i_datetime, string i_signal_type, double i_strength) {
    type = "SIGNAL";
    symbol = i_symbol;
    datetime = i_datetime;
    strength = i_strength;
    signal_type = i_signal_type;
}

// Order event initialization
OrderEvent::OrderEvent(string i_symbol, string i_order_type, int i_quantity, string i_direction) {
    type = "ORDER";
    symbol = i_symbol;
    order_type = i_order_type;
    quantity = i_quantity;
    direction = i_direction;
}

// Default order event initializer
OrderEvent::OrderEvent() { }

// Fill event initialization
FillEvent::FillEvent(string i_timeindex, string i_symbol, string i_exchange, int i_quantity, string i_direction,
                     double i_fill_cost, double i_commission) {
    type = "FILL";
    timeindex = i_timeindex;
    symbol = i_symbol;
    exchange = i_exchange;
    quantity = i_quantity;
    direction = i_direction;
    fill_cost = i_fill_cost;
    
    // Calculate commission
    if (i_commission) {
        commission = i_commission;
    } else {
        commission = calculate_IB_commission();
    }
}

// Calculate commission based on Interactive Brokers' rates
double FillEvent::calculate_IB_commission() {
    double full_cost = 1.3;
    if (quantity <= 500) { full_cost = max(1.3, 0.013 * quantity); }
    else { full_cost = max(1.3, 0.008 * quantity); }
    full_cost = min(full_cost, 0.5 / 100.0 * quantity * fill_cost);
    return full_cost;
}
