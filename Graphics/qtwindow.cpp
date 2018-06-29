//
// Created by Evan Kirkiles on 6/15/18.
//

#include <QPushButton>
#ifndef QLabel
#include <QLabel>
#endif
#include <QDate>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDateEdit>
#include <QFile>
#include <QLineEdit>
#include <QCheckBox>
#include <iomanip>
#include <sstream>

#include "qtwindow.hpp"

AlgoWindow::AlgoWindow(TradingInterface* i_trader, MainStrategy* i_strat, Benchmark* i_bench, GNUPlotter* i_gnuplot,
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

    // INitialize the montecarlowindow
    montecarlowindow = new MCWindow(interface, startdateaddr, enddateaddr);

    // Vertical layouts
    auto mainLayout = new QVBoxLayout;
    auto topLayout = new QVBoxLayout;
    auto botLayout = new QVBoxLayout;

    // Horizontal layouts for top
    auto startdatelayout = new QHBoxLayout;
    auto enddatelayout = new QHBoxLayout;
    auto initialcap = new QHBoxLayout;
    auto holdingslay = new QHBoxLayout;

    // Horizontal layouts for bottom
    auto totalreturnlayout = new QHBoxLayout;
    auto sharpelayout = new QHBoxLayout;
    auto hwmlayout = new QHBoxLayout;
    auto drawdownlayout = new QHBoxLayout;
    auto betalayout = new QHBoxLayout;
    auto alphalayout = new QHBoxLayout;

    // Set size of the window
    setFixedSize(250, 700);

    // Create the widgets to add to vertical layout
    m_button = new QPushButton("Initialize Backtest", this);
    montecarlo_button = new QPushButton("Monte Carlo Simulation", this);
    startdate = new QLabel("Start Date: ", this);
    startdateedit = new QDateEdit(this);
    enddate = new QLabel("End Date: ", this);
    enddateedit = new QDateEdit(this);
    initialcapital = new QLabel("Initial Capital: ", this);
    initialcapedit = new QLineEdit(this);
    showholdings = new QLabel("Show Holdings", this);
    holdingsbool = new QCheckBox(this);

    // Dividing line between top and bottom layouts
    dividerline = new QFrame();
    dividerline->setFrameShape(QFrame::HLine);
    dividerline->setFixedSize(210, 3);

    // Performance labels
    totalreturn = new QLabel("Total Return: ", this);
    totalreturnlabel= new QLabel("", this);
    sharpe = new QLabel("Sharpe Ratio: ", this);
    sharpelabel = new QLabel("", this);
    hwm = new QLabel("High-Water Mark: ", this);
    hwmlabel = new QLabel("", this);
    drawdown = new QLabel("Max Drawdown: ", this);
    drawdownlabel = new QLabel("", this);
    beta = new QLabel("Beta:", this);
    betalabel = new QLabel("", this);
    alpha = new QLabel("Alpha:", this);
    alphalabel = new QLabel("", this);

    // Create horizontal layouts for top layout
    startdatelayout->addWidget(startdate);
    startdatelayout->addWidget(startdateedit);
    enddatelayout->addWidget(enddate);
    enddatelayout->addWidget(enddateedit);
    initialcap->addWidget(initialcapital);
    initialcap->addWidget(initialcapedit);
    holdingslay->addWidget(showholdings);
    holdingslay->addWidget(holdingsbool);

    // Create horizontal layouts for bottom layout
    totalreturnlayout->addWidget(totalreturn);
    totalreturnlayout->addWidget(totalreturnlabel);
    sharpelayout->addWidget(sharpe);
    sharpelayout->addWidget(sharpelabel);
    hwmlayout->addWidget(hwm);
    hwmlayout->addWidget(hwmlabel);
    drawdownlayout->addWidget(drawdown);
    drawdownlayout->addWidget(drawdownlabel);
    betalayout->addWidget(beta);
    betalayout->addWidget(betalabel);
    alphalayout->addWidget(alpha);
    alphalayout->addWidget(alphalabel);


    // Add widgets to top layout
    topLayout->addWidget(m_button);
    topLayout->addLayout(startdatelayout);
    topLayout->addLayout(enddatelayout);
    topLayout->addLayout(initialcap);
    topLayout->addLayout(holdingslay);
    topLayout->addWidget(montecarlo_button);

    // Add widgets to bot layout
    botLayout->addLayout(alphalayout);
    botLayout->addLayout(betalayout);
    botLayout->addLayout(totalreturnlayout);
    botLayout->addLayout(sharpelayout);
    botLayout->addLayout(hwmlayout);
    botLayout->addLayout(drawdownlayout);

    // Set top widget properties
    m_button->setFixedSize(210, 100);
    m_button->setCheckable(true);
    montecarlo_button->setFixedSize(210, 30);
    montecarlo_button->setCheckable(true);
    montecarlo_button->setDisabled(true);
    startdate->setFixedSize(75, 10);
    startdateedit->setDisplayFormat("yyyy-MM-dd");
    startdateedit->setDate(QDate::fromString("2004-01-01", "yyyy-MM-dd"));
    enddate->setFixedSize(75, 10);
    enddateedit->setDisplayFormat("yyyy-MM-dd");
    enddateedit->setDate(QDate::fromString("2005-01-01", "yyyy-MM-dd"));
    initialcapital->setFixedSize(100, 13);
    initialcapedit->setText("10000000");
    initialcapedit->setValidator(new QIntValidator(1, 100000000, this));
    showholdings->setFixedSize(175, 13);
    holdingsbool->setCheckState(Qt::CheckState::Unchecked);

    // Set bot widget properties
    totalreturn->setFixedSize(110, 30);
    totalreturn->setObjectName("mediumstatistic");
    totalreturnlabel->setAlignment(Qt::AlignmentFlag::AlignCenter);
    totalreturnlabel->setObjectName("mediumstatisticlabel");
    sharpe->setFixedSize(110, 30);
    sharpe->setObjectName("mediumstatistic");
    sharpelabel->setAlignment(Qt::AlignmentFlag::AlignCenter);
    sharpelabel->setObjectName("mediumstatisticlabel");
    hwm->setFixedSize(110, 30);
    hwmlabel->setAlignment(Qt::AlignmentFlag::AlignCenter);
    hwmlabel->setObjectName("performancelabel");
    drawdown->setFixedSize(110, 30);
    drawdownlabel->setAlignment(Qt::AlignmentFlag::AlignCenter);
    drawdownlabel->setObjectName("performancelabel");
    beta->setFixedSize(110, 30);
    beta->setObjectName("bigstatistic");
    betalabel->setAlignment(Qt::AlignmentFlag::AlignCenter);
    betalabel->setObjectName("bigstatisticlabel");
    alpha->setFixedSize(110, 30);
    alpha->setObjectName("bigstatistic");
    alphalabel->setAlignment(Qt::AlignmentFlag::AlignCenter);
    alphalabel->setObjectName("bigstatisticlabel");

    // Add sub-vertical layouts to main vert layout
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(dividerline);
    mainLayout->addLayout(botLayout);

    // Add the slot and signal for the button click
    connect(m_button, SIGNAL (clicked(bool)), this, SLOT (buttonClicked(bool)));

    // Add the slot and signals for the variables changed
    connect(startdateedit, SIGNAL(dateChanged(QDate)), this, SLOT(varsChanged()));
    connect(enddateedit, SIGNAL(dateChanged(QDate)), this, SLOT(varsChanged()));
    connect(initialcapedit, SIGNAL(textChanged(QString)), this, SLOT(varsChanged()));
    connect(holdingsbool, SIGNAL(stateChanged(int)), this, SLOT(varsChanged()));

    // Add the slot and signal for ensuring viable backtest period
    connect(startdateedit, SIGNAL(dateChanged(QDate)), this, SLOT(setMinDate()));
    connect(enddateedit, SIGNAL(dateChanged(QDate)), this, SLOT(setMaxDate()));

    // Create window for Monte Carlo analysis when the button is clicked
    connect(montecarlo_button, SIGNAL(clicked(bool)), this, SLOT(showMonteCarlo()));

    // Set window options
    setLayout(mainLayout);
    setAttribute(Qt::WA_QuitOnClose);
    setWindowTitle(tr("Backtest Control"));
    setObjectName("main_window");

    // Set the style sheet
    QFile File("./Graphics/stylesheet.qss");
    File.open(QFile::ReadOnly);
    QString StyleSheet = QLatin1String(File.readAll());
    setStyleSheet(StyleSheet);
}

