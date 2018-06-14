//
//  gnuplotter.cpp
//  Backtest Environment
//
//  Created by Evan Kirkiles on 6/11/18.
//  Copyright © 2018 Evan Kirkiles. All rights reserved.
//

#include "gnuplotter.hpp"

// Constructor that takes in a pointer to the returns stream
GNUPlotter::GNUPlotter(NaivePortfolio* i_portfolio, NaivePortfolio* i_benchmark, char* dat_file, char* i_positionsFile, long startdate, long enddate) {
    
    portfolio = i_portfolio;
    benchmark = i_benchmark;
    dataFile = dat_file;
    positionsFile = i_positionsFile;
    focused = false;
    
    start = startdate;
    end = enddate;
    
    // Clear the data files
    remove(dataFile);
    remove(positionsFile);
}

// Creates pipe for commands and creates an empty plot
void GNUPlotter::initPlot() {
    gnuplotPipe = popen("/usr/local/bin/gnuplot", "w");
    
    // Set plot settings
    fprintf(gnuplotPipe, "set datafile separator \",\"\n");
    fprintf(gnuplotPipe, "set xlabel \"date (epochtime)\"\n");
    fprintf(gnuplotPipe, "set xrange [%ld:%ld]\n", start, end);
    fflush(gnuplotPipe);
    
    cout << "Building GNUPlot..." << endl;
}

// Replot from the updated returns stream
void GNUPlotter::updatePlot() {
    string plotpositions = "set terminal x11 1 title 'Holdings Distribution' size 2000, 200\n set yrange [0:100]\n set ylabel \"holdings\"\n plot \"" + string(positionsFile) + "\" ";
    
    // Update data file with new returns bar
    long date = portfolio->all_holdings.rbegin()->first;
    double equitycurve = portfolio->all_holdings.rbegin()->second["equitycurve"];
    double benchequity = benchmark->all_holdings.rbegin()->second["equitycurve"];
    double totalheld = portfolio->all_holdings.rbegin()->second["totalholdings"];
    
    data.open(dataFile, ios::in | ios::out | ios::app);
    data << date << ", " << equitycurve << ", " << benchequity << "\n";
    data.close();
    
    // Also record percentage of positions in each
    data.open(positionsFile, ios::in | ios::out | ios::app);
    data << date << ", ";
    focused = false;
    for (int i=0; i<(portfolio->symbol_list.size()); i++) {
        string symbol = portfolio->symbol_list[i];
        double holdings = portfolio->all_holdings.rbegin()->second[symbol];
        data << (holdings/totalheld)*100 << ", ";
        string toappend = ("using 1:" + to_string(i + 2) + " with lines title \"" + symbol + "\" ");
        if (focused == false) {
            plotpositions.append(toappend);
            focused = true;
        } else {
            plotpositions.append(", \"\" " + toappend);
        }
    }
    double cashpct = portfolio->all_holdings.rbegin()->second["heldcash"];
    data << (cashpct/totalheld)*100 << "\n";
    plotpositions.append(", \"\" using 1:" + to_string(portfolio->symbol_list.size() + 2) + " with lines title \"cash\" \n");
    data.close();
    
    // Plot the main equity curve and benchmark
    fprintf(gnuplotPipe, "set terminal x11 0 title 'Equity Curve' size 2000, 400\n set ylabel \"portfolio value\"\n set format y '%%4.2f%%%%' \n set autoscale y \n plot \"%s\" using ($1):($2*100) with lines title \"ALGORITHM\" lc 15, \"\" using ($1):($3*100) with lines title \"BENCHMARK\" lc 6, 0 with lines title \"baseline\" \n", dataFile);
    fflush(gnuplotPipe);
    
    // Plot the position percentages
    fprintf(gnuplotPipe, "%s", plotpositions.c_str());
    fflush(gnuplotPipe);
}

// Quits the plot
void GNUPlotter::quitPlot() {
    cout << "Press enter to quit." << endl;
    getchar();
    fprintf(gnuplotPipe, "exit \n");
}
