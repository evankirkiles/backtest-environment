//
//  main.cpp
//  Backtest Environment
//
//  Created by Evan Kirkiles on 5/25/18.
//  Copyright © 2018 Evan Kirkiles. All rights reserved.
//

#include <iostream>
#include "YahooFinanceCSVReader.hpp"

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    
    //YahooFinanceCSVReader(char *symbol, char *startdate, char *enddate, char *interval, char outfilename[FILENAME_MAX], char cookiefilename[FILENAME_MAX])
    
    YahooFinanceCSVReader("AAPL", "1524796578", "1527388578", "1d", "/Users/samkirkiles/Desktop/Backtest Environment/Backtest Environment/Backtest Environment/Data Handling/test.txt", "/Users/samkirkiles/Desktop/Backtest Environment/Backtest Environment/Backtest Environment/Data Handling/cookies.txt", "/Users/samkirkiles/Desktop/Backtest Environment/Backtest Environment/Backtest Environment/Data Handling/crumb.txt");
    return 0;
}
