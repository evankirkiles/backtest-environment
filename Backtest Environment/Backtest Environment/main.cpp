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

int main(int argc, const char * argv[]) {
    //YahooFinanceCSVReader(char *symbol, char *startdate, char *enddate, char *interval, char outfilename[FILENAME_MAX], char cookiefilename[FILENAME_MAX])
    
    /*
    char *symbol = (char*)"QQQ";
    
    
    MarketDataFrame moves = YahooFinanceCSVReader(
                                                   symbol,
                                                   (char*)(string("/Users/samkirkiles/Desktop/Backtest Environment/Backtest Environment/Backtest Environment/Data Handling/CSV directory/") + string(symbol) + string(".csv")).c_str(),
                                                   (char*)"/Users/samkirkiles/Desktop/Backtest Environment/Backtest Environment/Backtest Environment/Data Handling/cookies.txt",
                                                   (char*)"/Users/samkirkiles/Desktop/Backtest Environment/Backtest Environment/Backtest Environment/Data Handling/crumb.txt").marketmovements;
    
    // Mimic pandas DataFrame for historical data: hold indices in a vector
    // Allows for integer indexing and makes life a lot easier
    double getdate = moves.indices[2];
    
    cout << "Data for stock " << symbol << " on date " << getdate << endl;
    cout << "OPEN: " << moves.data["open"][getdate] << endl;
    cout << "CLOSE: " << moves.data["close"][getdate] << endl;
    cout << "HIGH: " << moves.data["high"][getdate] << endl;
    cout << "LOW: " << moves.data["low"][getdate] << endl;
    */
    
    vector<string> symbol_list = {string("QQQ"), string("AAPL")};
    
    TradingInterface interface = TradingInterface(symbol_list, 10000000, 0);
    BuyAndHoldStrategy strat = BuyAndHoldStrategy(interface.pipeline, &interface.events);
    
    interface.runbacktest(strat);
     
    return 0;
}
