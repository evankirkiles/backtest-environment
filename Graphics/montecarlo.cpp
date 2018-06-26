//
// Created by Evan Kirkiles on 6/25/18.
//

#include "montecarlo.hpp"

// Constructor for building Monte Carlo Qt window
MCWindow::MCWindow(TradingInterface *i_interface, QWidget *parent) : QWidget(parent) {

    // Instance pointers
    interface = i_interface;

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