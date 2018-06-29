//
// Created by Evan Kirkiles on 6/15/18.
//

#ifndef ALGOBACKTESTER_QTWINDOW_HPP
#define ALGOBACKTESTER_QTWINDOW_HPP

#include <QWidget>
#ifndef TradingInterface
#include "../Infrastructure/interface.hpp"
#endif
#include "montecarlo.hpp"

// Window for displaying performance stats and start/stop button
class QPushButton;
class QLabel;
class QDateEdit;
class QLineEdit;
class QCheckBox;
class QListView;
class QStringListModel;
class AlgoWindow : public QWidget {

    // Backtesting variables
    TradingInterface* interface;
    MainStrategy* strat;
    Benchmark* bench;
    GNUPlotter* gnuplot;
    MCWindow* montecarlowindow;

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

    // Override window close event so gnuplot closes when this window closes
    void closeEvent(QCloseEvent *event) override;

    QLabel *totalreturnlabel;
    QLabel *sharpelabel;
    QLabel *hwmlabel;
    QLabel *drawdownlabel;
    QLabel *betalabel;
    QLabel *alphalabel;

private slots:
    void buttonClicked(bool checked);
    void showMonteCarlo();
    void varsChanged();
    void setMaxDate();
    void setMinDate();
private:

    // Visualization widgets
    QPushButton *m_button;
    QPushButton *montecarlo_button;
    QLabel *startdate;
    QDateEdit *startdateedit;
    QLabel *enddate;
    QDateEdit*enddateedit;
    QLabel *initialcapital;
    QLineEdit *initialcapedit;
    QLabel *showholdings;
    QCheckBox *holdingsbool;

    // Below divider
    QFrame *dividerline;

    QLabel *totalreturn;
    QLabel *sharpe;
    QLabel *hwm;
    QLabel *drawdown;
    QLabel *beta;
    QLabel *alpha;

};

#endif //ALGOBACKTESTER_QTWINDOW_HPP
