/**
 * @author      : Sam Bell (sam@mimpidev.com)
 * @file        : MainWindow
 * @created     : Wednesday May 15, 2019 07:51:28 UTC
 */

#ifndef MAINWINDOW_H

#define MAINWINDOW_H
#include <QMainWindow>

class MainWindow: public QMainWindow {
    Q_OBJECT

    public:
        MainWindow (QWidget *parent=nullptr);
        virtual ~MainWindow ();
    private:
        /* private data */
};

#endif /* end of include guard MAINWINDOW_H */

