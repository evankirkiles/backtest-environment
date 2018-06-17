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
    totalreturnlabel= new QLabel("a", this);
    sharpe = new QLabel("Sharpe Ratio: ", this);
    sharpelabel = new QLabel("a", this);
    hwm = new QLabel("High-Water Mark: ", this);
    hwmlabel = new QLabel("a", this);
    drawdown = new QLabel("Max Drawdown: ", this);
    drawdownlabel = new QLabel("a", this);

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

    // Add widgets to top layout
    topLayout->addWidget(m_button);
    topLayout->addLayout(startdatelayout);
    topLayout->addLayout(enddatelayout);
    topLayout->addLayout(initialcap);
    topLayout->addLayout(holdingslay);

    // Add widgets to bot layout
    botLayout->addLayout(totalreturnlayout);
    botLayout->addLayout(sharpelayout);
    botLayout->addLayout(hwmlayout);
    botLayout->addLayout(drawdownlayout);

    // Set top widget properties
    m_button->setFixedSize(210, 100);
    m_button->setCheckable(true);
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

    // Set bot widget propertis
    totalreturn->setFixedSize(110, 30);
    sharpe->setFixedSize(110, 30);
    hwm->setFixedSize(110, 30);
    drawdown->setFixedSize(110, 30);

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

    // Add the slot and signal for ensuring viable backtest period
    connect(startdateedit, SIGNAL(dateChanged(QDate)), this, SLOT(setMinDate()));
    connect(enddateedit, SIGNAL(dateChanged(QDate)), this, SLOT(setMaxDate()));

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
        performanceValues();
        m_button->setDisabled(true);
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
    totalreturnlabel->setText(QString(to_string(performance["totalreturn"] * 100).c_str()) + "%");
    sharpelabel->setText(QString(to_string(performance["sharpe"]).c_str()));
    hwmlabel->setText(QString(to_string(performance["hwm"] * 100).c_str()) + "%");
    drawdownlabel->setText(QString(to_string(performance["maxdrawdown"] * -100).c_str()) + "%");

    cout << "alpha: " << performance["alpha"] << ", beta: " << performance["beta"] <<  ", mean: " << performance["mean"] <<
         ", variance:" << performance["variance"] << endl;
}