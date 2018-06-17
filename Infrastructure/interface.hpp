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
#include "portfolio.hpp"
#include "../Graphics/gnuplotter.hpp"
#include "../Strategies/strategiesheader.hpp"
#include "execution.hpp"

#include <boost/ptr_container/ptr_vector.hpp>

// Create a class for different types of trading interfaces
class TradingInterface {
public:
    
    // VARIABLES
    // executor: logic for executing trades (acts as a broker, essentially)
    // portfolio: portfolio that converts orders into fills and sets rules
    // pipeline: Data Handler instantiated from data.cpp
    // events: global list of events
    
    vector<string>* symbol_list;
    vector<string> benchmarksymbols;
    double *initial_capital;
    int continue_backtest;
    string *startdate;
    string *enddate;
    
    // Global events and execution handler
    boost::ptr_vector<Event> events;
    SimulatedExecutionHandler executor;
    
    // Strategy setup
    NaivePortfolio portfolio;
    HistoricalCSVDataHandler pipeline;
    MainStrategy strat;
    
    // Benchmark setup
    HistoricalCSVDataHandler benchmarkpipeline;
    NaivePortfolio benchmarkportfolio;
    Benchmark benchmark;

    // PARAMS
    // symbol_list: list of symbols to be traded
    // initial_cap: initial amount of capital allocated to algorithm
    // strategy: the algorithm whose signals are sent to the portfolio
    TradingInterface(vector<string>*symbol_list, vector<string>benchmarksymbols, double *initial_cap, string *start_date, string *end_date);

    // Placeholder initializer
    TradingInterface() = default;

    // Executes the while loop for running the backtest
    void runbacktest(MainStrategy strategy, Benchmark benchmark, GNUPlotter* plot);
};

#endif /* interface_hpp */