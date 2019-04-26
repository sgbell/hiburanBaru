/**
 * @author      : Sam Bell (sam@mimpidev.com)
 * @file        : MainWindow
 * @created     : Wednesday May 15, 2019 07:46:10 UTC
 */

#include "MainWindow.h"
#include <QMainWindow>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
   this->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
   //Found the the fullScreen call in QWidget still reports the wrong size of the Window, need to set it manually
}



MainWindow::~MainWindow() {
}

