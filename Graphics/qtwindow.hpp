//
// Created by Evan Kirkiles on 6/15/18.
//

#ifndef ALGOBACKTESTER_QTWINDOW_HPP
#define ALGOBACKTESTER_QTWINDOW_HPP

#include <QWidget>
#include "../Infrastructure/interface.hpp"

// Window for displaying performance stats and start/stop button
class QPushButton;
class Window : public QWidget {

    TradingInterface* interface;
    BuyAndHoldStrategy* strat;
    Benchmark* bench;
    GNUPlotter* gnuplot;

    Q_OBJECT
public:
    explicit Window(TradingInterface* trader, BuyAndHoldStrategy* strat, Benchmark* bench, GNUPlotter* gnuplot, QWidget *parent = 0);
private slots:
    void buttonClicked();
private:
    QPushButton *m_button;
};

#endif //ALGOBACKTESTER_QTWINDOW_HPP
