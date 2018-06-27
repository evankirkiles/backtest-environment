//
// Created by Evan Kirkiles on 6/25/18.
//

#include "montecarlo.hpp"

// Constructor for building Monte Carlo Qt window
MCWindow::MCWindow(TradingInterface *i_interface, string* i_start, string* i_end, QWidget *parent) : QWidget(parent) {

    // Instance pointers
    interface = i_interface;
    trials = 10;
    start = i_start;
    end = i_end;
    mc = new MonteCarlo(interface, &trials, start, end, (char*)"./Graphics/montecarlo.csv");

    // Create the layouts
    auto mainLayout = new QVBoxLayout;
    auto topLayout = new QVBoxLayout;
    auto botLayout = new QVBoxLayout;

    // Horizontal element layouts
    auto numtrialsLayout = new QHBoxLayout;
    auto maxdrawdownLayout = new QHBoxLayout;
    auto mindrawdownLayout = new QHBoxLayout;
    auto maxddperiodLayout = new QHBoxLayout;

    // Create the widgets to add to the layouts
    // Top layout widgets
    runmontecarlo = new QPushButton("Run Monte Carlo", this);
    numtrials = new QLabel("Number of Trials: ", this);
    numtrialsedit = new QSpinBox(this);

    // Set top layout properties
    runmontecarlo->setFixedSize(210, 50);
    numtrials->setFixedSize(125, 10);
    numtrialsedit->setMinimum(10);
    numtrialsedit->setMaximum(200);
    numtrialsedit->setSingleStep(10);
    numtrialsedit->setValue(10);

    // Add top widgets to their horizontal layouts
    numtrialsLayout->addWidget(numtrials);
    numtrialsLayout->addWidget(numtrialsedit);

    // Add horizontal layouts to top layout
    topLayout->addWidget(runmontecarlo);
    topLayout->addLayout(numtrialsLayout);

    // Dividing line between top and bottom layouts
    dividerline = new QFrame();
    dividerline->setFrameShape(QFrame::HLine);
    dividerline->setFixedSize(210, 3);

    // Bottom layout widgets
    maxdrawdown = new QLabel("Max Drawdown: ", this);
    maxdrawdownlabel = new QLabel("", this);
    mindrawdown = new QLabel("Min Drawdown", this);
    mindrawdownlabel = new QLabel("", this);
    maxddperiod = new QLabel("Max Drawdown Period: ", this);
    maxddperiodlabel = new QLabel("", this);

    // Add bottom widgets to their horizontal layouts
    maxdrawdownLayout->addWidget(maxdrawdown);
    maxdrawdownLayout->addWidget(maxdrawdownlabel);
    mindrawdownLayout->addWidget(mindrawdown);
    mindrawdownLayout->addWidget(mindrawdownlabel);
    maxddperiodLayout->addWidget(maxddperiod);
    maxddperiodLayout->addWidget(maxddperiodlabel);

    // Add horizontal layouts to bottom layout
    botLayout->addLayout(maxdrawdownLayout);
    botLayout->addLayout(mindrawdownLayout);
    botLayout->addLayout(maxddperiodLayout);

    // Add all layouts to the main layout
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(dividerline);
    mainLayout->addLayout(botLayout);

    // Widget connections
    connect(runmontecarlo, SIGNAL(clicked(bool)), this, SLOT(buttonClicked(bool)));
    connect(numtrialsedit, SIGNAL(valueChanged(int)), this, SLOT(trialsChanged(int)));

    // Window settings
    setFixedSize(250, 300);
    setLayout(mainLayout);
    setWindowTitle("Monte Carlo");
    setObjectName("monte_window");
    // Set the stylesheet
    QFile File("./Graphics/stylesheet.qss");
    File.open(QFile::ReadOnly);
    QString StyleSheet = QLatin1String(File.readAll());
    setStyleSheet(StyleSheet);
}

// Run whenever the button is clicked to run the Monte Carlo
void MCWindow::buttonClicked(bool checked) {
    mc->resetMC();
    mc->runMC();
    runmontecarlo->setDisabled(true);
    runmontecarlo->setText("Built Monte Carlo");
}

