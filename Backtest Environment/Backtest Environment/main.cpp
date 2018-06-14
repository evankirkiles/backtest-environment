//
//  main.cpp
//  Backtest Environment
//
//  Created by Evan Kirkiles on 5/25/18.
//  Copyright © 2018 Evan Kirkiles. All rights reserved.
//

#include <iostream>
#include "YahooFinanceCSVReader.hpp"
#include "interface.hpp"
#include "gnuplotter.hpp"


int main(int argc, const char * argv[]) {

    char* startdate = (char*)"2007-01-01";
    char* enddate = (char*)"2010-01-01";
    vector<string> symbol_list = {string("GS"), string("AAPL"), string("SPY")};
    vector<string> symbol_list2 = {string("SPY")};
    
    TradingInterface interface = TradingInterface(symbol_list, symbol_list2, 10000000, startdate, enddate);
    BuyAndHoldStrategy strat = BuyAndHoldStrategy(&interface.pipeline, &interface.events);
    Benchmark bench = Benchmark(&interface.benchmarkpipeline, &interface.events);
    GNUPlotter gnuplot(&interface.portfolio, &interface.benchmarkportfolio, (char*)"/Users/samkirkiles/Desktop/Backtest Environment/Backtest Environment/Backtest Environment/Graphics/data.csv", get_epoch_time(startdate), get_epoch_time(enddate));
    gnuplot.initPlot();
    
    // Begin the backtest on the created plot device
    interface.runbacktest(strat, bench, &gnuplot);
    
    return 0;
}
