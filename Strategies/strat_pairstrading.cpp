//
// Created by Evan Kirkiles on 6/18/18.
//

//
// Created by Evan Kirkiles on 6/17/18.
//

#include "strategiesheader.hpp"

#ifndef ptr_vector
#include <boost/ptr_container/ptr_vector.hpp>
#endif
#include <gsl/gsl_fit.h>
#include <gsl/gsl_statistics.h>

// Initialize strategy
MainStrategy::MainStrategy(HistoricalCSVDataHandler* i_bars, boost::ptr_vector<Event>*i_events) {

    // Set default instance variables
    bars = i_bars;
    events = i_events;
    *bars->symbol_list = {string("MA"),string("V")};
    symbol_list = bars->symbol_list;
    title = "Pairs Trading";

    // Set custom strategy variables

}

// Update map of bought symbols
void MainStrategy::calculate_signals(MarketEvent i_event) {

    // Initialize map of closing prices for past -lookback- days
    map<string, map<long, double>> closeprices;
    for (int i = 0; i < symbol_list->size(); i++) {
        closeprices[symbol_list->at(i)] = bars->get_latest_bars(symbol_list->at(i), lookback)["close"];
    }

    // Initialize price vectors for use in calculations
    vector<double>Y;
    vector<double>X;
    for(auto it = closeprices[symbol_list->at(0)].begin();
        it != closeprices[symbol_list->at(0)].end(); ++it) {
        Y.push_back(closeprices[symbol_list->at(0)][it->first]);
        X.push_back(closeprices[symbol_list->at(1)][it->first]);
    }

    // Calculate hedge ratio to get slope of linear regression between the stock prices
    double hedge = calculate_hedge_ratio(Y, X);

    // Now use hedge ratio to calculate new spread between the two stocks
    double newspread = Y.back() - hedge * X.back();
    //cout << "newspread: " << newspread << " Y: " << Y.back() << " X: " << X.back() << " hedge: " << hedge <<" date: " << get_std_time(bars->latestDates.back()) << endl;

    // If there is enough lookback in spreads, begin trading
    if (spreads.size() > z_lookback) {

        // Keep only the z-score lookback period
        vector<double> lbackspreads = spreads;
        reverse(lbackspreads.begin(), lbackspreads.end());
        lbackspreads.resize(z_lookback);
        reverse(lbackspreads.begin(), lbackspreads.end());

        // Now calculate the z-score from these lookback period spreads
        double zscore = (lbackspreads.back() - gsl_stats_mean(lbackspreads.data(), 1, z_lookback)) /
                        gsl_stats_sd(lbackspreads.data(), 1, z_lookback);

        // ORDERING LOGIC:
        if (inShort && zscore < 0.0) {

            // Exit the position when going short and the zscore goes negative
            events->push_back(new SignalEvent(symbol_list->at(0), bars->latestDates.end().operator*(), 0.0));
            events->push_back(new SignalEvent(symbol_list->at(1), bars->latestDates.end().operator*(), 0.0));
            inShort = false;
            inLong = false;
            return;
        }
        if (inLong && zscore > 0.0) {

            // Exit the position when going long and the zscore goes positive
            events->push_back(new SignalEvent(symbol_list->at(0), bars->latestDates.back(), 0.0));
            events->push_back(new SignalEvent(symbol_list->at(1), bars->latestDates.back(), 0.0));
            inShort = false;
            inLong = false;
            return;
        }

        if (!inLong && zscore < -1.0) {

            // Enter the position long when zscore exceeds -1.0 and not already going long
            inShort = false;
            inLong = true;
            vector<double> necessarypcts = computeHoldingsPct(1, hedge, Y.back(), X.back());
            cout << get_std_time(bars->latestDates.back()) << " at " <<  necessarypcts[0] << " : " << necessarypcts[1] << endl;
            events->push_back(new SignalEvent(symbol_list->at(0), bars->latestDates.back(), necessarypcts[0]));
            events->push_back(new SignalEvent(symbol_list->at(1), bars->latestDates.back(), necessarypcts[1]));
            return;
        }

        if (!inShort && zscore > 1.0) {

            // Enter the position long when zscore exceeds -1.0 and not already going long
            inShort = true;
            inLong = false;
            vector<double> necessarypcts = computeHoldingsPct(-1, hedge, Y.back(), X.back());
            cout << get_std_time(bars->latestDates.back()) << " at " << necessarypcts[0] << " : " << necessarypcts[1] << endl;
            events->push_back(new SignalEvent(symbol_list->at(0), bars->latestDates.back(), necessarypcts[0]));
            events->push_back(new SignalEvent(symbol_list->at(1), bars->latestDates.back(), necessarypcts[1]));
            return;
        }
    }

    spreads.push_back(newspread);
}

// Calculate the multiplier to hedge position against market movements
double MainStrategy::calculate_hedge_ratio(vector<double>Y, vector<double>X) {

    // Performs a least squares linear regression where c0 is the intercept and c1 is the slope
    double c0, c1, cov00, cov01, cov11, sumsq;
    gsl_fit_linear(X.data(), 1, Y.data(), 1, Y.size(), &c0, &c1, &cov00, &cov01, &cov11, &sumsq);

    //cout << get_std_time(bars->latestDates.back()) << ": hedge is " << c1 << endl;

    return c1;
}

// Compute the required holdings percents for each stock
vector<double> MainStrategy::computeHoldingsPct(double yShares, double xShares, double yPrice, double xPrice) {
    double yDol = yShares * yPrice;
    double xDol = xShares * xPrice;
    double notionalDol = abs(yDol) + abs(xDol);
    cout << get_std_time(bars->latestDates.back()) << ": Y price: " << yPrice << " X price: " << xPrice <<" Y dol: " << yDol << " X Dol: " << xDol << " Notional Dol: " << notionalDol << endl;
    cout << "X shares: " << xShares << " Y shares: " << yShares << " X pct: " << (xDol / notionalDol) << " Y pct: " << (yDol / notionalDol) << endl;
    return { (xDol / notionalDol), (yDol / notionalDol) };
}

// Placeholder initializer
MainStrategy::MainStrategy()=default;