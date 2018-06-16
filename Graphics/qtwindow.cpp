//
// Created by Evan Kirkiles on 6/15/18.
//

#include "qtwindow.hpp"
#include <QPushButton>
#include <QLabel>
#include <QDate>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDateEdit>
#include <QFile>
#include <QLineEdit>
#include <QCheckBox>

AlgoWindow::AlgoWindow(TradingInterface* i_trader, BuyAndHoldStrategy* i_strat, Benchmark* i_bench, GNUPlotter* i_gnuplot,
                       string *i_startdateaddr, string *i_enddateaddr, double* i_initialcapitaladdr, int* showholdingsval,
                       QWidget *parent) : QWidget(parent) {

    interface = i_trader;
    strat = i_strat;
    bench = i_bench;
    gnuplot = i_gnuplot;
    startdateaddr = i_startdateaddr;
    enddateaddr = i_enddateaddr;
    initialcapaddr = i_initialcapitaladdr;
    showholds = showholdingsval;

    // Vertical layouts
    auto mainLayout = new QVBoxLayout;
    auto topLayout = new QVBoxLayout;
    auto botLayout = new QVBoxLayout;

    // Horizontal layouts
    auto startdatelayout = new QHBoxLayout;
    auto enddatelayout = new QHBoxLayout;
    auto initialcap = new QHBoxLayout;
    auto holdingslay = new QHBoxLayout;

    // Set size of the window
    setFixedSize(250, 500);

    // Create the widgets to add to vertical layout
    m_button = new QPushButton("Initialize Backtest", this);
    startdate = new QLabel("Start Date: ", this);
    startdateedit = new QDateEdit(this);
    enddate = new QLabel("End Date: ", this);
    enddateedit = new QDateEdit(this);
    initialcapital = new QLabel("Initial Capital: ", this);
    initialcapedit = new QLineEdit(this);
    showholdings = new QLabel("Show Holdings", this);
    holdingsbool = new QCheckBox(this);

    // Performance labels
    totalreturn = new QLabel("Total Return: ", this);
    sharpe = new QLabel("Sharpe Ratio: ", this);
    hwm = new QLabel("High-Water Mark: ", this);
    drawdown = new QLabel("Drawdown: ", this);

    // Create horizontal layouts for top layout
    startdatelayout->addWidget(startdate);
    startdatelayout->addWidget(startdateedit);
    enddatelayout->addWidget(enddate);
    enddatelayout->addWidget(enddateedit);
    initialcap->addWidget(initialcapital);
    initialcap->addWidget(initialcapedit);
    holdingslay->addWidget(showholdings);
    holdingslay->addWidget(holdingsbool);

    // Add widgets to top layout
    topLayout->addWidget(m_button);
    topLayout->addLayout(startdatelayout);
    topLayout->addLayout(enddatelayout);
    topLayout->addLayout(initialcap);
    topLayout->addLayout(holdingslay);

    // Set widget properties
    m_button->setFixedSize(210, 100);
    m_button->setCheckable(true);
    startdate->setFixedSize(75, 10);
    startdateedit->setDisplayFormat("yyyy-MM-dd");
    enddate->setFixedSize(75, 10);
    enddateedit->setDisplayFormat("yyyy-MM-dd");
    initialcapital->setFixedSize(100, 13);
    initialcapedit->setText("10000000");
    initialcapedit->setValidator(new QIntValidator(1, 100000000, this));
    showholdings->setFixedSize(175, 13);
    holdingsbool->setCheckState(Qt::CheckState::Unchecked);

    // Add widgets to bot layout
    botLayout->addWidget(totalreturn);
    botLayout->addWidget(sharpe);
    botLayout->addWidget(hwm);
    botLayout->addWidget(drawdown);

    // Add sub-vertical layouts to main vert layout
    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(botLayout);

    // Add the slot and signal for the button click
    connect(m_button, SIGNAL (clicked(bool)), this, SLOT (buttonClicked(bool)));

    // Add the slot and signals for the variables changed
    connect(startdateedit, SIGNAL(dateChanged(QDate)), this, SLOT(varsChanged()));
    connect(enddateedit, SIGNAL(dateChanged(QDate)), this, SLOT(varsChanged()));
    connect(initialcapedit, SIGNAL(textChanged(QString)), this, SLOT(varsChanged()));
    connect(holdingsbool, SIGNAL(stateChanged(int)), this, SLOT(varsChanged()));

    // Set window options
    setLayout(mainLayout);
    setWindowTitle(tr("Backtest Control"));
    setObjectName("main_window");

    // Set the style sheet
    QFile File("/Users/samkirkiles/Desktop/algobacktester/Graphics/stylesheet.qss");
    File.open(QFile::ReadOnly);
    QString StyleSheet = QLatin1String(File.readAll());
    setStyleSheet(StyleSheet);
}

// Private slot that changes the init variables when text is changed
void AlgoWindow::varsChanged() {
    *startdateaddr = startdateedit->text().toStdString();
    *enddateaddr = enddateedit->text().toStdString();
    *initialcapaddr = (double)initialcapedit->text().toInt();
    if(holdingsbool->checkState() == Qt::CheckState::Checked) {
        *showholds = 1;
    } else {
        *showholds = 0;
    }

    m_button->setDisabled(false);
    m_button->setChecked(false);
    m_button->setText("Initialize Backtest");
}

// Private slot that listens for when button is clicked
void AlgoWindow::buttonClicked(bool checked) {
    if (checked){

        gnuplot->initPlot();
        m_button->setText("Run Backtest");
    } else {
        m_button->setText("Built Backtest");
        interface->runbacktest(*strat, *bench, gnuplot);
        m_button->setDisabled(true);
    }
}