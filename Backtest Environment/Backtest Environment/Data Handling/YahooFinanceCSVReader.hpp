//
//  YahooFinanceCSVReader.hpp
//  Backtest Environment
//
//  Created by Sam Kirkiles on 5/26/18.
//  Copyright © 2018 Evan Kirkiles. All rights reserved.
//

#ifndef YahooFinanceCSVReader_hpp
#define YahooFinanceCSVReader_hpp

#include <stdio.h>

class YahooFinanceCSVReader {
public: YahooFinanceCSVReader(char *symbol, char *startdate, char *enddate, char *interval, char outfilename[FILENAME_MAX], char cookiefilename[FILENAME_MAX], char crumbfilename[FILENAME_MAX]);
};

#endif /* YahooFinanceCSVReader_hpp */
