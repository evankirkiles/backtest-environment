//
//  csvdatastruct.h
//  Backtest Environment
//
//  Created by Evan Kirkiles on 5/26/18.
//  Copyright © 2018 Evan Kirkiles. All rights reserved.
//

#ifndef csvdatastruct_h
#ifndef string
#include <string>
#endif /* <string> */

#define csvdatastruct_h

struct DataPacket {
    tm date_;
    std::string symbol_;
    double open_;
    double low_;
    double high_;
    double close_;
    double adj_;
    double volume_;
};

#endif /* csvdatastruct_h */
