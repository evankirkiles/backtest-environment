//
//  gnuplotter.hpp
//  Backtest Environment
//
//  Created by Evan Kirkiles on 6/11/18.
//  Copyright © 2018 Evan Kirkiles. All rights reserved.
//

#ifndef gnuplotter_hpp
#define gnuplotter_hpp

#ifndef Portfolio
#include "portfolio.hpp"
#endif
#ifndef fstream
#include <fstream>
#endif

#include <stdio.h>

using namespace std;

// Graphing functionality
class GNUPlotter {
public:
    
    // Contains the latest data and returns stream
    NaivePortfolio* portfolio;
    NaivePortfolio* benchmark;
    FILE *gnuplotPipe;
    char* dataFile;
    char* positionsFile;
    fstream data;
    
    long start;
    long end;
    
    string holdingsformat;
    string equitycurveformat;
    
    // Refocuses the data if it is the first bar
    bool focused;
    
    // Determines whether or not to plot holdings sizes
    bool showholdings = false;
    
    // Constructor
    GNUPlotter(NaivePortfolio* bars, NaivePortfolio* benchmark, char* dat_file, char* i_positionsFile, long startdate, long enddate, bool showholds);
    
    // Creates empty returns stream plot that prepares for data from returns stream
    void initPlot();
    // Updates the plot with data from the returns stream
    void updatePlot();
    
    // Quits the gnuplot instance
    void quitPlot();
};

#endif /* gnuplotter_hpp */