// Override close event
void AlgoWindow::closeEvent(QCloseEvent *event) {

    // Close the MCWindow
    montecarlowindow->close();
    QWidget::closeEvent(event);
}

// Private slot that shows the monte carlo window
void AlgoWindow::showMonteCarlo() {
    montecarlowindow->show();
}

// Private slot that changes the init variables when text is changed
void AlgoWindow::varsChanged() {
    m_button->setDisabled(false);
    m_button->setChecked(false);
    m_button->setText("Initialize Backtest");
}

// Private slot that listens for when button is clicked
void AlgoWindow::buttonClicked(bool checked) {
    if (checked){
        *startdateaddr = startdateedit->text().toStdString();
        *enddateaddr = enddateedit->text().toStdString();
        *initialcapaddr = (double)initialcapedit->text().toInt();
        if(holdingsbool->checkState() == Qt::CheckState::Checked) {
            *showholds = 1;
        } else {
            *showholds = 0;
        }
        gnuplot->initPlot();
        m_button->setText("Run Backtest");
        montecarlo_button->setDisabled(true);
        montecarlowindow->hide();
    } else {
        m_button->setText("Built Backtest");
        interface->runbacktest(*strat, *bench, gnuplot);
        performanceValues();
        m_button->setDisabled(true);
        montecarlo_button->setDisabled(false);
    }
}

