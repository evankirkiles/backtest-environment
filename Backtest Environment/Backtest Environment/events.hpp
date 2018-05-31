//
//  events.hpp
//  Backtest Environment
//
//  Created by Evan Kirkiles on 5/29/18.
//  Copyright © 2018 Evan Kirkiles. All rights reserved.
//

#ifndef events_hpp
#define events_hpp
#ifndef string
#include <string>
#endif
#ifndef iostream
#include <iostream>
#endif

#include <stdio.h>

using namespace std;

// Base event class to which we can add features
class Event { };

// Market event for updating data with corresponding bars
class MarketEvent: public Event {
public:
    string type;
    MarketEvent();
};

// Signal event for sending a signal from an algo
class SignalEvent: public Event {
public:
    string type;
    string symbol;
    string datetime;
    string signal_type;
    
    // PARAMS:
    // symbol: ticker symbol
    // datetime: the timestamp at which signal was generated
    // signal_type: "LONG" or "SHORT"
    SignalEvent(string symbol, string datetime, string signal_type);
};

// Order event for sending an order to execution system
class OrderEvent: public Event {
public:
    string type;
    string symbol;
    string order_type;
    int quantity;
    string direction;
    
    // PARAMS:
    // symbol: ticker symbol
    // order_type: "MKT" or "LMT" for market or limit
    // quantity: non-negative integer for quantity
    // direction: "BUY" or "SELL" for long or short
    OrderEvent(string symbol, string order_type, int quantity, string direction);
    
    // Prints the values in the order
    void print_order() {
        cout << "Order: Symbol=" << symbol << " Type=" << order_type << " Quantity=" << quantity << " Direction=" << direction << endl;
    }
};

// Gets the filled information about an order from the broker
class FillEvent: public Event {
public:
    string type;
    string timeindex;
    string symbol;
    string exchange;
    int quantity;
    string direction;
    double fill_cost;
    double commission;
    
    // PARAMS:
    // timeindex: bar resolution when order was filled
    // symbol: ticker symbol for order filled
    // exchange: the exchange on which order was filled
    // quantity: the filled quantity (can implement slippage)
    // direction: the direction of the fill ("BUY" or "SELL")
    // fill_cost: the holdings value in dollars
    // commission: optional commission sent from IB
    
    FillEvent(string timeindex, string symbol, string exchange, int quantity, string direction,
              double fill_cost, double commission=NULL);
    
    // Calculates the commission for an order given the size
    // Based on Interactive Brokers' https://www.interactivebrokers.com/en/index.php?f=commission&p=stocks2
    double calculate_IB_commission();
};

#endif /* events_hpp */
