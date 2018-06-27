# Event-Driven Backtest Environment for Algorithms in C++

Large project that will most likely take the entire summer, if not longer. Along the way I plan to pick up C++ as it is the fastest language for event-driven backtesting and is a great tool to have mastery of. 

Using my own C+ Yahoo Finance interface to download historical EOD data in .csv file format, which will then be used for backtesting. In the future, I will modify the data handler to integrate with the Bloomberg API on the Bloomberg Terminal in my school's library, which will also necessitate porting the program over to Windows, although that won't be much of a hassle.

Runs an event-driven loop that continuously checks an Event queue for market data updates, strategy-specific signals, orders, and fill events. Passes the market data updates through the given strategy which calculates the signals to send to the execution handler which then sends orders and finally updates the portfolio with the information of the fill. For visualization, I use GNUPlot and Qt: the former for graphing the equity curve and holdings information, and the latter for displaying performance statistics and setting various backtest environment variables, i.e. start date, end date, and initial capital. A screenshot of the interface:

<img src="/screenshot1.png" alt="Screenshot of interface"/>

It is split into several windows because I prefer using GNUplot for realtime updates to the graph rather than going to great lengths to set up multithreading in Qt for a single window application.
