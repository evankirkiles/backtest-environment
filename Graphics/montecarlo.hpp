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
    int* trials;
    map<string, double> perfvalues;
    fstream data;
    char* dataFile;

    // Constructor that instantiates instance pointer variables
    MonteCarlo(TradingInterface* interface, int* trials, char* dataFile);

    // Function to run the Monte Carlo analysis
    void runMC();

    // Function to close the gnuplot window
    void closeMCWindow();

};

// Pop-up window for displaying Monte Carlo analysis
class MCWindow : public QWidget {
public:

    // Backtesting variables
    TradingInterface* interface;
    MonteCarlo* mc;

    int trials;

    Q_OBJECT
public:
    explicit MCWindow(TradingInterface* interface, QWidget* parent = 0);

    // Editable labels for Monte Carlo performance reporting
    QLabel *maxdrawdownlabel;
    QLabel *mindrawdownlabel;
    QLabel *maxddperiodlabel;

private slots:
    void buttonClicked(bool checked);
private:

    // Visualization widgets
    QPushButton *runmontecarlo;
    QLabel *numtrials;
    QSpinBox *numtrialsedit;

    // Below divider
    QFrame *dividerline;

    QLabel *maxdrawdown;
    QLabel *mindrawdown;
    QLabel *maxddperiod;
};

#endif //ALGOBACKTESTER_MONTECARLO_H
