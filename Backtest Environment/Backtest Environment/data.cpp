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

using namespace std;

// Gets data from Yahoo Finance CSV's and returns them in format
// An interface to allow for getting "latest"
HistoricalCSVDataHandler::HistoricalCSVDataHandler(vector<Event> i_events, vector<string> i_symbol_list) {
    
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
        sort(allDates.begin(), allDates.end(), std::greater<>());
        auto last = unique(allDates.begin(), allDates.end());
        allDates.erase(last, allDates.end());
    }
}

// Format symbol data
void HistoricalCSVDataHandler::format_csv_data() {
    // Loop through symbols and load their data into a single frame
    for(int i=0;i<symbol_list.size();i++) {
        string symbol = symbol_list[i];
        
        // Get the data from Yahoo Finance
        // Access formula is symbol_data[SYMBOL].data[TYPE][DATE]
        MarketDataFrame moves = YahooFinanceCSVReader((char*)symbol.c_str(),
                                                      (char*)(string("/Users/samkirkiles/Desktop/Backtest Environment/Backtest Environment/Backtest Environment/Data Handling/CSV directory/") + symbol + string(".csv")).c_str(),
                                                      (char*)"/Users/samkirkiles/Desktop/Backtest Environment/Backtest Environment/Backtest Environment/Data Handling/cookies.txt",
                                                      (char*)"/Users/samkirkiles/Desktop/Backtest Environment/Backtest Environment/Backtest Environment/Data Handling/crumb.txt").marketmovements;
        symbol_data[symbol] = moves.data;
        append_to_dates(moves.indices);
    }
    latest_data = {};
    latestDates = {};
}


// Gets input iterator for going through data with forward merge implemented within
// Coroutine will return SDOLHCV data
void HistoricalCSVDataHandler::get_new_bar(boost::coroutines2::coroutine<tuple<string, long, double, double, double, double, double, double>>::push_type &sink, string symbol) {
    // Spits out a bar until there are no more bars to yield
    tuple<string, long, double, double, double, double, double, double>lastbar;
    for (int i=1; i<+allDates.size();i++) {
        long date = allDates.end()[-i];
        // Formatted in symbol - date - open - low - high - close - volume
        if (symbol_data[symbol]["open"][date] != 0) {
            lastbar = make_tuple(symbol, date, symbol_data[symbol]["open"][date],symbol_data[symbol]["low"][date],symbol_data[symbol]["high"][date],
                                 symbol_data[symbol]["close"][date],symbol_data[symbol]["adj"][date],symbol_data[symbol]["volume"][date]);
        } else {
            // If data is not found, use data from last get_new_bar call
            cout << "Data not found on day " << to_string(date) << " for symbol " << symbol << endl;
        }
        sink(lastbar);
    }
    
    // When done, throw an error so get_new_bar will exit
    throw std::runtime_error("No more bars to retrieve after this one.");
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
    
    for (int i=0; i < symbol_list.size(); i++)  {
        string symbol = symbol_list[i];

        // There will always be a first bar to get
        boost::coroutines2::coroutine<tuple<string, long, double, double, double, double, double, double>>::pull_type source{std::bind(&HistoricalCSVDataHandler::get_new_bar, this, std::placeholders::_1, symbol)};
        tuple<string, long, double, double, double, double, double, double>updateData = source.get();
        latest_data[get<0>(updateData)]["open"][get<1>(updateData)] = get<2>(updateData);
        latest_data[get<0>(updateData)]["low"][get<1>(updateData)] = get<3>(updateData);
        latest_data[get<0>(updateData)]["high"][get<1>(updateData)] = get<4>(updateData);
        latest_data[get<0>(updateData)]["close"][get<1>(updateData)] = get<5>(updateData);
        latest_data[get<0>(updateData)]["adj"][get<1>(updateData)] = get<6>(updateData);
        latest_data[get<0>(updateData)]["volume"][get<1>(updateData)] = get<7>(updateData);
        
        // Add new date to the dates available
        latestDates[get<0>(updateData)].push_back(get<1>(updateData));
        
        // Try to move the data forwards
        try {
            source();
        } catch (const std::runtime_error e) {
            cout << e.what() << endl;
            continue_backtest = false;
        }
        
        // Add bar as a MarketEvent
        events.push_back(MarketEvent());
    }
}

