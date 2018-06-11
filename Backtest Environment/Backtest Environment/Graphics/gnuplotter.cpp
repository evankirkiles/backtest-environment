//
//  gnuplotter.cpp
//  Backtest Environment
//
//  Created by Evan Kirkiles on 6/11/18.
//  Copyright © 2018 Evan Kirkiles. All rights reserved.
//

#include "gnuplotter.hpp"

// Constructor that takes in a pointer to the returns stream
GNUPlotter::GNUPlotter(HistoricalCSVDataHandler* i_bars) {
    
    bars = i_bars;
}

// Creates pipe for commands and creates an empty plot
void GNUPlotter::initPlot() {
    gnuplotPipe = popen("/usr/local/bin/gnuplot", "w");
    
    // Placeholder test for gnuplot
    fprintf(gnuplotPipe, "plot sin(x)\n");
    fflush(gnuplotPipe);
    
    printf("press enter");
    getchar();
    fprintf(gnuplotPipe, "exit \n");
}
