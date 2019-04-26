/**
 * @author      : Sam Bell (sam@mimpidev.com)
 * @file        : webuihandler
 * @created     : Wednesday May 15, 2019 11:32:52 UTC
 */

#ifndef WEBUIHANDLER_H

#define WEBUIHANDLER_H
#include <QWebEngineUrlSchemeHandler>
#include <QXmlStreamWriter>
#include <QString>
#include <string>

#include "mpd/MpdConnector.h"
#include "tpl_engine/TemplateMap.h"


class WebUiHandler : public QWebEngineUrlSchemeHandler {
    Q_OBJECT

    public:
        const static QByteArray schemeName;
        const static QUrl mainUrl;
        WebUiHandler (QObject *parent = nullptr);
        void requestStarted(QWebEngineUrlRequestJob *job) override;
        static void registerUrlScheme();
        virtual ~WebUiHandler ();
        void setMpd(MpdConnector &mpdConnector) { m_mpd = &mpdConnector; }
        void directoryHTML(MpdDirectory *directory, QXmlStreamWriter *xmlWriter);
        void getStatus(QWebEngineUrlRequestJob *job);
        TemplateMap* convertMap(MpdDirectory *directory);
    private:
        /* private data */
        MpdConnector *m_mpd;
};

#endif /* end of include guard WEBUIHANDLER_H */

