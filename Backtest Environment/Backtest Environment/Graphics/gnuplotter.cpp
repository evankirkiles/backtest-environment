//
//  gnuplotter.cpp
//  Backtest Environment
//
//  Created by Evan Kirkiles on 6/11/18.
//  Copyright © 2018 Evan Kirkiles. All rights reserved.
//

#include "gnuplotter.hpp"

// Constructor that takes in a pointer to the returns stream
GNUPlotter::GNUPlotter(NaivePortfolio* i_portfolio, char* dat_file) {
    
    portfolio = i_portfolio;
    dataFile = dat_file;
    focused = false;
    
    // Clear the data file
    remove(dataFile);
    
}

// Creates pipe for commands and creates an empty plot
void GNUPlotter::initPlot() {
    gnuplotPipe = popen("/usr/local/bin/gnuplot", "w");
    
    // Set plot settings
    fprintf(gnuplotPipe, "set datafile separator \",\"\n");
    fflush(gnuplotPipe);
    fprintf(gnuplotPipe, "set xlabel \"date (epochtime)\"\n");
    fflush(gnuplotPipe);
    fprintf(gnuplotPipe, "set ylabel \"total returns\"\n");
    fflush(gnuplotPipe);
}

// Replot from the updated returns stream
void GNUPlotter::updatePlot() {
    
    // Update data file with new returns bar
    long date = portfolio->all_holdings.rbegin()->first;
    double equitycurve = portfolio->all_holdings.rbegin()->second["equitycurve"];
    
    data.open(dataFile, ios::in | ios::out | ios::app);
    data << date << ", " << equitycurve << "\n";
    data.close();
    
    // In case of it being the first equity curve, plot the data there
    if (!focused) {
        cout << "Building GNUPlot..." << endl;
        fprintf(gnuplotPipe, "plot \"%s\" using 1:2 with lines title \"PERFORMANCE\", 0 with lines title \"baseline\" \n", dataFile);
        fflush(gnuplotPipe);
        focused = true;
    } else {
        // Replot with new data
        fprintf(gnuplotPipe, "replot \n");
        fflush(gnuplotPipe);
    }
}

// Quits the plot
void GNUPlotter::quitPlot() {
    cout << "Press enter to quit." << endl;
    getchar();
    fprintf(gnuplotPipe, "exit \n");
}
