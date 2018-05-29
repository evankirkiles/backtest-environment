//
//  MarketDataFrame.hpp
//  Backtest Environment
//
//  Created by Evan Kirkiles on 5/27/18.
//  Copyright © 2018 Evan Kirkiles. All rights reserved.
//

#ifndef MarketDataFrame_hpp
#define MarketDataFrame_hpp
#ifndef string
#include <string>
#endif /* <string> */

#include <stdio.h>
#include <vector>
#include <map>

using namespace std;

class MarketDataFrame {
public:
    map<string, map<string, double>> data;
    vector<string> indices;
    MarketDataFrame(char csv_file[FILENAME_MAX], string symbol);
};

#endif /* MarketDataFrame_hpp */
