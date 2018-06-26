//
//  portfolio.cpp
//  Backtest Environment
//
//  Created by Evan Kirkiles on 6/2/18.
//  Copyright © 2018 Evan Kirkiles. All rights reserved.
//

#include "portfolio.hpp"

using namespace std;

// MARK: Naive Portfolio

// Fake constructor
NaivePortfolio::NaivePortfolio() { };

// Constructor
NaivePortfolio::NaivePortfolio(HistoricalCSVDataHandler* i_bars, vector<string>*i_symbol_list, boost::ptr_vector<Event>* i_events, string *i_start_date, string *i_end_date, double *i_initial_capital) {
    
    // Initialize all instance variables
    bars = i_bars;
    events = i_events;
    start_date = i_start_date;
    end_date = i_end_date;
    initial_capital = i_initial_capital;
    symbol_list = i_symbol_list;
}

void NaivePortfolio::format_portfolio() {
    // Create positions and holdings
    all_positions = construct_all_positions();
    current_positions = construct_current_positions();
    all_holdings = construct_all_holdings();
    current_holdings = construct_current_holdings();

    // Update performance map
    perfmap.clear();
    perfmap["maxdrawdown"] = 0;
    perfmap["maxddperiod"] = 0;
}

// Positions & holdings constructors
map<long, map<string, double>> NaivePortfolio::construct_all_positions() {
    map<string, double> temp = {};
    for (int i = 0; i < symbol_list->size(); i++) {
        temp[symbol_list->at(i)] = 0;
    }
    map<long, map<string, double>> c;
    c[get_epoch_time(*start_date)] = temp;
    return c;
}
map<string, double> NaivePortfolio::construct_current_positions() {
    map<string, double> temp = {};
    for (int i = 0; i < symbol_list->size(); i++) {
        temp[symbol_list->at(i)] = 0;
    }
    return temp;
}
map<long, map<string, double>> NaivePortfolio::construct_all_holdings() {
    map<string, double> temp = {};
    for (int i = 0; i < symbol_list->size(); i++) {
        temp[symbol_list->at(i)] = 0;
    }
    temp["heldcash"] = *initial_capital;
    temp["commission"] = 0.0;
    temp["slippage"] = 0.0;
    temp["totalholdings"] = *initial_capital;
    
    // Equity curve data
    temp["returns"] = 0.0;
    temp["equitycurve"] = 0.0;

    // Necessary performance data
    temp["mean"] = 0.0;
    temp["variance"]= 0.0;
    temp["sharpe"] = 0.0;
    temp["highwatermark"] = 0.0;
    temp["drawdown"] = 0.0;
    temp["ddperiod"] = 0.0;
    
    map<long, map<string, double>> c;
    c[get_epoch_time(*start_date)] = temp;
    return c;
}
map<string, double> NaivePortfolio::construct_current_holdings() {
    map<string, double> temp = {};
    for (int i = 0; i < symbol_list->size(); i++) {
        temp[symbol_list->at(i)] = 0;
    }
    temp["heldcash"] = *initial_capital;
    temp["commission"] = 0.0;
    temp["slippage"] = 0.0;
    temp["totalholdings"] = *initial_capital;
    return temp;
}

// Update holdings evaluations with most recently completed bar (previous day)
void NaivePortfolio::update_timeindex() {
    map<string, map<string, map<long, double>>> lastbar;
    long date;
    double sumvalues = 0;
    double previoustotal = all_holdings.rbegin().operator*().second["totalholdings"];
    double previouscurve = all_holdings.rbegin().operator*().second["equitycurve"];
    
    for (int i=0; i<symbol_list->size();i++) {
        string symbol = symbol_list->at(i);
        
        // Update positions
        lastbar[symbol] = bars->get_latest_bars(symbol,1);
        
        date = lastbar[symbol]["open"].rbegin()->first;
        all_positions[date][symbol] = current_positions[symbol];
        
        // Update holdings
        // Estimates market value of a stock by using the quantity * its closing price (most likely inaccurate)
        double market_value = current_positions[symbol] * lastbar[symbol]["close"][date];
        all_holdings[date][symbol] = market_value;
        current_holdings[symbol] = market_value;
        sumvalues += market_value;
    }
    
    current_holdings["totalholdings"] = current_holdings["heldcash"] + sumvalues;
    all_holdings[date]["totalholdings"] = current_holdings["heldcash"] + sumvalues;
    all_holdings[date]["heldcash"] = current_holdings["heldcash"];
    all_holdings[date]["commission"] = current_holdings["commission"];
    all_holdings[date]["slippage"] = current_holdings["slippage"];
    
    // Equity curve and performance handling
    if (all_holdings.size() > 1) {

        // Equity curve updating
        double returns = (all_holdings[date]["totalholdings"] / previoustotal) - 1;
        all_holdings[date]["returns"] = returns;
        returns_stream.push_back(returns);
        all_holdings[date]["equitycurve"] = (previouscurve + 1) * (returns + 1) - 1;

        // Update the performance values
        update_MeanAndVariance(date);
        update_Sharpe(date);
        update_HighWaterMark(date);
        update_Drawdowns(date);
    } else {
        returns_stream.push_back(1);
    }
}

