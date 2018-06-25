//
//  main.cpp
//  Backtest Environment
//
//  Created by Evan Kirkiles on 5/25/18.
//  Copyright © 2018 Evan Kirkiles. All rights reserved.
//

#include <iostream>
#include "DataHandling/YahooFinanceCSVReader.hpp"
#include "Graphics/qtwindow.hpp"

#include <QApplication>

int main(int argc, char **argv) {

    // Placeholder starting global variables
    auto startdate = string("1970-01-01");
    auto enddate = string("1971-01-01");
    double initialcapital = 100000000;
    int showholdings = 0;
    vector<string> symbol_list = {string("SPY")};
    vector<string> symbol_list2 = {string("SPY")};

    TradingInterface interface = TradingInterface(&symbol_list, {string("SPY")}, &initialcapital, &startdate, &enddate);
    MainStrategy strat = MainStrategy(&interface.pipeline, &interface.events);
    Benchmark bench = Benchmark(&interface.benchmarkpipeline, &interface.events);
    GNUPlotter gnuplot(&interface.portfolio,
                       &interface.benchmarkportfolio, (char*)"./Graphics/data.csv", (char*)"./Graphics/positions.csv", &startdate, &enddate, &showholdings);

    // Run the backtest window
    QApplication app(argc, argv);

    AlgoWindow window(&interface, &strat, &bench, &gnuplot, &startdate, &enddate, &initialcapital, &showholdings);
    window.show();
    window.raise();

    app.exec();

    return 0;
}
