//
//  gnuplotter.hpp
//  Backtest Environment
//
//  Created by Evan Kirkiles on 6/11/18.
//  Copyright © 2018 Evan Kirkiles. All rights reserved.
//

#ifndef gnuplotter_hpp
#define gnuplotter_hpp

#ifndef DataHandler
#include "data.hpp"
#endif

#include <stdio.h>

// Graphing functionality
class GNUPlotter {
public:
    
    // Contains the latest data and returns stream
    HistoricalCSVDataHandler* bars;
    FILE* gnuplotPipe;
    
    // Constructor
    GNUPlotter(HistoricalCSVDataHandler* bars);
    
    // Creates empty returns stream plot that prepares for data from returns stream
    void initPlot();
    // Updates the plot with data from the returns stream
    void updatePlot();
};

#endif /* gnuplotter_hpp */
