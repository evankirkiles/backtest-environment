//
//  main.cpp
//  Backtest Environment
//
//  Created by Evan Kirkiles on 5/25/18.
//  Copyright © 2018 Evan Kirkiles. All rights reserved.
//

#include <iostream>
#include "DataHandling/YahooFinanceCSVReader.hpp"
#include "Infrastructure/interface.hpp"
#include "Graphics/gnuplotter.hpp"

#include <QApplication>
#include "Graphics/qtwindow.hpp"

int main(int argc, char **argv) {

    auto startdate = (char*)"2007-01-01";
    auto enddate = (char*)"2010-01-01";
    vector<string> symbol_list = {string("GS"), string("AAPL"), string("SPY")};
    vector<string> symbol_list2 = {string("SPY")};
    
    TradingInterface interface = TradingInterface(symbol_list, symbol_list2, 10000000, startdate, enddate);
    BuyAndHoldStrategy strat = BuyAndHoldStrategy(&interface.pipeline, &interface.events);
    Benchmark bench = Benchmark(&interface.benchmarkpipeline, &interface.events);
    GNUPlotter gnuplot(&interface.portfolio,
                       &interface.benchmarkportfolio, (char*)"/Users/samkirkiles/Desktop/algobacktester/Graphics/data.csv", (char*)"/Users/samkirkiles/Desktop/algobacktester/Graphics/positions.csv", get_epoch_time(startdate), get_epoch_time(enddate), true);
    gnuplot.initPlot();

    // Begin the backtest on the created plot device
    //interface.runbacktest(strat, bench, &gnuplot);

    QApplication app(argc, argv);

    Window window(&interface, &strat, &bench, &gnuplot);
    window.show();
    window.raise();

    app.exec();

    return 0;
}
