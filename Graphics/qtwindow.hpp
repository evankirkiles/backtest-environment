//
// Created by Evan Kirkiles on 6/15/18.
//

#ifndef ALGOBACKTESTER_QTWINDOW_HPP
#define ALGOBACKTESTER_QTWINDOW_HPP

#include <QWidget>
#include "../Infrastructure/interface.hpp"

// Window for displaying performance stats and start/stop button
class QPushButton;
class QLabel;
class QDateEdit;
class QLineEdit;
class QCheckBox;
class AlgoWindow : public QWidget {

    // Backtesting variables
    TradingInterface* interface;
    BuyAndHoldStrategy* strat;
    Benchmark* bench;
    GNUPlotter* gnuplot;

    bool initialized;

    string *startdateaddr;
    string *enddateaddr;
    double* initialcapaddr;
    int* showholds;

    Q_OBJECT
public:
    explicit AlgoWindow(TradingInterface* trader, BuyAndHoldStrategy* strat, Benchmark* bench, GNUPlotter* gnuplot,
                        string *startdate, string *enddate, double* initialcapitalval, int* showholds, QWidget *parent = 0);
private slots:
    void buttonClicked(bool checked);
    void varsChanged();
private:

    // Visualization widgets
    QPushButton *m_button;
    QLabel *startdate;
    QDateEdit *startdateedit;
    QLabel *enddate;
    QDateEdit*enddateedit;
    QLabel *initialcapital;
    QLineEdit *initialcapedit;
    QLabel *showholdings;
    QCheckBox *holdingsbool;
    QLabel *totalreturn;
    QLabel *sharpe;
    QLabel *hwm;
    QLabel *drawdown;
};

#endif //ALGOBACKTESTER_QTWINDOW_HPP
