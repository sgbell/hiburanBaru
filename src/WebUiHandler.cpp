/**
 * @author      : Sam Bell (sam@mimpidev.com)
 * @file        : webuihandler
 * @created     : Wednesday May 15, 2019 11:39:45 UTC
 */

#include "WebUiHandler.h"

#include <QFile>
#include <QWebEngineUrlRequestJob>
#include <QWebEngineUrlScheme>
#include <QImageReader>
#include <QImage>
#include <QBuffer>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QUrl>
#include <boost/algorithm/string.hpp>
#include <memory>

#include "tpl_engine/Template.h"
#include "tpl_engine/TemplateList.h"
#include "tpl_engine/TemplateString.h"
#include "tpl_engine/TemplateParserException.h"

#include "mpd/MpdConnectionException.h"
#include "mpd/MpdPlaylist.h"
#include "mpd/MpdDirectory.h"
#include "mpd/MpdSong.h"
#include "mpd/MpdArtist.h"
#include "mpd/MpdAlbum.h"
#include <mpd/client.h>

#define SCHEMENAME "hibu"

const QByteArray WebUiHandler::schemeName = QByteArrayLiteral(SCHEMENAME);
const QUrl WebUiHandler::mainUrl = QUrl(QStringLiteral(SCHEMENAME "://main"));

WebUiHandler::WebUiHandler(QObject *parent) : QWebEngineUrlSchemeHandler(parent) {
}

