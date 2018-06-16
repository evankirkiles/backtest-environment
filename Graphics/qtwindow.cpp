//
// Created by Evan Kirkiles on 6/15/18.
//

#include "qtwindow.hpp"
#include <QPushButton>
#include <QVBoxLayout>
#include <QFile>

Window::Window(TradingInterface* i_trader, BuyAndHoldStrategy* i_strat, Benchmark* i_bench, GNUPlotter* i_gnuplot, QWidget *parent) : QWidget(parent) {

    interface = i_trader;
    strat = i_strat;
    bench = i_bench;
    gnuplot = i_gnuplot;

    QVBoxLayout* mainLayout = new QVBoxLayout;

    // Set size of the window
    setFixedSize(250, 500);

    // Create and position the button
    m_button = new QPushButton("Run Backtest", this);
    m_button->setGeometry(95, 210,80, 100);

    mainLayout->addWidget(m_button);

    connect(m_button, SIGNAL (clicked(bool)), this, SLOT (buttonClicked()));
    setLayout(mainLayout);
    setWindowTitle(tr("Algorithm Control"));
    setObjectName("main_window");

    // Set the style sheet
    QFile File("/Users/samkirkiles/Desktop/algobacktester/Graphics/stylesheet.qss");
    File.open(QFile::ReadOnly);
    QString StyleSheet = QLatin1String(File.readAll());
    setStyleSheet(StyleSheet);
}

// Private slot that listens for when button is clicked
void Window::buttonClicked() {
    interface->runbacktest(*strat, *bench, gnuplot);
}