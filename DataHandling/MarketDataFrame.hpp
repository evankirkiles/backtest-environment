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
    map<string, map<long, double>> data;
    vector<long> indices;
    MarketDataFrame(char csv_file[FILENAME_MAX], string symbol);
    MarketDataFrame();
};

long get_epoch_time(string date);
string get_std_time(long epochtime);

#endif /* MarketDataFrame_hpp */