void WebUiHandler::requestStarted(QWebEngineUrlRequestJob *job){
    static const QUrl webUiOrigin(QStringLiteral(SCHEMENAME ":"));
    static const QByteArray GET(QByteArrayLiteral("GET"));
    static const QByteArray POST(QByteArrayLiteral("POST"));

    QByteArray method = job->requestMethod();
    QUrl url = job->requestUrl();
    QUrl initiator = job->initiator();

    if (method == GET){
        //qDebug() << url.toString();
        if (url == QUrl(QStringLiteral("hibu://assets/css/all.css"))){
            QFile *file = new QFile(QStringLiteral(":assets/css/all.css"));
            file->open(QIODevice::ReadOnly);
            job->reply(QByteArrayLiteral("text/css"), file);
        } else if (url == QUrl(QStringLiteral("hibu://assets/webfonts/fa-solid-900.woff2"))){
            QFile *file = new QFile(QStringLiteral(":assets/webfonts/fa-solid-900.woff2"));
            file->open(QIODevice::ReadOnly);
            job->reply(QByteArrayLiteral("font/woff2"), file);
        } else if (url == mainUrl){
            //Main Screen
            QFile *file = new QFile(QStringLiteral(":main.html"), job);
            if (file->exists()){
                file->open(QIODevice::ReadOnly);
                job->reply(QByteArrayLiteral("text/html"), file);
            }
        } else {
            // 1. Convert QUrl to string
            std::string urlString = url.toString().toStdString();
            // 2. Remove protocol from the front of the url string
            urlString = urlString.substr(std::string(SCHEMENAME).length(),urlString.length());
            while (urlString.at(0)==':' ||
                   urlString.at(0)=='/'){
                urlString=urlString.substr(1,urlString.length());
            }
            if (urlString!="status"){
                qDebug() << "Url Minus the protocol" << QString(urlString.c_str());
            }
            // 3. Split remoaining url, and handle request
            stringlist_t urlSplit;
            boost::split(urlSplit, urlString, boost::is_any_of("/"));
            auto urlSplitIt = urlSplit.begin();

            if ((*urlSplitIt)=="library"){
                // Handle this here for now, but I will want to pass it off to it's own playlist url object/method eventually.
                urlSplitIt++;
                if ((*urlSplitIt)=="show"){
                    // Current Playlist
                    QBuffer *output = new QBuffer;

                    connect(job, SIGNAL(destroyed()), output, SLOT(deleteLater()));
                    output->open(QIODevice::WriteOnly);
                    urlSplitIt++;
                    if (urlSplitIt == urlSplit.end()){
                        QFile *file = new QFile(QStringLiteral(":library.html"), job);
                        if (file->exists()){
                            file->open(QIODevice::ReadOnly);
                            job->reply(QByteArrayLiteral("text/html"), file);
                        }
                    } else {
                        // Working with using a defined template on this sucker look at github.com/no1msd/mstch
                        std::unique_ptr<Template> tpl_instance;
                        try {
    		                QFile *templateFile = new QFile(QStringLiteral(":library_tpl.html"));
                            tpl_instance = std::unique_ptr<Template>(new Template(*templateFile));
    		            } catch (TemplateParserException *ex) {
                            qDebug() << ex->message().c_str();
                            exit(1);
    		            }
                        std::unique_ptr<TemplateMap> tplData = std::unique_ptr<TemplateMap>(new TemplateMap());
                        if ((*urlSplitIt)=="playlists"){
                            tplData->put("entityHeading", "Playlists");
                            tplData->put("entityClass", "playlist");
                            tplData->put("playlist", new TemplateList());
                            std::unique_ptr<playlist_t> playlists = std::unique_ptr<playlist_t>(m_mpd->getPlaylists());
                            if (playlists!=nullptr){
                                auto playlistIt = playlists->begin();
                                while (playlistIt != playlists->end()){
                                    ((TemplateList*)(tplData->get("playlist")))->push(*(new TemplateString(((*playlistIt)->getName()))));
                                    playlistIt++;
                                }
                            }
                        } else if ((*urlSplitIt)=="files"){
    		                QFile *templateFile = new QFile(QStringLiteral(":directory_tpl.html"));
                            tpl_instance = std::unique_ptr<Template>(new Template(*templateFile));
                            tplData->put("entityHeading", "Directories");
                            tplData->put("entityClass", "directory");
                            tplData->put("directory", new TemplateList());
                            urlSplitIt++;
                            std::unique_ptr<MpdDirectory> directory;
                            if (urlSplitIt == urlSplit.end() ||
                                *urlSplitIt=="0"){
                                directory = std::unique_ptr<MpdDirectory>(m_mpd->getDirectory(""));
                            } else {
                                // Current Directory listing
                                std::string decodedDirectory = QUrl::fromPercentEncoding((*urlSplitIt).c_str()).toStdString();
                                qDebug() << "Directory Id: " << decodedDirectory.c_str();
                                directory = std::unique_ptr<MpdDirectory>(m_mpd->getDirectory(decodedDirectory));
                            }
                            TemplateList* directoryTpl = (TemplateList*)tplData->get("directory");
                            auto directoryIt = directory->getSubdirectories()->begin();
                            while (directoryIt != directory->getSubdirectories()->end()){
                                TemplateMap* returnMap = new TemplateMap();
                                returnMap->put("entityClass", "directory");
                                returnMap->put("fullName", (*directoryIt)->getFullPath().c_str());
                                returnMap->put("fullNameUrl", QUrl::toPercentEncoding((*directoryIt)->getFullPath().c_str()).toStdString());
                                returnMap->put("name", (*directoryIt)->getName().c_str());
                                directoryTpl->push(*returnMap);
                                directoryIt++;
                            }
                            auto songIt = directory->getSongs()->begin();
                            while (songIt != directory->getSongs()->end()){
                                TemplateMap* songMap = new TemplateMap();
	                            songMap->put("filename", (*songIt)->getName().c_str());
	                            songMap->put("fullName", (*songIt)->getFullPath().c_str());
                                songMap->put("fullNameUrl", QUrl::toPercentEncoding((*songIt)->getFullPath().c_str()).toStdString());
	                            songMap->put("songName", (*songIt)->getTitle().c_str());
	                            songMap->put("album", (*songIt)->getAlbum().c_str());
	                            songMap->put("entityClass", "song");
                                directoryTpl->push(*songMap);
                                songIt++;
                            }
                        } else if ((*urlSplitIt)=="artists"){
                            try {
    		                    QFile *templateFile = new QFile(QStringLiteral(":artist_tpl.html"));
                                tpl_instance = std::unique_ptr<Template>(new Template(*templateFile));
    		                } catch (TemplateParserException *ex) {
                            }
                            tplData->put("entityHeading", "Artists/Albums");
                            tplData->put("entityClass", "artists");
                            tplData->put("artists", new TemplateList());
                            urlSplitIt++;
                            try {
                                std::unique_ptr<artistlist_t> artistList = std::unique_ptr<artistlist_t>(m_mpd->getArtists());
                                TemplateList *templateArtists = (TemplateList*)tplData->get("artists");
                                auto artistIt = artistList->begin();
                                while (artistIt != artistList->end()){
                                    if ((*artistIt)->getName()!=""){
                                        TemplateMap* artistMap = new TemplateMap();
                                        artistMap->put("entityClass", "artist");
                                        artistMap->put("name", (*artistIt)->getName().c_str());
                                        artistMap->put("nameUrl", QUrl::toPercentEncoding((*artistIt)->getName().c_str()).toStdString());
                                        templateArtists->push(*artistMap);
                                    }
                                    artistIt++;
                                }
                            } catch (MpdConnectionException mce){
                            }
                        } else if ((*urlSplitIt)=="artist"){
                            urlSplitIt++;
                            if (urlSplitIt !=urlSplit.end()){
                                std::string artistName = *urlSplitIt;
                                std::string decodedArtist = QUrl::fromPercentEncoding(artistName.c_str()).toStdString();
                                urlSplitIt++;
                                if (urlSplitIt == urlSplit.end()){
                                    try {
    		                            QFile *templateFile = new QFile(QStringLiteral(":album_tpl.html"));
                                        tpl_instance = std::unique_ptr<Template>(new Template(*templateFile));
    		                        } catch (TemplateParserException *ex) {
                                    }
                                    tplData->put("entityHeading", "Artists/Albums");
                                    tplData->put("entityClass", "albums");
                                    tplData->put("albums", new TemplateList());
                                    std::unique_ptr<albumlist_t> albumList = std::unique_ptr<albumlist_t>(m_mpd->getAlbums(decodedArtist));
                                    TemplateList *templateAlbums = (TemplateList*)tplData->get("albums");
                                    auto albumIt = albumList->begin();
                                    while (albumIt != albumList->end()){
                                        TemplateMap *albumMap = new TemplateMap();
                                        albumMap->put("entityClass", "album");
                                        albumMap->put("name", (*albumIt)->getName());
                                        albumMap->put("albumArtistUrl", artistName);
                                        albumMap->put("albumArtist", decodedArtist);
                                        albumMap->put("nameUrl", QUrl::toPercentEncoding((*albumIt)->getName().c_str()).toStdString());
                                        templateAlbums->push(*albumMap);
                                        albumIt++;
                                    }
                                } else {
                                    // Load Song list from Album
                                    std::string albumName = *urlSplitIt;
                                    std::string decodedAlbum = QUrl::fromPercentEncoding(albumName.c_str()).toStdString();
                                    try {
    		                            QFile *templateFile = new QFile(QStringLiteral(":tracks_tpl.html"));
                                        tpl_instance = std::unique_ptr<Template>(new Template(*templateFile));
    		                        } catch (TemplateParserException *ex) {
                                    }
                                    tplData->put("entityHeading", "Album Details");
                                    tplData->put("entityClass", "album");
                                    tplData->put("album", decodedAlbum);
                                    tplData->put("albumUrl", QUrl::toPercentEncoding(albumName.c_str()).toStdString());
                                    tplData->put("albumArtist", decodedArtist);
                                    tplData->put("albumArtistUrl", artistName);
                                    tplData->put("tracks", new TemplateList());
                                    std::unique_ptr<MpdAlbum> album = std::unique_ptr<MpdAlbum>(m_mpd->getAlbum(decodedArtist, decodedAlbum));
                                    TemplateList *templateAlbum = (TemplateList*)tplData->get("tracks");
                                    tplData->put("releaseDate", album->getReleaseDate());
                                    auto songIt = album->getSongs()->begin();
                                    while (songIt != album->getSongs()->end()){
                                        TemplateMap *songMap = new TemplateMap();
                                        songMap->put("entityClass", "song");
                                        songMap->put("name", (*songIt)->getTitle());
                                        songMap->put("nameUrl", QUrl::toPercentEncoding((*songIt)->getName().c_str()).toStdString());
                                        songMap->put("albumUrl", QUrl::toPercentEncoding(albumName.c_str()).toStdString());
                                        songMap->put("albumArtistUrl", artistName);
                                        songMap->put("album", decodedAlbum);
                                        templateAlbum->push(*songMap);
                                        songIt++;
                                    }
                                }
                            }
                        }

                        try {
                            output->write(tpl_instance->compile(tplData.get())->c_str());
                        } catch (TemplateParserException e){
                            qDebug() << "Time to cry it couldn't create the template";
                        }
                        output->close();
                        job->reply(QByteArrayLiteral("text/html"), output);
                    }
                }
            } else if ((*urlSplitIt)=="playlist"){
                urlSplitIt++;
                if ((*urlSplitIt)=="show"){
                    std::unique_ptr<songlist_t> currentPlaylist = std::unique_ptr<songlist_t>(m_mpd->getCurrentPlaylist());
                    QBuffer *output = new QBuffer;
                    std::unique_ptr<Template> tpl_instance;

                    connect(job, SIGNAL(destroyed()), output, SLOT(deleteLater()));
                    output->open(QIODevice::WriteOnly);
                    try {
    		            QFile *templateFile = new QFile(QStringLiteral(":tracks_tpl.html"));
                        tpl_instance = std::unique_ptr<Template>(new Template(*templateFile));
                    } catch (TemplateParserException *ex) {
                    }
                    std::unique_ptr<TemplateMap> tplData = std::unique_ptr<TemplateMap>(new TemplateMap());
                    tplData->put("entityHeading", "Queue");
                    tplData->put("entityClass", "queue");
                    tplData->put("tracks", new TemplateList());
                    TemplateList *templateQueue = (TemplateList*)tplData->get("tracks");
                    auto currentPlaylistIt = currentPlaylist->begin();
                    while (currentPlaylistIt != currentPlaylist->end()){
                        TemplateMap *songMap = new TemplateMap();
                        songMap->put("nameUrl",QUrl::toPercentEncoding((*currentPlaylistIt)->getName().c_str()).toStdString());
                        songMap->put("name", (*currentPlaylistIt)->getName());
                        songMap->put("albumArtistUrl","playlist");
                        templateQueue->push(*songMap);
                        currentPlaylistIt++;
                    }
                    try {
                        output->write(tpl_instance->compile(tplData.get())->c_str());
                    } catch (TemplateParserException e){
                        qDebug() << "Time to cry it couldn't create the template";
                    }
                    output->close();
                    job->reply(QByteArrayLiteral("text/html"), output);
                } else {
                    std::string songPath=urlString.substr(9,urlString.length());
                    m_mpd->playSongById(songPath);
                    try {
                        getStatus(job);
                    } catch (MpdConnectionException &ex){
                        qDebug() << "Error: " << ex.message().c_str();
                    }
                }
            } else if ((*urlSplitIt)=="trackcontrol"){
                urlSplitIt++;
                if ((*urlSplitIt)=="previous"){
                    m_mpd->previousSong();
                } else if ((*urlSplitIt)=="pause"){
                    m_mpd->pausePlayback();
                } else if ((*urlSplitIt)=="next"){
                    m_mpd->nextSong();
                } else if ((*urlSplitIt)=="random"){
                    m_mpd->toggleRandom();
                } else if ((*urlSplitIt)=="repeat"){
                    m_mpd->toggleRepeat();
                }
            } else if ((*urlSplitIt)=="volume"){
                urlSplitIt++;
                if ((*urlSplitIt)=="up"){
                    urlSplitIt++;
                    if (urlSplitIt!=urlSplit.end()){
                        m_mpd->volumeUp(std::stoi((*urlSplitIt)));
                    } else {
                        m_mpd->volumeUp();
                    }
                } else if ((*urlSplitIt)=="mute"){
                    m_mpd->muteVolume();
                } else if ((*urlSplitIt)=="down"){
                    urlSplitIt++;
                    if (urlSplitIt!=urlSplit.end()){
                        m_mpd->volumeDown(std::stoi((*urlSplitIt)));
                    } else {
                        m_mpd->volumeDown();
                    }
                }
            } else if ((*urlSplitIt)=="settings"){
                urlSplitIt++;
                if (urlSplitIt==urlSplit.end()){
                    QFile *file = new QFile(QStringLiteral(":settings.html"), job);
                    if (file->exists()){
                        file->open(QIODevice::ReadOnly);
                        job->reply(QByteArrayLiteral("text/html"), file);
                    }
                } else if ((*urlSplitIt)=="network"){
                    qDebug() << "Network Settings";
                } else if ((*urlSplitIt)=="files"){
                    qDebug() << "Files Settings";
                } else if ((*urlSplitIt)=="sync"){
                    qDebug() << "Synchronization Settings";
                } else {
                    qDebug() << "Main Settings";
                }
            } else if ((*urlSplitIt)=="status"){
                getStatus(job);
            } else if ((*urlSplitIt)=="play"){
                urlSplitIt++;
                try {
                    if ((*urlSplitIt)=="playlist"){
                        // This is for handling the current queue, jumping to a track in the queue
                        urlSplitIt++;
                        m_mpd->skipToSong((*urlSplitIt));
                    } else {
                        // This is for replacing the queue with the selected song/album/directory
                        urlString=urlString.substr(5);
                        std::string decodedUrl = QUrl::fromPercentEncoding(urlString.c_str()).toStdString();
                        m_mpd->clearQueueAndPlay(decodedUrl);
                    }
                } catch (MpdConnectionException mce){
                    qDebug() << "Error: " << mce.message().c_str();
                }
                getStatus(job);
            } else if ((*urlSplitIt)=="queue"){
                qDebug() << "queue";
            } else if ((*urlSplitIt)=="albumart"){
                qDebug() << (*urlSplitIt).c_str();
                std::string  urlsplitString = *(urlSplitIt++);
                urlString=urlString.substr(urlsplitString.size(),urlString.size());
                qDebug() << urlString.c_str();
                QBuffer *output = new QBuffer;
                connect(job, SIGNAL(destroyed()), output, SLOT(deleteLater()));
                output->open(QIODevice::WriteOnly);
                std::unique_ptr<std::string> fileData = std::unique_ptr<std::string>(new std::string());
                try {
                    // Doesn't currently appear to be implemented correctly. Will need to pull the album art from the file in the next version
                    // Hook into musicBrainz in mpd, and pull it down, and insert it into the file
                    std::unique_ptr<std::string> fileData = std::unique_ptr<std::string>(m_mpd->getAlbumArt(urlString));
                    output->write(fileData->c_str());
                    output->close();
                    job->reply(QByteArrayLiteral("image/jpeg"), output);
                } catch (MpdConnectionException mce){
                    QFile *file = new QFile(QStringLiteral(":assets/images/no_album_image.jpg"), job);
                    if (file->exists()){
                        file->open(QIODevice::ReadOnly);
                    }
                    job->reply(QByteArrayLiteral("image/jpeg"), file);
                }
            }
        }
    }
}