// Run whenever the number of trials is changed
void MCWindow::trialsChanged(int newvalue) {
    trials = newvalue;
    runmontecarlo->setDisabled(false);
    runmontecarlo->setText("Run Monte Carlo");
}

// Monte Carlo module class
// Constructor
MonteCarlo::MonteCarlo(TradingInterface *i_interface, int *i_trials, string* i_start, string* i_end, char *i_dataFile) {

    // Initializes instance pointer variables
    interface = i_interface;
    trials = i_trials;
    dataFile = i_dataFile;
    start = i_start;
    end = i_end;

    resetMC();
}

// Resets the Monte Carlo simulation
void MonteCarlo::resetMC() {
    remove(dataFile);
}

// Logic behind the Monte Carlo system is here
void MonteCarlo::runMC() {

    double maxdd = 0;
    double mindd = -1;
    double hwm = 0;
    double ddperiod;
    vector<vector<double>> rand_returns;
    vector<vector<double>> equitycurves;
    vector<map<string, double>> perfvalues;

    // Randomizer
    std::random_device rd;
    std::mt19937 g(rd());

    // Get the initial returns stream
    vector<double> r_stream = interface->portfolio.returns_stream;

    // Randomizes all the returns
    for (int i = 0; i < *trials; i++) {
        vector<double> temp = r_stream;
        vector<double> temp1 = {0};
        map<string, double> temp2 = {};
        temp2["hwm"] = 0;
        temp2["ddperiod"] = 0;
        temp2["drawdown"] = 0;
        temp2["mindrawdown"] = -1;
        std::shuffle(temp.begin(), temp.end(), g);
        rand_returns.push_back(temp);
        equitycurves.push_back(temp1);
        perfvalues.push_back(temp2);
    }

    // Open the data file to write to
    data.open(dataFile, ios::in | ios::out | ios::app);

    // Cycles through every date and adds the equity curve to the file
    for (int i = 0; i < interface->portfolio.all_holdings.size() - 1; i++) {

        // Puts the date in the first column of the .csv
        data << get_std_time(interface->portfolio.bars->allDates[i]) << ", ";

        // Puts the corresponding equity curve data in the other columns
        for (int j = 0; j < rand_returns.size(); j++) {
            if (i == 0) {
                data << "0, ";
            } else {
                //cout << equity_curves[j].size() << " " << equitycurves[j][i - 1] * 100 << " " << rand_returns[j][i] * 100 << endl;
                equitycurves[j].push_back(((equitycurves[j][i - 1] + 1) * (rand_returns[j][i] + 1)) - 1);

                if (equitycurves[j][i] < perfvalues[j]["hwm"]) {
                    perfvalues[j]["ddperiod"]++;
                    perfvalues[j]["drawdown"] = equitycurves[j][i] - perfvalues[j]["hwm"];
                    if (perfvalues[j]["drawdown"] < maxdd) {
                        maxdd = perfvalues[j]["drawdown"];
                    } else if (perfvalues[j]["drawdown"] > perfvalues[j]["mindrawdown"]) {
                        perfvalues[j]["mindrawdown"] = perfvalues[j]["drawdown"];
                        if (perfvalues[j]["mindrawdown"] > mindd) {
                            mindd = perfvalues[j]["mindrawdown"];
                        }
                    }
                } else {
                    if (perfvalues[j]["ddperiod"] > ddperiod) {
                        ddperiod = perfvalues[j]["ddperiod"];
                    }
                    perfvalues[j]["ddperiod"] = 0;
                    perfvalues[j]["drawdown"] = 0;
                    if (equitycurves[j][i] > perfvalues[j]["hwm"]) {
                        perfvalues[j]["hwm"] = equitycurves[j][i];
                        if (perfvalues[j]["hwm"] > hwm) {
                            hwm = perfvalues[j]["hwm"];
                        }
                    }
                }

                // Put the data into the .csv
                data << to_string(equitycurves[j][i] * 100) << ", ";
            }
        }

        // End the line in the .csv
        data << " \n";
    }

    cout << "Max Drawdown: " << maxdd << " Min Drawdown: " << mindd << " DD period: " << ddperiod << " Max HWM: " << hwm << endl;
    data.close();

    // Plot the simulated equity curves with GNUplot

}

// Returns the string for the Monte Carlo gnuplot format
string MonteCarlo::getMCFormat() {
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