/**
 * @author      : Sam Bell (sam@mimpidev.com)
 * @file        : MpdConnector
 * @created     : Saturday Jun 01, 2019 12:15:53 UTC
 */

#ifndef MPDCONNECTOR_H

#define MPDCONNECTOR_H
#include <mpd/client.h>
#include "MpdConnectionException.h"
#include <QSettings>
#include "Typedef.h"
#include "MusicCollection.h"

class MpdConnector
{
    public:
        MpdConnector ();
        MpdConnector (QSettings &settings);
        virtual ~MpdConnector ();

        void connect();
        std::string getStatus() { return m_lastMessage; }
        MusicCollection* getList();
        songlist_t* getCurrentPlaylist();
        void playSongById(std::string songId);
        stringmap_t* getCurrentStatus();
        unsigned getVolumeLevel();
        void volumeDown();
        void volumeDown(int volumePoints);
        void volumeUp();
        void volumeUp(int volumePoints);
        void changeVolume(int volumePoints);
        void muteVolume();
        void toggleRepeat();
        void toggleRandom();
        void nextSong();
        void previousSong();
        void pausePlayback();
        std::string* getAlbumArt(std::string url);
        void skipToSong(std::string url);
        void clearQueueAndPlay(std::string url);

        MpdDirectory* getDirectory(std::string path);
        playlist_t* getPlaylists();
        artistlist_t* getArtists();
        albumlist_t* getAlbums(std::string artistName);
        MpdAlbum* getAlbum(std::string artistName, std::string albumName);
    private:
        /* private data */
        struct mpd_connection *m_conn = NULL;
        std::string *m_hostname;
        int m_port;
        std::string m_lastMessage;
        int m_mutedVolume=100;
};

#endif /* end of include guard MPDCONNECTOR_H */

