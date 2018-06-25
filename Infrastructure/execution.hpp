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
#ifndef ptr_vector
#include <boost/ptr_container/ptr_vector.hpp>
#endif
#ifndef chrono
#include <chrono>
#endif

#ifndef Event
#include "events.hpp"
#endif
#ifndef HistoricalCSVDataHandler
#include "data.hpp"
#endif

// Base execution handler to which we can add features
class ExecutionHandler {
public:
    boost::ptr_vector<Event>* eventlist;
    HistoricalCSVDataHandler* data;
    virtual void execute_order(OrderEvent event) = 0;
};

// Simulated execution handler that assumes all orders are filled at the current market price
// for all quantities; need more sophisticated slippage and market impact
class SimulatedExecutionHandler: ExecutionHandler {
public:
    // Constructor
    SimulatedExecutionHandler(boost::ptr_vector<Event>* events, HistoricalCSVDataHandler* data);

    // Placeholder constructor
    SimulatedExecutionHandler() = default;
    
    // Inherited order execution system
    void execute_order(OrderEvent event);

    // Slippage calculations
    std::pair<int, double> calculate_slippage(OrderEvent event);
};

#endif /* execution_hpp */
