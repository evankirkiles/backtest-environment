//
//  interface.hpp
//  Backtest Environment
//
//  Created by Evan Kirkiles on 6/9/18.
//  Copyright © 2018 Evan Kirkiles. All rights reserved.
//

#ifndef interface_hpp
#define interface_hpp
#ifndef string
#include <string>
#endif
#ifndef vector
#include <vector>
#endif
#ifndef iostream
#include <iostream>
#endif

#include <stdio.h>
#include "strategiesheader.hpp"
#include "execution.hpp"
#include "portfolio.hpp"

#include <boost/ptr_container/ptr_vector.hpp>

// Create a class for different types of trading interfaces
class TradingInterface {
public:
    
    // VARIABLES
    // executor: logic for executing trades (acts as a broker, essentially)
    // portfolio: portfolio that converts orders into fills and sets rules
    // pipeline: Data Handler instantiated from data.cpp
    // events: global list of events
    
    vector<string> symbol_list;
    double initial_capital;
    bool continue_backtest;
    long startdate;
    
    boost::ptr_vector<Event> events;
    SimulatedExecutionHandler executor;
    NaivePortfolio portfolio;
    HistoricalCSVDataHandler pipeline;
    
    // PARAMS
    // symbol_list: list of symbols to be traded
    // initial_cap: initial amount of capital allocated to algorithm
    // strategy: the algorithm whose signals are sent to the portfolio
    TradingInterface(vector<string>symbol_list, double initial_cap, long start_date);
    
    // Executes the while loop for running the backtest
    void runbacktest(BuyAndHoldStrategy strategy);
};

#endif /* interface_hpp */