// Update positions from a FillEvent
void NaivePortfolio::update_positions_from_fill(FillEvent fill) {
    int fill_dir = 0;
    if (fill.direction == "BUY") {
        fill_dir = 1;
    } else if (fill.direction == "SELL") {
        fill_dir = -1;
    }
    
    // Update the positions with retrieved fill information
    current_positions[fill.symbol] += fill_dir*fill.quantity;
}

// Updates holdings from a FillEvent
// To estimate the cost of a fill, uses the closing price of the last bar
void NaivePortfolio::update_holdings_from_fill(FillEvent fill) {
    int fill_dir = 0;

    // Makes sure that selling shares one does not have is correctly subtracted from the total cash
    // The same situation is also true for buying shares when already short
    if (fill.direction == "BUY") {
        fill_dir = 1;
    } else if (fill.direction == "SELL") {
        fill_dir = -1;
    }

    // Default slippage is 0
    // Update holdings list with calculated fill information
    double fill_cost = bars->get_latest_bars(fill.symbol, 1)["close"].rbegin()->second;
    double cost = fill_dir * fill_cost * fill.quantity;
    current_holdings[fill.symbol] += cost;
    current_holdings["commission"] += fill.commission;
    current_holdings["slippage"] += fill.slippage;
    current_holdings["heldcash"] -= (cost + fill.commission + fill.slippage);
    current_holdings["totalholdings"] -= (fill.commission + fill.slippage);
}

// Updates positions and holdings in case of fill event
void NaivePortfolio::update_fill(FillEvent event) {
    update_holdings_from_fill(event);
    update_positions_from_fill(event);
}
void NaivePortfolio::update_signal(SignalEvent event) {
    generate_naive_order(event);
}

// Generates order signal for 100 or so shares of each asset
// No risk management or position sizing; to be implemented later
void NaivePortfolio::generate_naive_order(SignalEvent signal) {
    
    string symbol = signal.symbol;
    double strength = signal.strength;
    
    int mkt_quantity = calculate_quantity(symbol, strength);
    string order_type = "MKT";
    
    // Order logic
    if (mkt_quantity > 0) {
        events->push_back(new OrderEvent(symbol, order_type, abs(mkt_quantity), "BUY", signal.target));
    } else if (mkt_quantity < 0) {
        events->push_back(new OrderEvent(symbol, order_type, abs(mkt_quantity), "SELL", signal.target));
    } else {
        cout << "Already holding " << strength << "% in " << symbol << ".";
    }
}

// Calculates the number of shares needed to fill the given percentage (rounds down)
int NaivePortfolio::calculate_quantity(string symbol, double percentage) {
    
    // If want to completely exit, then just return the opposite position size
    if (percentage == 0) {
        return -current_positions[symbol];
    }
    
    // Otherwise, calculate required quantity of shares
    double currentposition = current_positions[symbol];
    double sellingprice = bars->get_latest_bars(symbol, 1)["close"].rbegin()->second;
    double requiredcash = (percentage * all_holdings.rbegin().operator*().second["totalholdings"]) - (currentposition * sellingprice);
    
    // Commission included in calculation
    if (requiredcash <= 500 * (sellingprice + 0.013)) {
        return (int)floor(requiredcash  / (sellingprice + 0.013));
    } else {
        return (int)floor(requiredcash  / (sellingprice + 0.008));
    }
}

