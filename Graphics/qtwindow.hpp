//
// Created by Evan Kirkiles on 6/15/18.
//

#ifndef ALGOBACKTESTER_QTWINDOW_HPP
#define ALGOBACKTESTER_QTWINDOW_HPP

#include <QWidget>
#ifndef TradingInterface
#include "../Infrastructure/interface.hpp"
#endif

// Window for displaying performance stats and start/stop button
class QPushButton;
class QLabel;
class QDateEdit;
class QLineEdit;
class QCheckBox;
class AlgoWindow : public QWidget {

    // Backtesting variables
    TradingInterface* interface;
    MainStrategy* strat;
    Benchmark* bench;
    GNUPlotter* gnuplot;

    bool initialized;

    string *startdateaddr;
    string *enddateaddr;
    double* initialcapaddr;
    int* showholds;

    Q_OBJECT
public:
    explicit AlgoWindow(TradingInterface* trader, MainStrategy* strat, Benchmark* bench, GNUPlotter* gnuplot,
                        string *startdate, string *enddate, double* initialcapitalval, int* showholds, QWidget *parent = 0);

    void performanceValues();
    QLabel *totalreturnlabel;
    QLabel *sharpelabel;
    QLabel *hwmlabel;
    QLabel *drawdownlabel;
    QLabel *betalabel;
    QLabel *alphalabel;

private slots:
    void buttonClicked(bool checked);
    void varsChanged();
    void setMaxDate();
    void setMinDate();
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
    QLabel *beta;
    QLabel *alpha;
    QFrame *dividerline;
};

#endif //ALGOBACKTESTER_QTWINDOW_HPP
