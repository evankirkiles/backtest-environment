//
//  interface.cpp
//  Backtest Environment
//
//  Created by Evan Kirkiles on 6/9/18.
//  Copyright © 2018 Evan Kirkiles. All rights reserved.
//

#include "interface.hpp"

using namespace std;

// Constructor that initializes the executor and replaces the empty portfolio and pipeline with functioning ones
TradingInterface::TradingInterface(vector<string>*i_symbol_list, vector<string>i_benchmarksymbols, double* i_initial_cap, string *i_start_date, string *i_end_date) : executor(&events, &pipeline) {
    
    // Initialize variables inputted in constructor
    symbol_list = i_symbol_list;
    benchmarksymbols = i_benchmarksymbols;
    initial_capital = i_initial_cap;
    startdate = i_start_date;
    enddate = i_end_date;
    continue_backtest = 0;

    // Create data handler and portfolio management
    pipeline = HistoricalCSVDataHandler(&events, symbol_list, startdate, enddate, &continue_backtest);
    portfolio = NaivePortfolio(&pipeline, symbol_list, &events, startdate, enddate, initial_capital);

    // Initialize benchmark portfolio and data
    benchmarkpipeline = HistoricalCSVDataHandler(&events, &benchmarksymbols, startdate, enddate, &continue_backtest);
    benchmarkportfolio = NaivePortfolio(&benchmarkpipeline, &benchmarksymbols, &events, startdate, enddate, initial_capital);
}

// Begins the backtest!
void TradingInterface::runbacktest(MainStrategy strategy, Benchmark i_benchmark, GNUPlotter* plot) {
    continue_backtest = 1;
    portfolio.format_portfolio();
    pipeline.format_csv_data();
    pipeline.update_bars();
    benchmarkportfolio.format_portfolio();
    benchmarkpipeline.format_csv_data();
    benchmarkpipeline.update_bars();

    strat = strategy;
    benchmark = i_benchmark;

    cout << "Initializing backtest..." << endl;

    int instcontinue = continue_backtest;

    // Event-driven loop that continues to check for events
    while (instcontinue == 1) {

        // Handles each event in the list and removes it from the stack
        if (!events.empty()) {
            if (events[0].type == "MARKET") {
                auto marketevent = dynamic_cast<MarketEvent *>(&events[0]);

                // In case of a MarketEvent, use updated data to calculate next strategy's next move and send a signal
                strat.calculate_signals(*marketevent);
                benchmark.calculate_signals(*marketevent);

            } else if (events[0].type == "SIGNAL") {
                auto signalevent = dynamic_cast<SignalEvent *>(&events[0]);

                // In case of a SignalEvent, portfolio sends necessary orders based on signal send by strategy
                // Determine who is target for the signal event
                if (events[0].target == "ALGO") {
                    portfolio.update_signal(*signalevent);
                } else if (events[0].target == "BENCH") {
                    benchmarkportfolio.update_signal(*signalevent);
                }

            } else if (events[0].type == "ORDER") {
                auto orderevent = dynamic_cast<OrderEvent *>(&events[0]);

                // In case of an OrderEvent, the execution handler fills the received order (like a brokerage)
                executor.execute_order(*orderevent);
                orderevent->print_order();

            } else if (events[0].type == "FILL") {
                auto fillevent = dynamic_cast<FillEvent *>(&events[0]);

                // In case of a FillEvent, the portfolio updates its information based on the fill information
                // Determine who is target for the fill event
                if (events[0].target == "ALGO") {
                    cout << "Filled " << fillevent->symbol << " at " << get_std_time(fillevent->timeindex)<< endl;
                    portfolio.update_fill(*fillevent);
                } else if (events[0].target == "BENCH") {
                    benchmarkportfolio.update_fill(*fillevent);
                }
            }
            events.erase(events.begin());
        } else {

            /*
            cout << "Total Returns: " << portfolio.all_holdings.rbegin().operator*().second["equitycurve"] * 100 <<
                 "% Mean: " << portfolio.all_holdings.rbegin().operator*().second["mean"] * 100 <<
                 " Variance: " << portfolio.all_holdings.rbegin().operator*().second["variance"] * 100*100 <<
                 " Sharpe: " << portfolio.all_holdings.rbegin().operator*().second["sharpe"] <<
                 " Drawdown: " << portfolio.all_holdings.rbegin().operator*().second["drawdown"] * 100 <<
                 " HWM: " << portfolio.all_holdings.rbegin().operator*().second["highwatermark"] * 100 << endl;
                 */

            if (continue_backtest == 0) {
                instcontinue = 0;
                benchmarkportfolio.update_timeindex();
                portfolio.update_timeindex();
                plot->updatePlot();
                break;
            }

            benchmarkportfolio.update_timeindex();
            benchmarkpipeline.update_bars();
            portfolio.update_timeindex();
            pipeline.update_bars();
            plot->updatePlot();
        }
    }
}