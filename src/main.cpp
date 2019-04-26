/**
 * @author      : Sam Bell (sam@mimpidev.com)
 * @file        : main
 * @created     : Sunday May 12, 2019 11:52:21 UTC
 */

#include <QApplication>
#include <QWebEngineView>
#include <QQuickStyle>
#include <QWebEngineProfile>
#include <QWebEnginePage>
#include <QDebug>
#include <QDesktopWidget>
#include <QThread>
#include <QFile>

#include "MainWindow.h"
#include "WebUiHandler.h"

int main(int argc, char* argv[]) {
   QCoreApplication::setOrganizationName("Mimpi Development");
   QCoreApplication::setApplicationName("Hiburan Baru");
   QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
   QApplication app(argc,argv);

   QQuickStyle::setStyle(QStringLiteral("Material"));

   WebUiHandler::registerUrlScheme();
 
   MainWindow *mainWindow = new MainWindow(nullptr);
   QRect desktopSize = QApplication::desktop()->screenGeometry();
   // Doing this rather than using fullScreen on the MainWindow widget because the qt system lies about the size of the window when fullscreen is used.
   //mainWindow->setGeometry(0,0,desktopSize.width(),desktopSize.height());
   mainWindow->setGeometry(0,0,800,480);

   QWebEngineView *webView = new QWebEngineView(mainWindow);
   QWebEngineProfile profile;

   WebUiHandler handler;
   profile.installUrlSchemeHandler(WebUiHandler::schemeName, &handler);
   

   // qDebug() << "Main Window: " << mainWindow->width() << "x" << mainWindow->height();
   QFile *etcConf = new QFile("/etc/hiburan.conf");
   QSettings *settings = nullptr;
   if (etcConf->exists()){
       settings = new QSettings("/etc/hiburan.conf", QSettings::NativeFormat);
   } else {
       settings = new QSettings(std::string(std::getenv("HOME")).append("/.config/hiburan.conf").c_str(), QSettings::NativeFormat);
   }
   MpdConnector *mpdConnection = new MpdConnector(*settings);
   handler.setMpd(*mpdConnection);

   QWebEnginePage page(&profile, webView);
   mainWindow->setCentralWidget(webView);
   page.load(WebUiHandler::mainUrl);
   webView->setContextMenuPolicy(Qt::NoContextMenu);
   webView->setPage(&page);
   mainWindow->show();

   return app.exec();  
}