//
//  YahooFinanceCSVReader.hpp
//  Backtest Environment
//
//  Created by Evan Kirkiles on 5/26/18.
//  Copyright © 2018 Evan Kirkiles. All rights reserved.
//

#ifndef YahooFinanceCSVReader_hpp
#define YahooFinanceCSVReader_hpp

#include <stdio.h>
#include "MarketDataFrame.hpp"

class YahooFinanceCSVReader {
public: YahooFinanceCSVReader(char *symbol, long startdate, long enddate, char outfilename[FILENAME_MAX], char cookiefilename[FILENAME_MAX], char crumbfilename[FILENAME_MAX]);
    MarketDataFrame marketmovements;
};

#endif /* YahooFinanceCSVReader_hpp */
