//
// Created by Evan Kirkiles on 6/25/18.
//

#ifndef ALGOBACKTESTER_MONTECARLO_H
#define ALGOBACKTESTER_MONTECARLO_H

#ifndef QWidget
#include <QWidget>
#endif
#ifndef QFile
#include <QFile>
#endif
#ifndef QPushButton
#include <QPushButton>
#endif
#ifndef QSpinBox
#include <QSpinBox>
#endif
#ifndef QVBoxLayout
#include <QVBoxLayout>
#endif
#ifndef QFrame
#include <QFrame>
#endif
#include <random>
#ifndef TradingInterface
#include "../Infrastructure/interface.hpp"
#endif

// Actual Monte Carlo module
class MonteCarlo {
public:

    TradingInterface* interface;
    string* start;
    string* end;
    int* trials;
    map<string, double> perfvalues;
    fstream data;
    char* dataFile;

    FILE* plotPipe;
    bool windowOpen = false;

    double maxdd;
    double mindd;
    double hwm;
    int ddperiod;

    // Constructor that instantiates instance pointer variables
    MonteCarlo(TradingInterface* interface, int* trials, string* start, string* end, char* dataFile);

    // Function to run the Monte Carlo analysis
    void runMC();
    void resetMC();

    // GNUplot related functions
    void closeMCWindow();
    string getMCFormat();
};

// Pop-up window for displaying Monte Carlo analysis
class MCWindow : public QWidget {
public:

    // Backtesting variables
    TradingInterface* interface;
    MonteCarlo* mc;

    string* start;
    string* end;

    int trials;

    Q_OBJECT
public:
    explicit MCWindow(TradingInterface* interface, string* start, string* end, QWidget* parent = 0);
    void displayMCStats();

    // Override window close event so gnuplot closes when this window closes
    void closeEvent(QCloseEvent *event) override;

    // Editable labels for Monte Carlo performance reporting
    QLabel *maxdrawdownlabel;
    QLabel *maxddperiodlabel;
    QLabel *maxhwmlabel;

private slots:
    void buttonClicked(bool checked);
    void trialsChanged(int newvalue);
private:

    // Visualization widgets
    QPushButton *runmontecarlo;
    QLabel *numtrials;
    QSpinBox *numtrialsedit;

    // Below divider
    QFrame *dividerline;

    QLabel *maxdrawdown;
    QLabel *maxddperiod;
    QLabel *maxhwm;
};

#endif //ALGOBACKTESTER_MONTECARLO_H
