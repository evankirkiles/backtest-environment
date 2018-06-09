//
//  execution.hpp
//  Backtest Environment
//
//  Created by Evan Kirkiles on 6/8/18.
//  Copyright © 2018 Evan Kirkiles. All rights reserved.
//

#ifndef execution_hpp
#define execution_hpp
#ifndef vector
#include <vector>
#endif

#include <stdio.h>
#include "events.hpp"

// Base execution handler to which we can add features
class ExecutionHandler {
public:
    vector<Event>eventlist;
    virtual void execute_order(Event* event) = 0;
};

// Simulated execution handler that assumes all orders are filled at the current market price
// for all quantities; need more sophisticated slippage and market impact
class SimulatedExecutionHandler: ExecutionHandler {
    // Constructor
    SimulatedExecutionHandler(vector<Event> events);
    
    // Inherited order execution system
    void execute_order(Event* event);
};

#endif /* execution_hpp */
