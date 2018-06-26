//
//  execution.cpp
//  Backtest Environment
//
//  Created by Evan Kirkiles on 6/8/18.
//  Copyright © 2018 Evan Kirkiles. All rights reserved.
//

#include "execution.hpp"

using namespace std;

// MARK: Simulated Execution Handler
// Should be redone because executes trades at current market price with no slippage or impact

// Initialize pointer to event list
SimulatedExecutionHandler::SimulatedExecutionHandler(boost::ptr_vector<Event>* events, HistoricalCSVDataHandler* i_data) {
    eventlist = events;
    data = i_data;
}

// Order filling and execution
// NYSE parameter is pretty meaningless because no brokerage being used
// Also no latency, slippage, or fill ratio problems
void SimulatedExecutionHandler::execute_order(OrderEvent event) {

    double slippage = 0;
    int quantity = event.quantity;
    
    // Fill_cost is 1 here because it is taken care of by the Naive Portfolio object
    // Commission is -1 so it will be calculated by the FillEvent
    if (event.target == "ALGO") {
        std::pair<int, double> pair = calculate_slippage(event);
        quantity = pair.first;
        slippage = pair.second;
    }

    eventlist->push_back(new FillEvent(data->latestDates.back(), event.symbol, "NYSE", quantity, event.direction, slippage, -1, event.target));
}

// Calculates slippage based on certain parameters (read more at https://www.quantopian.com/help#ide-slippage)
// 1. May not order more than 2.5% of the trading volume of the previous day.
// 2. There is a base slippage rate of 5 bips (0.05%) on each order
std::pair<int, double> SimulatedExecutionHandler::calculate_slippage(OrderEvent event) {

    // Fill direction normalization
    int fill_dir = 0;
    if (event.direction == "BUY") {
        fill_dir = 1;
    } else if (event.direction == "SELL") {
        fill_dir = -1;
    }
    // Variables to be returned in the pair
    int quantity = event.quantity;
    double slippage = 0;

    // How many bips to dock from your fill
    double basisPointDeduc = 5;
    // Max percent of the day's trading volume allowed to be ordered
    double volumeLimit = 0.025;

    // Check if the quantity is greater than [volumelimit] of the previous day's trading volume
    double volume = data->latest_data[event.symbol]["volume"].rbegin().operator*().second;
    double lastclose = data->latest_data[event.symbol]["close"].rbegin().operator*().second;
    if (quantity > (volume * volumeLimit)) {

        // Log the failure to fill full order
        cout << "Failed to fill " << to_string((quantity - (volume * volumeLimit)) * fill_dir) << " shares of " << event.symbol << "." << endl;
        quantity = (int)floor(volume * volumeLimit);
    }

    // Slippage is the correct number of basis points deducted from fill cost
    slippage = quantity * lastclose * (basisPointDeduc/10000);

    return make_pair(quantity, slippage);
}

