//
//  events.cpp
//  Backtest Environment
//
//  Created by Evan Kirkiles on 5/29/18.
//  Copyright © 2018 Evan Kirkiles. All rights reserved.
//

#include "events.hpp"

using namespace std;

// Market event initialization
MarketEvent::MarketEvent() {
    type = "MARKET";
}

// Signal event initialization
SignalEvent::SignalEvent(string i_symbol, long i_datetime, double percentage, string i_target) {
    type = "SIGNAL";
    symbol = i_symbol;
    datetime = i_datetime;
    strength = percentage;
    target = i_target;
}

// Order event initialization
OrderEvent::OrderEvent(string i_symbol, string i_order_type, int i_quantity, string i_direction, string i_target) {
    type = "ORDER";
    symbol = i_symbol;
    order_type = i_order_type;
    quantity = i_quantity;
    direction = i_direction;
    target = i_target;
}

// Default order event initializer
OrderEvent::OrderEvent() { }

// Fill event initialization
FillEvent::FillEvent(long i_timeindex, string i_symbol, string i_exchange, int i_quantity, string i_direction, double i_fill_cost, double i_commission, string i_target) {
    type = "FILL";
    timeindex = i_timeindex;
    symbol = i_symbol;
    exchange = i_exchange;
    quantity = i_quantity;
    direction = i_direction;
    slippage = i_fill_cost;
    target = i_target;
    
    // Calculate commission
    if (i_commission >= 0) {
        commission = i_commission;
    } else {
        commission = calculate_IB_commission();
    }
}

// Calculate commission based on Interactive Brokers' rates (outdated at the moment)
double FillEvent::calculate_IB_commission() {
    double full_cost = 1.3;
    if (quantity <= 500) {
        full_cost = max(1.3, 0.013 * quantity);
    } else {
        full_cost = max(1.3, 0.008 * quantity);
    }
    return full_cost;
}