// Private slot that makes sure the startdate is never after the enddate
void AlgoWindow::setMaxDate() {
    QDate endate = enddateedit->date();
    startdateedit->setMaximumDate(endate.addDays(-1));
}

// Private slot that makes sure the enddate is never before the startdate
void AlgoWindow::setMinDate() {
    QDate stadate = startdateedit->date();
    enddateedit->setMinimumDate(stadate.addDays(+1));
}

// Update QLabels with performance variables
void AlgoWindow::performanceValues() {
    map<string, double>performance = interface->portfolio.getPerformanceStats(interface->benchmarkportfolio);

    // Set label values
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2) << performance["totalreturn"] * 100;
    totalreturnlabel->setText(QString(stream.str().append("%").c_str()));
    double annualizingfactor = startdateedit->date().daysTo(enddateedit->date()) / 365;
    if (performance["totalreturn"] / annualizingfactor > 0.25) {
        totalreturnlabel->setStyleSheet("color: #D4AF37");
    } else if (performance["totalreturn"] / annualizingfactor > 0.15) {
        totalreturnlabel->setStyleSheet("color: #77d68d");
    } else if (performance["totalreturn"] / annualizingfactor < 0) {
        totalreturnlabel->setStyleSheet("color: #f44e42");
    } else {
        totalreturnlabel->setStyleSheet("color: #ddeaff");
    }
    stream.str("");
    stream << std::fixed << std::setprecision(2) << performance["sharpe"];
    sharpelabel->setText(QString(stream.str().c_str()));
    if (performance["sharpe"] > 2) {
        sharpelabel->setStyleSheet("color: #D4AF37");
    } else if (performance["sharpe"] > 1) {
        sharpelabel->setStyleSheet("color: #77d68d");
    } else if (performance["sharpe"] < 0.75) {
        sharpelabel->setStyleSheet("color: #f44e42");
    } else {
        sharpelabel->setStyleSheet("color: #ddeaff");
    }
    stream.str("");
    stream << std::fixed << std::setprecision(2) << performance["hwm"] * 100;
    hwmlabel->setText(QString(stream.str().append("%").c_str()));
    stream.str("");
    stream << std::fixed << std::setprecision(2) << performance["maxdrawdown"] * -100;
    drawdownlabel->setText(QString(stream.str().append("%").c_str()));
    stream.str("");
    stream << std::fixed << std::setprecision(2) << performance["beta"];
    betalabel->setText(QString(stream.str().c_str()));
    if (abs(performance["beta"]) < 0.15) {
        betalabel->setStyleSheet("color: #D4AF37");
    } else if (abs(performance["beta"]) < 0.4) {
        betalabel->setStyleSheet("color: #77d68d");
    } else if (abs(performance["beta"]) > 1.0) {
        betalabel->setStyleSheet("color: #f44e42");
    } else {
        betalabel->setStyleSheet("color: #ddeaff");
    }
    stream.str("");
    stream << std::fixed << std::setprecision(2) << performance["alpha"];
    alphalabel->setText(QString(stream.str().c_str()));
    if (performance["alpha"] > 0.15) {
        alphalabel->setStyleSheet("color: #D4AF37");
    } else if (performance["alpha"] > 0.05) {
        alphalabel->setStyleSheet("color: #77d68d");
    } else if (performance["alpha"] < 0.0) {
        alphalabel->setStyleSheet("color: #f44e42");
    } else {
        alphalabel->setStyleSheet("color: #ddeaff");
    }
}