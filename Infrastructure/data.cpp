//
//  data.cpp
//  Backtest Environment
//
//  Created by Evan Kirkiles on 5/29/18.
//  Copyright © 2018 Evan Kirkiles. All rights reserved.
//

#include "data.hpp"
#include <tuple>
#include <iostream>

using namespace std;

// Gets data from Yahoo Finance CSV's and returns them in format
// An interface to allow for getting "latest"
HistoricalCSVDataHandler::HistoricalCSVDataHandler(boost::ptr_vector<Event>* i_events, vector<string>* i_symbol_list, string *i_start_date, string *i_end_date, int* i_continue_backtest) {
    
    events = i_events;
    symbol_list = i_symbol_list;
    continue_backtest = i_continue_backtest;
    start_date = i_start_date;
    end_date = i_end_date;
};

// Placeholder initializer
HistoricalCSVDataHandler::HistoricalCSVDataHandler() = default;

// Appends unique dates to master dates list
void HistoricalCSVDataHandler::append_to_dates(vector<long> new_dates, string which) {
    if (which == "allDates") {
        if (allDates.empty()) {
            allDates = new_dates;
        } else {
            allDates.insert(allDates.end(), new_dates.begin(), new_dates.end());
            sort(allDates.begin(), allDates.end());
            auto last = unique(allDates.begin(), allDates.end());
            allDates.erase(last, allDates.end());
        }
    } else if (which == "latestDates") {
        if (latestDates.empty()) {
            latestDates = new_dates;
        } else {
            latestDates.insert(latestDates.end(), new_dates.begin(), new_dates.end());
            sort(latestDates.begin(), latestDates.end());
            auto last = unique(latestDates.begin(), latestDates.end());
            latestDates.erase(last, latestDates.end());
        }
    }
}

// Format symbol data
void HistoricalCSVDataHandler::format_csv_data() {
    allDates = {};
    latestDates = {};
    datesbegin = get_epoch_time(*start_date);
    datesend = get_epoch_time(*end_date);

    symbol_data={};
    latest_data = {};
    currentDatesIndex = {};

    // Loop through symbols and load their data into a single frame
    for(int i=0;i<symbol_list->size();i++) {
        string symbol = (*symbol_list)[i];

        cout << string("Loading ") << symbol << string(".csv ...") << endl;

        // Get the data from Yahoo Finance
        // Access formula is symbol_data[SYMBOL].data[TYPE][DATE]
        MarketDataFrame moves = YahooFinanceCSVReader((char*)symbol.c_str(), get_epoch_time(*start_date),
                                                      get_epoch_time(*end_date),
                                                      (char*)(string("./DataHandling/CSV directory/") + symbol + string(".csv")).c_str(),
                                                      (char*)"./DataHandling/cookies.txt",
                                                      (char*)"./DataHandling/crumb.txt").marketmovements;
        symbol_data[symbol] = moves.data;
        currentDatesIndex[symbol] = 0;
        append_to_dates(moves.indices, "allDates");

        // Also adds data from a year back to latest_data as a buffer for data requests
        // For a larger buffer, increase the number subtracted from the epoch time of the start date
        MarketDataFrame movesbuffer = YahooFinanceCSVReader((char*)symbol.c_str(), get_epoch_time(*start_date) - 31557600,
                                                            get_epoch_time(*start_date) - 86400,
                                                            (char*)(string("./DataHandling/CSV directory/") + symbol + string(".csv")).c_str(),
                                                            (char*)"./DataHandling/cookies.txt",
                                                            (char*)"./DataHandling/crumb.txt").marketmovements;
        latest_data[symbol] = movesbuffer.data;
        append_to_dates(movesbuffer.indices, "latestDates");
    }
}


// Gets input iterator for going through data with forward merge implemented within
tuple<string, long, double, double, double, double, double, double> HistoricalCSVDataHandler::get_new_bar(string symbol) {
    // Spits out a bar until there are no more bars to yield
    tuple<string, long, double, double, double, double, double, double>lastbar;
    long date = allDates[currentDatesIndex[symbol]];
    
    // Formatted in symbol - date - open - low - high - close - volume
    if (symbol_data[symbol]["open"][date] != 0) {
        lastbar = make_tuple(symbol, date, symbol_data[symbol]["open"][date],symbol_data[symbol]["low"][date],symbol_data[symbol]["high"][date],
                             symbol_data[symbol]["close"][date],symbol_data[symbol]["adj"][date],symbol_data[symbol]["volume"][date]);
        previousbar[symbol] = lastbar;
        currentDatesIndex[symbol]++;
    } else {
        // If data is not found, use data from last get_new_bar call
        lastbar = previousbar[symbol];
        cout << "Data not found on day " << to_string(date) << " for symbol " << symbol << endl;
    }
    return lastbar;
}

// Get latest N bars
map<string, map<long, double>> HistoricalCSVDataHandler::get_latest_bars(string symbol, int N=1) {
    
    // If symbol exists in latest_data
    map<string, map<long, double>>bars_list;

    for (int i = N; i >= 0; i--) {
        long date = latestDates[latestDates.size() - 1 - i];
        bars_list["open"][date] = latest_data[symbol]["open"][date];
        bars_list["close"][date] = latest_data[symbol]["close"][date];
        bars_list["high"][date] = latest_data[symbol]["high"][date];
        bars_list["low"][date] = latest_data[symbol]["low"][date];
        bars_list["adj"][date] = latest_data[symbol]["adj"][date];
        bars_list["volume"][date] = latest_data[symbol]["volume"][date];
    }
        
    // Return cleaned bars list
    return bars_list;
}

// Push the latest bar to latest_data for all symbols in list
void HistoricalCSVDataHandler::update_bars() {
    
    for (int i=0; i < symbol_list->size(); i++) {
        string symbol = (*symbol_list)[i];

        // There will always be a first bar to get
        tuple<string, long, double, double, double, double, double, double> updateData = get_new_bar(symbol);
        latest_data[get<0>(updateData)]["open"][get<1>(updateData)] = get<2>(updateData);
        latest_data[get<0>(updateData)]["low"][get<1>(updateData)] = get<3>(updateData);
        latest_data[get<0>(updateData)]["high"][get<1>(updateData)] = get<4>(updateData);
        latest_data[get<0>(updateData)]["close"][get<1>(updateData)] = get<5>(updateData);
        latest_data[get<0>(updateData)]["adj"][get<1>(updateData)] = get<6>(updateData);
        latest_data[get<0>(updateData)]["volume"][get<1>(updateData)] = get<7>(updateData);

        // Add new date to latestDates
        if (latestDates.back() != get<1>(updateData)) {
            latestDates.push_back(get<1>(updateData));
        }

        // Check if there are any more bars to get
        if (currentDatesIndex[symbol] == allDates.size()) {
            *continue_backtest = 0;
        }
    }
    // Add bar as a MarketEvent
    events->push_back(new MarketEvent());
}

