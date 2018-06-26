//
// Created by Evan Kirkiles on 6/25/18.
//

#include "montecarlo.h"

// Constructor for building Monte Carlo Qt window
MCWindow::MCWindow(TradingInterface *i_interface, QWidget *parent) : QWidget(parent) {

    // Instance pointers
    interface = i_interface;

    // Window settings
    setFixedSize(200, 200);
    setWindowTitle("Monte Carlo Analysis");
    setObjectName("monte_window");
    // Set the stylesheet
    QFile File("./Graphics/stylesheet.qss");
    File.open(QFile::ReadOnly);
    QString StyleSheet = QLatin1String(File.readAll());
    setStyleSheet(StyleSheet);
}