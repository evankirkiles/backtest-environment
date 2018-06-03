//
//  portfolio.cpp
//  Backtest Environment
//
//  Created by Evan Kirkiles on 6/2/18.
//  Copyright © 2018 Evan Kirkiles. All rights reserved.
//

#include "portfolio.hpp"

using namespace std;

// MARK: Naive Portfolio

// Constructor
NaivePortfolio::NaivePortfolio(map<string, map<string, map<long, double>>> i_bars, vector<string> i_symbol_list, vector<Event> i_events, long i_start_date, double i_initial_capital=100000.0) {
    
    // Initialize all instance variables
    bars = i_bars;
    events = i_events;
    start_date = i_start_date;
    initial_capital = i_initial_capital;
    symbol_list = i_symbol_list;
    
    // Create positions and holdings
    all_positions = construct_all_positions();
    current_positions = construct_current_positions();
    all_holdings = construct_all_holdings();
    current_holdings = construct_current_holdings();
}

// Positions & holdings constructors
map<long, map<string, double>> NaivePortfolio::construct_all_positions() {
    map<string, double> temp = {};
    for (int i = 0; i < symbol_list.size(); i++) {
        temp[symbol_list[i]] = 0;
    }
    map<long, map<string, double>> c;
    c[start_date] = temp;
    return c;
}
map<string, double> NaivePortfolio::construct_current_positions() {
    map<string, double> temp = {};
    for (int i = 0; i < symbol_list.size(); i++) {
        temp[symbol_list[i]] = 0;
    }
    return temp;
}
map<long, map<string, double>> NaivePortfolio::construct_all_holdings() {
    map<string, double> temp = {};
    for (int i = 0; i < symbol_list.size(); i++) {
        temp[symbol_list[i]] = 0;
    }
    temp["heldcash"] = initial_capital;
    temp["commission"] = 0.0;
    temp["totalholdings"] = initial_capital;
    map<long, map<string, double>> c;
    c[start_date] = temp;
    return c;
}
map<string, double> NaivePortfolio::construct_current_holdings() {
    map<string, double> temp = {};
    for (int i = 0; i < symbol_list.size(); i++) {
        temp[symbol_list[i]] = 0;
    }
    temp["heldcash"] = initial_capital;
    temp["commission"] = 0.0;
    temp["totalholdings"] = initial_capital;
    return temp;
}