// MARK: Performance calculation functions
// Mean and variance of the returns stream of the equity curve
void NaivePortfolio::update_MeanAndVariance(long date) {

    // Assumes there is no risk-free rate or benchmark return being used in calculation (value is 0)
    double previousMean = next(all_holdings.rbegin(), 1).operator*().second["mean"];
    double previousVariance = next(all_holdings.rbegin(), 1).operator*().second["variance"];
    double newReturn = all_holdings.rbegin().operator*().second["returns"];
    long n = all_holdings.size();

    // Incremental updating of mean and variance
    // (https://math.stackexchange.com/questions/102978/incremental-computation-of-standard-deviation)
    double mean = previousMean + ((newReturn - previousMean) / n);
    all_holdings[date]["mean"] = mean;
    all_holdings[date]["variance"] = ((previousVariance*(n-1)) + ((newReturn - previousMean) * (newReturn - mean)))/n;
}

// Sharpe Ratio of the portfolio
void NaivePortfolio::update_Sharpe(long date, int periods) {

    double mean = all_holdings.rbegin().operator*().second["mean"];
    double variance = all_holdings.rbegin().operator*().second["variance"];

    all_holdings[date]["sharpe"] = sqrt(periods) * (mean/sqrt(variance));
}

// High-Water Mark used to calculate drawdowns
void NaivePortfolio::update_HighWaterMark(long date) {

    double prevHWM = next(all_holdings.rbegin(), 1).operator*().second["highwatermark"];
    double currReturn = all_holdings.rbegin().operator*().second["equitycurve"];

    // Simply keeps the highest returns up to that date
    if (prevHWM > currReturn) {
        all_holdings[date]["highwatermark"] = prevHWM;
        all_holdings[date]["ddperiod"]++;
    } else {
        if (all_holdings[date]["ddperiod"] > perfmap["maxddperiod"]) {
            perfmap["maxddperiod"] = all_holdings[date]["ddperiod"];
        }
        all_holdings[date]["highwatermark"] = currReturn;
        all_holdings[date]["ddperiod"] = 0;
    }
}

// Drawdown value and period
void NaivePortfolio::update_Drawdowns(long date) {

    double hwm = all_holdings.rbegin().operator*().second["highwatermark"];
    double returns = all_holdings.rbegin().operator*().second["equitycurve"];

    all_holdings[date]["drawdown"] = hwm - returns;

    if (all_holdings[date]["drawdown"] > perfmap["maxdrawdown"]) {
        perfmap["maxdrawdown"] = all_holdings[date]["drawdown"];
    }
}

// Run at the end of the backtest, calculates final performance statistics in readable format
// Total return, alpha, beta, Sharpe ratio, max drawdown, max drawdown period, highwater mark
map<string, double> NaivePortfolio::getPerformanceStats(NaivePortfolio benchmark) {

    // Total return
    perfmap["totalreturn"] = all_holdings.rbegin().operator*().second["equitycurve"];
    // Sharpe ratio
    perfmap["sharpe"] = all_holdings.rbegin().operator*().second["sharpe"];
    // Highwater mark
    perfmap["hwm"] = all_holdings.rbegin().operator*().second["highwatermark"];

    // Beta
    // Covariance (http://ci.columbia.edu/ci/premba_test/c0331/s7/s7_5.html)
    double mean = all_holdings.rbegin().operator*().second["mean"];
    double mkt_mean = benchmark.all_holdings.rbegin().operator*().second["mean"];
    double cov = 0;
    for (int i=0; i < bars->allDates.size(); i++) {
        long date = bars->allDates[i];
        cov += ((all_holdings[date]["returns"] - mean) * (benchmark.all_holdings[date]["returns"] - mkt_mean));
    }
    cov /= bars->allDates.size()-1;
    // Variance of the market's returns
    double variance = benchmark.all_holdings.rbegin().operator*().second["variance"];
    perfmap["beta"] = cov/variance;

    // Alpha
    double returns = all_holdings.rbegin().operator*().second["equitycurve"];
    double mkt_return = next(benchmark.all_holdings.rbegin(), 1).operator*().second["equitycurve"];
    perfmap["alpha"] = returns - (mkt_return * (cov/variance));

    // Mean & variance
    perfmap["mean"] = mean;
    perfmap["variance"] = all_holdings.rbegin().operator*().second["variance"];

    return perfmap;
}