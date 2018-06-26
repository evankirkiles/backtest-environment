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
#ifndef TradingInterface
#include "../Infrastructure/interface.hpp"
#endif

// Pop-up window for displaying Monte Carlo analysis
class MCWindow : public QWidget {
public:

    // Backtesting variables
    TradingInterface* interface;

    Q_OBJECT
public:
    explicit MCWindow(TradingInterface* interface, QWidget* parent = 0);

    // Editable labels for Monte Carlo performance reporting
    QLabel *maxdrawdownlabel;
    QLabel *mindrawdownlabel;
    QLabel *maxddperiodlabel;

private slots:
    //void buttonClicked();
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
