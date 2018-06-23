//
// Created by Evan Kirkiles on 6/15/18.
//

#include "qtwindow.hpp"
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
#include <QDesktopWidget>
#include <QScreen>
#include <QGuiApplication>

#include <iomanip>
#include <sstream>
#ifndef min
#include <algorithm>
#endif
#include <thread>

QT_CHARTS_USE_NAMESPACE

AlgoWindow::AlgoWindow(TradingInterface* i_trader, MainStrategy* i_strat, Benchmark* i_bench, QLineSeries *lineseries,
                       QLineSeries *i_benchseries, string *i_startdateaddr, string *i_enddateaddr,
                       double* i_initialcapitaladdr, int* showholdingsval,
                       QWidget *parent) : QWidget(parent) {

    interface = i_trader;
    strat = i_strat;
    bench = i_bench;
    startdateaddr = i_startdateaddr;
    enddateaddr = i_enddateaddr;
    initialcapaddr = i_initialcapitaladdr;
    showholds = showholdingsval;

    // Vertical layouts
    auto mainLayout = new QVBoxLayout;
    auto topLayout = new QHBoxLayout;
    auto botLayout1 = new QHBoxLayout;
    auto botLayout2 = new QHBoxLayout;

    // Horizontal layouts for top
    auto startdatelayout = new QHBoxLayout;
    auto enddatelayout = new QHBoxLayout;
    auto initialcap = new QHBoxLayout;
    auto holdingslay = new QHBoxLayout;

    // Vertical top groupings
    auto secondcolumnlayout = new QVBoxLayout;
    auto thirdcolumnlayout = new QVBoxLayout;

    // Create fourth column with a special background color
    QWidget* fourthcolbackground = new QWidget(this);
    fourthcolbackground->setObjectName("fourthcolbackground");
    fourthcolbackground->setFixedSize(210, 70);
    auto fourthcolumnlayout = new QVBoxLayout(fourthcolbackground);

    // Horizontal layouts for bottom
    auto totalreturnlayout = new QHBoxLayout;
    auto sharpelayout = new QHBoxLayout;
    auto hwmlayout = new QHBoxLayout;
    auto drawdownlayout = new QHBoxLayout;
    auto betalayout = new QHBoxLayout;
    auto alphalayout = new QHBoxLayout;

    // Set size of the window
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect dims = screen->geometry();
    setFixedSize(std::min(2000, (int)floor(0.7*dims.width())), std::min(500, (int)floor(0.7*dims.height())));

    // Create the widgets to add to top layout
    m_button = new QPushButton("Initialize Backtest", this);
    startdate = new QLabel("Start Date: ", this);
    startdateedit = new QDateEdit(this);
    enddate = new QLabel("End Date: ", this);
    enddateedit = new QDateEdit(this);
    initialcapital = new QLabel("Initial Capital: ", this);
    initialcapedit = new QLineEdit(this);
    showholdings = new QLabel("Show Holdings", this);
    holdingsbool = new QCheckBox(this);
    algoname = new QLabel("Algorithm Title: ", this);
    algonamedisplay = new QLabel(QString(strat->title.c_str()), this);

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

    // Graph settings
    series = lineseries;
    benchseries = i_benchseries;
    baseline = new QLineSeries();
    baseline->append(startdateedit->dateTime().toMSecsSinceEpoch(), 0);
    baseline->append(enddateedit->dateTime().toMSecsSinceEpoch(), 0);

    // Create the QCustomPlot
    qcpplot = new QCustomPlot();
    qcpplot->addGraph(); // blue line
    qcpplot->graph(0)->setPen(QPen(QColor(40, 110, 255)));
    qcpplot->addGraph(); // red line
    qcpplot->graph(1)->setPen(QPen(QColor(255, 110, 40)));
    qcpplot->setFixedHeight(300);

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    qcpplot->xAxis->setTicker(timeTicker);
    qcpplot->axisRect()->setupFullAxesBox();
    qcpplot->yAxis->setRange(-1.2, 1.2);

    // make left and bottom axes transfer their ranges to right and top axes:
    connect(qcpplot->xAxis, SIGNAL(rangeChanged(QCPRange)), qcpplot->xAxis2, SLOT(setRange(QCPRange)));
    connect(qcpplot->yAxis, SIGNAL(rangeChanged(QCPRange)), qcpplot->yAxis2, SLOT(setRange(QCPRange)));

// setup a timer that repeatedly calls MainWindow::realtimeDataSlot:
    QTimer* dataTimer = new QTimer(this);
    connect(dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
    dataTimer->start(0); // Interval 0 means to refresh as fast as possible

    /*
    // Create the chart
    chart = new QChart();
    //chart->legend()->hide();
    chart->addSeries(baseline);
    chart->addSeries(series);
    chart->addSeries(benchseries);
    chart->setTitle("Equity Curve");
    chart->setTitleBrush(QBrush(QColor("#FFFFFF")));

    // CHART CUSTOMIZATIONS
    // Axes settings
    axisX = new QDateTimeAxis();
    axisX->setFormat("yyyy-MM-dd");
    axisY = new QValueAxis();
    // Custom grid lines and shades
    QPen gridPen(QColor("#464646"), 2, Qt::DotLine);
    axisX->setGridLinePen(gridPen);
    axisY->setGridLinePen(gridPen);
    // Custom axis colors
    QPen axisPen(QColor("#FFFFFF"), 2, Qt::SolidLine);
    axisX->setLinePen(gridPen);
    axisY->setLinePen(axisPen);
    // Custom axis label colors
    QBrush axisBrush(QColor("#FFFFFF"));
    axisX->setLabelsBrush(axisBrush);
    axisY->setLabelsBrush(axisBrush);

    // SET RANGES
    axisX->setRange(QDateTime::fromString("2004-01-01", "yyyy-MM-dd"), QDateTime::fromString("2005-01-01", "yyyy-MM-dd"));
    axisY->setRange(-40, 10);

    // Apply the axes to the chart
    chart->setAxisX(axisX, series);
    chart->setAxisX(axisX, benchseries);
    chart->setAxisX(axisX, baseline);
    chart->setAxisY(axisY, series);
    chart->setAxisY(axisY, benchseries);
    chart->setAxisY(axisY, baseline);

    // Create the chartview
    chartview = new QtCharts::QChartView(chart);
    chartview->setRenderHints(QPainter::Antialiasing);
    chartview->setFixedHeight(300);
    chartview->setRubberBand(QChartView::HorizontalRubberBand);

    // Also set the background colors of both the chartview and the chart
    chart->setBackgroundBrush(QColor("#232323"));
    chartview->setBackgroundBrush(QColor("#232323"));
    */

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
    secondcolumnlayout->addLayout(startdatelayout);
    secondcolumnlayout->addLayout(enddatelayout);
    topLayout->addLayout(secondcolumnlayout);
    thirdcolumnlayout->addLayout(initialcap);
    thirdcolumnlayout->addLayout(holdingslay);
    topLayout->addLayout(thirdcolumnlayout);
    fourthcolumnlayout->addWidget(algoname);
    fourthcolumnlayout->addWidget(algonamedisplay);
    topLayout->addWidget(fourthcolbackground);

    // Add widgets to bot layout
    botLayout1->addLayout(alphalayout);
    botLayout1->addLayout(betalayout);
    botLayout1->addLayout(totalreturnlayout);
    botLayout2->addLayout(sharpelayout);
    botLayout2->addLayout(hwmlayout);
    botLayout2->addLayout(drawdownlayout);
    alphalayout->setAlignment(Qt::AlignmentFlag::AlignCenter);
    betalayout->setAlignment(Qt::AlignmentFlag::AlignCenter);
    totalreturnlayout->setAlignment(Qt::AlignmentFlag::AlignCenter);
    sharpelayout->setAlignment(Qt::AlignmentFlag::AlignCenter);
    hwmlayout->setAlignment(Qt::AlignmentFlag::AlignCenter);
    drawdownlayout->setAlignment(Qt::AlignmentFlag::AlignCenter);

    // Set top widget properties
    m_button->setFixedSize(210, 50);
    m_button->setCheckable(true);
    startdate->setFixedSize(75, 15);
    startdateedit->setDisplayFormat("yyyy-MM-dd");
    startdateedit->setDate(QDate::fromString("2004-01-01", "yyyy-MM-dd"));
    startdateedit->setFixedSize(100, 15);
    enddate->setFixedSize(75, 15);
    enddateedit->setDisplayFormat("yyyy-MM-dd");
    enddateedit->setDate(QDate::fromString("2005-01-01", "yyyy-MM-dd"));
    enddateedit->setFixedSize(100, 15);
    initialcapital->setFixedSize(100, 15);
    initialcapedit->setText("10000000");
    initialcapedit->setValidator(new QIntValidator(1, 100000000, this));
    initialcapedit->setFixedSize(75, 15);
    showholdings->setFixedSize(165, 15);
    holdingsbool->setCheckState(Qt::CheckState::Unchecked);
    algoname->setAlignment(Qt::AlignmentFlag::AlignCenter);
    algonamedisplay->setObjectName("algonamedisplay");
    algonamedisplay->setAlignment(Qt::AlignmentFlag::AlignCenter);

    // Set bot widget properties
    totalreturn->setFixedSize(125, 30);
    totalreturn->setObjectName("bigstatistic");
    totalreturnlabel->setAlignment(Qt::AlignmentFlag::AlignCenter);
    totalreturnlabel->setObjectName("bigstatisticlabel");
    sharpe->setFixedSize(125, 30);
    sharpe->setObjectName("mediumstatistic");
    sharpelabel->setAlignment(Qt::AlignmentFlag::AlignCenter);
    sharpelabel->setObjectName("mediumstatisticlabel");
    hwm->setFixedSize(125, 30);
    hwm->setObjectName("mediumstatistic");
    hwmlabel->setAlignment(Qt::AlignmentFlag::AlignCenter);
    hwmlabel->setObjectName("performancelabel");
    drawdown->setFixedSize(125, 30);
    drawdown->setObjectName("mediumstatistic");
    drawdownlabel->setAlignment(Qt::AlignmentFlag::AlignCenter);
    drawdownlabel->setObjectName("performancelabel");
    beta->setFixedSize(125, 30);
    beta->setObjectName("bigstatistic");
    betalabel->setAlignment(Qt::AlignmentFlag::AlignCenter);
    betalabel->setObjectName("bigstatisticlabel");
    alpha->setFixedSize(125, 30);
    alpha->setObjectName("bigstatistic");
    alphalabel->setAlignment(Qt::AlignmentFlag::AlignCenter);
    alphalabel->setObjectName("bigstatisticlabel");

    // Add sub-vertical layouts to main vert layout
    mainLayout->addLayout(topLayout);
    //mainLayout->addWidget(chartview);
    mainLayout->addWidget(qcpplot);
    mainLayout->addLayout(botLayout1);
    mainLayout->addLayout(botLayout2);

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
        // Create the baseline for returns
        baseline->clear();
        baseline->append(startdateedit->dateTime().toMSecsSinceEpoch(), 0);
        baseline->append(enddateedit->dateTime().toMSecsSinceEpoch(), 0);

        if(holdingsbool->checkState() == Qt::CheckState::Checked) {
            *showholds = 1;
        } else {
            *showholds = 0;
        }

        axisX->setRange(startdateedit->dateTime(), enddateedit->dateTime());

        //gnuplot->initPlot();
        m_button->setText("Run Backtest");
    } else {
        m_button->setText("Built Backtest");
        interface->runbacktest(*strat, *bench, series, benchseries);
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

void AlgoWindow::realtimeDataSlot() {
    static QTime time(QTime::currentTime());
// calculate two new data points:
    double key = time.elapsed() / 1000.0; // time elapsed since start of demo, in seconds

// QVector<double> x(101); // initialize with entries 0..100
// for (int i=0; i<101; ++i)
// {
// //x = data1 ;// x goes from -1 to 1
// //y = x*x; // let's plot a quadratic function
// }

    static double lastPointKey = 0;
    if (key - lastPointKey > 0.002) // at most add point every 2 ms
    {
// add data to lines:
        qcpplot->graph(0)->addData(key, qSin(key) + qrand() / (double) RAND_MAX * 1 * qSin(key / 0.3843));
        //qcpplot->graph(0)->setData(key,(**data1));
        qcpplot->graph(1)->addData(key, qCos(key)+qrand()/(double)RAND_MAX*0.5*qSin(key/0.4364));
// rescale value (vertical) axis to fit the current data:
        qcpplot->graph(0)->rescaleValueAxis();
        qcpplot->graph(1)->rescaleValueAxis(true);
        lastPointKey = key;
    }
// make key axis range scroll with the data (at a constant range size of 8):
    qcpplot->xAxis->setRange(key, 8, Qt::AlignRight);
    qcpplot->replot();
}