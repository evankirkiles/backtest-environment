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

// Initialize instance of event list
SimulatedExecutionHandler::SimulatedExecutionHandler(vector<Event> events) {
    eventlist = events;
}

// Order filling and execution
// ONLY EXECUTES ON NYSE; NEED TO IMPLEMENT PARAMETER FOR OTHER EXCHANGES
// Also no latency, slippage, or fill ratio problems
void SimulatedExecutionHandler::execute_order(Event* event) {
    if (event->type == "ORDER") {
        OrderEvent* order = dynamic_cast<OrderEvent*>(event);
        FillEvent filledorder = FillEvent(long(chrono::duration_cast<std::chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count()), order->symbol, "NYSE", order->quantity, order->direction, 0);
        eventlist.push_back(filledorder);
    }
}

