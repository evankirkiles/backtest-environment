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
NaivePortfolio::NaivePortfolio(HistoricalCSVDataHandler i_bars, vector<string> i_symbol_list, vector<Event> i_events, long i_start_date, double i_initial_capital=100000.0) {
    
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

// Update holdings evaluations with most recently completed bar (previous day)
void NaivePortfolio::update_timeindex(Event event) {
    map<string, map<string, map<long, double>>> lastbar;
    long date;
    double sumvalues = 0;
    
    for (int i=0; i<symbol_list.size();i++) {
        
        // Update positions
        lastbar[symbol_list[i]] = bars.get_latest_bars(symbol_list[i],1);
        
        date = lastbar[symbol_list[i]]["open"].end()->first;
        all_positions[date][symbol_list[i]] = current_positions[symbol_list[i]];
        
        // Update holdings
        // Estimates market value of a stock by using the quantity * its closing price (most likely inaccurate)
        double market_value = current_positions[symbol_list[i]] * lastbar[symbol_list[i]]["close"][date];
        all_holdings[date][symbol_list[i]] = market_value;
        sumvalues += market_value;
    }
    
    all_holdings[date]["totalholdings"] = current_holdings["heldcash"] + sumvalues;
    all_holdings[date]["heldcash"] = current_holdings["heldcash"];
    all_holdings[date]["commission"] = current_holdings["commission"];
}

// Update positions from a FillEvent
void NaivePortfolio::update_positions_from_fill(FillEvent fill) {
    int fill_dir = 0;
    if (fill.direction == "BUY") {
        fill_dir = 1;
    } else if (fill.direction == "SELL") {
        fill_dir = -1;
    }
    
    // Update the positions with retrieved fill information
    current_positions[fill.symbol] += fill_dir*fill.quantity;
}

// Updates holdings from a FillEvent
// To estimate the cost of a fill, uses the closing price of the last bar
void NaivePortfolio::update_holdings_from_fill(FillEvent fill) {
    int fill_dir = 0;
    if (fill.direction == "BUY") {
        fill_dir = 1;
    } else if (fill.direction == "SELL") {
        fill_dir = -1;
    }
    
    // Update holdings list with calculated fill information
    double fill_cost = bars.get_latest_bars(fill.symbol, 1)["close"].end()->second;
    double cost = fill_dir * fill_cost * fill.quantity;
    current_holdings[fill.symbol] += cost;
    current_holdings["commission"] += fill.commission;
    current_holdings["heldcash"] -= (cost + fill.commission);
    current_holdings["totalholdings"] -= (cost + fill.commission);
}

// Updates positions and holdings in case of fill event
void NaivePortfolio::update_fill(Event* event) {
    if (event->type == "FILL") {
        FillEvent* fill = dynamic_cast<FillEvent*>(event);
        update_positions_from_fill(*fill);
        update_holdings_from_fill(*fill);
    }
}
void NaivePortfolio::update_signal(Event* event) {
    if (event->type == "SIGNAL") {
        SignalEvent* signal = dynamic_cast<SignalEvent*>(event);
        OrderEvent orderevent = generate_naive_order(*signal);
        events.push_back(orderevent);
    }
}

// Generates order signal for 100 or so shares of each asset
// No risk management or position sizing; to be implemented later
OrderEvent NaivePortfolio::generate_naive_order(SignalEvent signal) {
    
    OrderEvent order = OrderEvent();
    
    string symbol = signal.symbol;
    string direction = signal.signal_type;
    double strength = signal.strength;
    
    int mkt_quantity = floor(100 * strength);
    int cur_quantity = current_positions[symbol];
    string order_type = "MKT";
    
    // Order logic
    if (direction == "LONG" && cur_quantity == 0) { order = OrderEvent(symbol, order_type, mkt_quantity, "BUY"); }
    if (direction == "SHORT" && cur_quantity == 0) { order = OrderEvent(symbol, order_type, mkt_quantity, "SELL"); }
    
    if (direction == "EXIT" && cur_quantity > 0) { order = OrderEvent(symbol, order_type, abs(cur_quantity), "SELL"); }
    if (direction == "EXIT" && cur_quantity < 0) { order = OrderEvent(symbol, order_type, abs(cur_quantity), "BUY"); }
    
    return order;
}

// Create returns stream for performance calculations
void NaivePortfolio::create_equity_curve() {
    equity_curve = all_holdings;
    double previoustotal = -1000000000.0;
    double previouscurve = 1.0;
    for (std::map<long, map<string, double>>::iterator it=equity_curve.begin(); it!=equity_curve.end(); ++it) {
        if (previoustotal == -1000000000.0) {
            equity_curve[it->first]["equitycurve"] = previouscurve;
            continue;
        }
        double returns = ((it->second["totalholdings"])/previoustotal) - 1;
        equity_curve[it->first]["returns"] = returns;
        previouscurve *= returns;
        equity_curve[it->first]["equitycurve"] = previouscurve;
        previoustotal = it->second["totalholdings"];
    }
}
