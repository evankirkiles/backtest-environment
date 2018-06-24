//
// Created by Evan Kirkiles on 6/15/18.
//

#ifndef ALGOBACKTESTER_QTWINDOW_HPP
#define ALGOBACKTESTER_QTWINDOW_HPP

#include <QWidget>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChartView>
#include <QtCharts/QValueAxis>
#include <QtCharts/QDateTimeAxis>
#ifndef TradingInterface
#include "../Infrastructure/interface.hpp"
#endif

// Window for displaying performance stats and start/stop button
class QPushButton;
class QLabel;
class QDateEdit;
class QLineEdit;
class QCheckBox;

// Custom chartview class to allow overriding functions
class CustomChartView : public QtCharts::QChartView {
public:
    string* startdate;
    string* enddate;
    QtCharts::QChart* chart;

    explicit CustomChartView(QtCharts::QChart* chart, string* i_startdate, string* i_enddate, QWidget *parent = 0);
    void mouseReleaseEvent(QMouseEvent *e) override;
};

class AlgoWindow : public QWidget {

    // Backtesting variables
    TradingInterface* interface;
    MainStrategy* strat;
    Benchmark* bench;

    bool initialized;

    string *startdateaddr;
    string *enddateaddr;
    double* initialcapaddr;
    int* showholds;

    Q_OBJECT
public:
    explicit AlgoWindow(TradingInterface* trader, MainStrategy* strat, Benchmark* bench, QtCharts::QLineSeries *algoseries,
                        QtCharts::QLineSeries *benchseries, string *startdate, string *enddate, double* initialcapitalval,
                        int* showholds, QWidget *parent = 0);

    void performanceValues();
    QLabel *totalreturnlabel;
    QLabel *sharpelabel;
    QLabel *hwmlabel;
    QLabel *drawdownlabel;
    QLabel *betalabel;
    QLabel *alphalabel;

    QtCharts::QLineSeries *series;
    QtCharts::QLineSeries *benchseries;
    CustomChartView *chartview;
    QtCharts::QChart *chart;
    QtCharts::QDateTimeAxis *axisX;
    QtCharts::QValueAxis *axisY;

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
    QLabel *algoname;
    QLabel *algonamedisplay;
    QLabel *totalreturn;
    QLabel *sharpe;
    QLabel *hwm;
    QLabel *drawdown;
    QLabel *beta;
    QLabel *alpha;
};

#endif //ALGOBACKTESTER_QTWINDOW_HPP
