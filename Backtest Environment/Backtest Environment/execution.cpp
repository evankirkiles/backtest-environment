//
//  execution.cpp
//  Backtest Environment
//
//  Created by Evan Kirkiles on 6/8/18.
//  Copyright © 2018 Evan Kirkiles. All rights reserved.
//

#include "execution.hpp"
#ifndef chrono
#include <chrono>
#endif

using namespace std;

// MARK: Simulated Execution Handler
// Should be redone because executes trades at current market price with no slippage or impact

// Initialize pointer to event list
SimulatedExecutionHandler::SimulatedExecutionHandler(boost::ptr_vector<Event>* events) {
    eventlist = events;
}

// Order filling and execution
// ONLY EXECUTES ON NYSE; NEED TO IMPLEMENT PARAMETER FOR OTHER EXCHANGES
// Also no latency, slippage, or fill ratio problems
void SimulatedExecutionHandler::execute_order(OrderEvent event) {
    
    // Fill_cost is 1 here because it is taken care of by the Naive Portfolio object
    eventlist->push_back(new FillEvent(long(chrono::duration_cast<std::chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count()), event.symbol, "NYSE", event.quantity, event.direction, 1));
}