void WebUiHandler::getStatus(QWebEngineUrlRequestJob *job){
    stringmap_ptr status = stringmap_ptr(m_mpd->getCurrentStatus());
    QBuffer *output = new QBuffer;
    connect(job, SIGNAL(destroyed()), output, SLOT(deleteLater()));
    output->open(QIODevice::WriteOnly);
    QJsonObject *statusJson = new QJsonObject();
    auto statusIt = status->begin();
    while (statusIt != status->end()){
        (*statusJson)[(statusIt->first).c_str()]=(statusIt->second).c_str();
        statusIt++;
    }
    QJsonDocument jsonDocument(*statusJson);
    output->write(jsonDocument.toJson(QJsonDocument::Compact));
    output->close();

    job->reply(QByteArrayLiteral("text/json"), output);
}

void WebUiHandler::directoryHTML(MpdDirectory *directory, QXmlStreamWriter *xmlWriter){
}

void WebUiHandler::registerUrlScheme(){
    QWebEngineUrlScheme webUiScheme(schemeName);
    webUiScheme.setFlags(QWebEngineUrlScheme::SecureScheme |
                         QWebEngineUrlScheme::LocalScheme |
                         QWebEngineUrlScheme::LocalAccessAllowed);
    QWebEngineUrlScheme::registerScheme(webUiScheme);
}

WebUiHandler::~WebUiHandler() {
}

TemplateMap* WebUiHandler::convertMap(MpdDirectory *directory){
    return nullptr;
}
