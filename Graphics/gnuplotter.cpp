//
//  gnuplotter.cpp
//  Backtest Environment
//
//  Created by Evan Kirkiles on 6/14/18.
//  Copyright © 2018 Evan Kirkiles. All rights reserved.
//

#include "gnuplotter.hpp"

// Constructor that takes in a pointer to the returns stream
GNUPlotter::GNUPlotter(NaivePortfolio* i_portfolio, NaivePortfolio* i_benchmark, char* dat_file, char* i_positionsFile, string *startdate, string *enddate, int* showholds) {
    
    portfolio = i_portfolio;
    benchmark = i_benchmark;
    dataFile = dat_file;
    positionsFile = i_positionsFile;
    focused = false;

    start = startdate;
    end = enddate;

    showholdings = showholds;


    holdingsformat = string(" set terminal x11 1 background \"#232323\" size 2000, 200 title \"Holdings\"\n") +
                     string("set xdata\n") +
                     string("set style data histograms\n") +
                     string("set style histogram rowstacked\n") +
                     string("set style fill solid 0.75 noborder\n") +
                     string("set boxwidth 1 relative\n") +
                     string("set yrange [0:100]\n") +
                     string("set style line 100 lc rgb \"#5CDB95\" \n") +
                     string("set xtics textcolor rgb \"#232323\" \n") +
                     string("set ytics textcolor linestyle 100 \n") +
                     string("set xlabel \"Date\" tc ls 3 \n") +
                     string("set ylabel \"Percent of Holdings\" tc ls 3 offset 0, -3 \n") +
                     string("set key tc ls 3 \n") +
                     string("set key outside top right \n") +
                     string("set key title \"Legend\" tc ls 3 \n") +
                     string("set key Left \n") +
                     string("unset grid\n") +
                     string("set title \"Holdings Distribution\" tc ls 3 \n") +
                     string("set border linewidth 1 linestyle 3 \n");

    // Initialize plot pipe and its settings
    gnuplotPipe = popen("/usr/local/bin/gnuplot", "w");

    // Set plot settings
    fprintf(gnuplotPipe, "set datafile separator \",\" \n");
    fflush(gnuplotPipe);
}

// Creates pipe for commands and creates an empty plot
void GNUPlotter::initPlot() {
    
    cout << "Building GNUPlot..." << endl;

    string init1 = getEquityFormat() + string(" clear\n plot 0 with lines title \"baseline\" \n");

    fprintf(gnuplotPipe, "%s", init1.c_str());
    fflush(gnuplotPipe);

    if (*showholdings == 1) {
        string init2 = holdingsformat + string(" clear \n set xrange[0:1] \n") +string("plot 102 with boxes \n");
        fprintf(gnuplotPipe, "%s", init2.c_str());
        fflush(gnuplotPipe);
    } else {
        fprintf(gnuplotPipe, "set terminal x11 1 close\n");
        fflush(gnuplotPipe);
    }

    // Clear the data files
    remove(dataFile);
    remove(positionsFile);
}

// Replot from the updated returns stream
void GNUPlotter::updatePlot() {

    // Update data file with new returns bar
    long date = portfolio->all_holdings.rbegin().operator*().first;
    double equitycurve = portfolio->all_holdings.rbegin().operator*().second["equitycurve"];
    double benchequity = benchmark->all_holdings.rbegin().operator*().second["equitycurve"];
    double totalheld = portfolio->all_holdings.rbegin().operator*().second["totalholdings"];
    
    data.open(dataFile, ios::in | ios::out | ios::app);
    data << get_std_time(date) << ", " << equitycurve*100 << ", " << benchequity*100 << "\n";
    data.close();
    
    // VERY intensive, so be wary when turning this on
    if (*showholdings == 1) {
        
        string plotpositions = holdingsformat + string("set xrange[0:") + to_string(portfolio->bars->allDates.size()) + "] \n" + "plot \"" + string(positionsFile) + "\" ";
        
        // Also record percentage of positions in each
        data.open(positionsFile, ios::in | ios::out | ios::app);
        data << date << ", ";
        focused = false;
        for (int i=0; i<(portfolio->symbol_list->size()); i++) {
            string symbol = portfolio->symbol_list->at(i);
            double holdings = portfolio->all_holdings.rbegin().operator*().second[symbol];
            data << (holdings/totalheld)*100 << ", ";
            string toappend;
            if (!focused) {
                toappend = ("using " + to_string(i + 2) + " title \"" + symbol + "\" ");
                plotpositions.append(toappend);
                focused = true;
            } else {
                toappend = ("using " + to_string(i + 2) + ":xticlabels(1) title \"" + symbol + "\"");
                plotpositions.append(", \"\" " + toappend);
            }
        }
        double cashpct = portfolio->all_holdings.rbegin().operator*().second["heldcash"];
        data << (cashpct/totalheld)*100 << "\n";
        plotpositions.append(", \"\" using " + to_string(portfolio->symbol_list->size() + 2) + ":xticlabels(1) title \"cash\" \n");
        data.close();
        
        // Plot the position percentages
        fprintf(gnuplotPipe, "%s", plotpositions.c_str());
        fflush(gnuplotPipe);
    }
    
    // Plot the main equity curve and benchmark
    string plot2 = getEquityFormat() + string("plot \"") + string(dataFile) +
            string("\" using 1:2 with lines title \"ALGORITHM\" ls 1, "
                   "\"\" using 1:3 with lines title \"BENCHMARK\" ls 2,"
                   " 0 with lines title \"baseline\" \n");
    fprintf(gnuplotPipe, "%s", plot2.c_str());
    fflush(gnuplotPipe);
}

// Quits the plot
void GNUPlotter::quitPlot() {
    cout << "Press enter to quit." << endl;
    getchar();
    fprintf(gnuplotPipe, "exit \n");
}

// Gets the format for the equity curve
string GNUPlotter::getEquityFormat() {
    return string(" set terminal x11 0 background \"#232323\" size 2000, 400 title \"Equity Curve\" \n") +
           string("set xdata time\n") +
           string("set style data lines\n") +
           string("set timefmt \"%Y-%m-%d\"\n") +
           string("set format x \"%Y/%m/%d\"\n") +
           string("set format y '%4.2f%%' \n") +
           string("set autoscale y \n") +
           string("set xrange[\"") + *start + "\":\"" + *end + "\"] \n" +
           string("set style line 1 lt 1 lc rgb \"#5CDB95\" \n") +
           string("set style line 2 lt 2 lc rgb \"#C3073F\" pt 6 \n") +
           string("set style line 3 lc rgb \"#FFFFFF\" \n") +
           string("set style line 4 lt 0 lc rgb \"#6d6d6d\" \n") +
           string("set xtics textcolor linestyle 3 \n") +
           string("set ytics textcolor linestyle 1 \n") +
           string("set xlabel \"Date\" tc ls 3 \n") +
           string("set ylabel \"Total Holdings\" tc ls 3 offset 0, -3 \n") +
           string("set key tc ls 3 \n") +
           string("set key outside top right \n") +
           string("set key title \"Legend\" tc ls 3 \n") +
           string("set key Left \n") +
           string("set grid ls 4 \n") +
           string("set title \"Equity Curve\" tc ls 3 \n") +
           string("set border linewidth 1 linestyle 3 \n");
}