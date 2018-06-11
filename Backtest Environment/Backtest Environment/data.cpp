//
//  data.cpp
//  Backtest Environment
//
//  Created by Evan Kirkiles on 5/29/18.
//  Copyright © 2018 Evan Kirkiles. All rights reserved.
//

#include "data.hpp"
#ifndef boost
#include <boost/coroutine2/all.hpp>
#endif
#ifndef ptr_vector
#include <boost/ptr_container/ptr_vector.hpp>
#endif

using namespace std;

// Gets data from Yahoo Finance CSV's and returns them in format
// An interface to allow for getting "latest"
HistoricalCSVDataHandler::HistoricalCSVDataHandler(boost::ptr_vector<Event>* i_events, vector<string>* i_symbol_list) {
    
    events = i_events;
    symbol_list = i_symbol_list;
    continue_backtest = true;
};

// Placeholder initializer
HistoricalCSVDataHandler::HistoricalCSVDataHandler() {};

// Appends unique dates to master dates list
void HistoricalCSVDataHandler::append_to_dates(vector<long> new_dates) {
    if (allDates.empty()) {
        allDates = new_dates;
    } else {
        allDates.insert(allDates.end(), new_dates.begin(), new_dates.end());
        sort(allDates.begin(), allDates.end(), std::less<>());
        auto last = unique(allDates.begin(), allDates.end());
        allDates.erase(last, allDates.end());
    }
}

// Format symbol data
void HistoricalCSVDataHandler::format_csv_data() {
    // Loop through symbols and load their data into a single frame
    for(int i=0;i<symbol_list->size();i++) {
        string symbol = (*symbol_list)[i];
        
        // Get the data from Yahoo Finance
        // Access formula is symbol_data[SYMBOL].data[TYPE][DATE]
        MarketDataFrame moves = YahooFinanceCSVReader((char*)symbol.c_str(),
                                                      (char*)(string("./Backtest Environment/Data Handling/CSV directory/") + symbol + string(".csv")).c_str(),
                                                      (char*)"./Backtest Environment/Data Handling/cookies.txt",
                                                      (char*)"./Backtest Environment/Data Handling/crumb.txt").marketmovements;
        symbol_data[symbol] = moves.data;
        currentDatesIndex[symbol] = 0;
        append_to_dates(moves.indices);
    }
    latest_data = {};
    latestDates = {};
}


// Gets input iterator for going through data with forward merge implemented within
// Coroutine will return SDOLHCV data
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
    try { map<string, map<long, double>>bars_list = latest_data[symbol];
        
        // Iterate through the bars and erase ones before the date specified by N
        std::map<long, double>::iterator iter = bars_list["open"].begin();
        for (; iter != bars_list["open"].end();) {
            if (iter->first < latestDates[symbol].end()[-N]) {
                long date = iter->first;
                iter++;
                bars_list["open"].erase(date);
                bars_list["close"].erase(date);
                bars_list["high"].erase(date);
                bars_list["low"].erase(date);
                bars_list["volume"].erase(date);
                bars_list["adj"].erase(date);
            } else {
                iter++;
            }
        }
        
        // Return cleaned bars list
        return bars_list;
    } catch (exception& e) {
        cout << "Data nonexistent for symbol " << symbol << endl;
        return { };
    }
}

// Push the latest bar to latest_data for all symbols in list
void HistoricalCSVDataHandler::update_bars() {
    
    for (int i=0; i < symbol_list->size(); i++)  {
        string symbol = (*symbol_list)[i];

        // There will always be a first bar to get
        tuple<string, long, double, double, double, double, double, double>updateData = get_new_bar(symbol);
        latest_data[get<0>(updateData)]["open"][get<1>(updateData)] = get<2>(updateData);
        latest_data[get<0>(updateData)]["low"][get<1>(updateData)] = get<3>(updateData);
        latest_data[get<0>(updateData)]["high"][get<1>(updateData)] = get<4>(updateData);
        latest_data[get<0>(updateData)]["close"][get<1>(updateData)] = get<5>(updateData);
        latest_data[get<0>(updateData)]["adj"][get<1>(updateData)] = get<6>(updateData);
        latest_data[get<0>(updateData)]["volume"][get<1>(updateData)] = get<7>(updateData);
        
        // Add new date to the dates available
        latestDates[get<0>(updateData)].push_back(get<1>(updateData));
        
        // Check if there are any more bars to get
        if (currentDatesIndex[symbol] == allDates.size()) {
            continue_backtest = false;
        }
    }
    // Add bar as a MarketEvent
    events->push_back(new MarketEvent());
}

