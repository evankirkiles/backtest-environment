//
//  data.hpp
//  Backtest Environment
//
//  Created by Evan Kirkiles on 5/29/18.
//  Copyright © 2018 Evan Kirkiles. All rights reserved.
//

#ifndef data_hpp
#define data_hpp

#include <stdio.h>

#include "MarketDataFrame.hpp"

// Abstract base class so cannot be instantiated directly
class DataHandler {
public:
    // Virtual functions to be used by subclasses
    virtual MarketDataFrame get_latest_bars(std::string symbol, int N=1);
    virtual MarketDataFrame update_bars();
};

#endif /* data_hpp */
