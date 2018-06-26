
//
//  portfolio.hpp
//  Backtest Environment
//
//  Created by Evan Kirkiles on 6/2/18.
//  Copyright © 2018 Evan Kirkiles. All rights reserved.
//

#ifndef portfolio_hpp
#define portfolio_hpp

#include <cstdio>
#ifndef Event
#include "events.hpp"
#endif

#include "data.hpp"
#ifndef setprecision
#include <iomanip>
#endif
#ifndef stringstream
#include <sstream>
#endif
#ifndef QLabel
#include <QLabel>
#endif

using namespace std;

// Abstract base portfolio class so cannot be instantiated directly
class Portfolio {
public:
    // Generate new signal based on portfolio logic
    virtual void update_signal(SignalEvent event)=0;
    // Update portfolio current position and holdings
    virtual void update_fill(FillEvent event)=0;
};

// Placeholder 'naive' portfolio that has minimal logic behind its ordering
// Used to test simple strategies
class NaivePortfolio:Portfolio {
public:
    HistoricalCSVDataHandler* bars;
    boost::ptr_vector<Event>* events;
    vector<string>* symbol_list;
    string *start_date;
    string *end_date;
    double *initial_capital;
    
    map<long, map<string, double>> all_positions;
    map<string, double> current_positions;
    
    map<long, map<string, double>> all_holdings;
    vector<double> returns_stream;
    map<string, double> current_holdings;
    
    // Performance map for all necessary evaluation statistics
    map<string, double> perfmap;
    
    // Initialization function
    NaivePortfolio(HistoricalCSVDataHandler* i_bars, vector<string>* i_symbol_list, boost::ptr_vector<Event>* i_events, string *i_start_date, string *i_end_date, double *i_initial_capital);
    
    // Placeholder initialization function
    NaivePortfolio();

    // Initializes all positions and holdings by calling below functions
    void format_portfolio();
    
    // Functions to construct positions and holdings
    map<long, map<string, double>> construct_all_positions();
    map<string, double> construct_current_positions();
    map<long, map<string, double>> construct_all_holdings();
    map<string, double> construct_current_holdings();
    
    // Updates the holdings tracking
    void update_timeindex();
    
    // Receives a FillEvent and changes the positions and holdings to correctly reflect the event
    void update_positions_from_fill(FillEvent event);
    void update_holdings_from_fill(FillEvent event);

    // Parent portfolio functions
    void update_signal(SignalEvent event);
    void update_fill(FillEvent event);
    
    // Send order for 100 shares of each asset
    void generate_naive_order(SignalEvent signal);
    
    // Function to calculate how many shares need to be bought for the given percentage
    int calculate_quantity(string symbol, double percentage);

    // MARK: Performance functions
    // Mean and variance of the returns stream of the equity curve
    void update_MeanAndVariance(long date);

    // Sharpe ratio (https://www.investopedia.com/terms/s/sharperatio.asp)
    void update_Sharpe(long date, int periods=252);

    // High-Water Mark (https://www.investopedia.com/terms/h/highwatermark.asp)
    void update_HighWaterMark(long date);

    // Drawdowns (https://www.investopedia.com/terms/d/drawdown.asp)
    void update_Drawdowns(long date);

    // Retrieves performance stats about the algorithm
    map<string, double> getPerformanceStats(NaivePortfolio benchmark);
};

#endif /* portfolio_hpp */
