/**
 * @author      : Sam Bell (sam@mimpidev.com)
 * @file        : MpdConnector
 * @created     : Saturday Jun 01, 2019 12:19:02 UTC
 */

#include "MpdConnector.h"
#include <QDebug>

#include <mpd/database.h>
#include <mpd/entity.h>
#include <mpd/player.h>
#include <mpd/client.h>
#include <mpd/recv.h>

#include <memory>

#include "MpdDirectory.h"
#include "MpdSong.h"
#include "MpdPlaylist.h"
#include "MpdArtist.h"
#include "MpdAlbum.h"

MpdConnector::MpdConnector() {
    
}

MpdConnector::MpdConnector(QSettings &settings){
   settings.beginGroup("mpd_server");
   m_hostname = new std::string(settings.value("hostname", QString("127.0.0.1")).toString().toUtf8().constData());
   m_port = settings.value("port", 6600).toInt();
   settings.endGroup();
}

void MpdConnector::connect() {
    qDebug() << m_hostname->c_str() << m_port;
    m_conn = mpd_connection_new(m_hostname->c_str(), m_port, 1000);
    if (mpd_connection_get_error(m_conn) != MPD_ERROR_SUCCESS)
        throw MpdConnectionException(*(new std::string(mpd_connection_get_error_message(m_conn))));
    qDebug() << "Successfully connected";
}

MpdDirectory* MpdConnector::getDirectory(std::string path){
    try {
        if (m_conn == NULL){
            connect();
        }
        mpd_send_list_meta(m_conn, path.c_str());
        if (mpd_connection_get_error(m_conn) != MPD_ERROR_SUCCESS){
            connect();
            mpd_send_list_meta(m_conn, path.c_str());
        }
        if (mpd_connection_get_error(m_conn) != MPD_ERROR_SUCCESS)
           throw MpdConnectionException(*(new std::string(mpd_connection_get_error_message(m_conn))));
        MpdDirectory *results = new MpdDirectory(path);
        mpd_entity *entity = mpd_recv_entity(m_conn);
        while (entity != NULL){
            switch (mpd_entity_get_type(entity)){
                case MPD_ENTITY_TYPE_DIRECTORY:
                    {
                        MpdDirectory *dir = new MpdDirectory(entity);
                        results->addDirectory(dir);
                    }
                    break;
                case MPD_ENTITY_TYPE_SONG:
                    {
                        MpdSong *song = new MpdSong(entity);
                        results->addSong(song);
                    }
                    break;
                case MPD_ENTITY_TYPE_PLAYLIST:
                    {
                        MpdPlaylist *playlist = new MpdPlaylist(entity);
                        results->addPlaylist(playlist);
                    }
                    break;
            }              
            entity = mpd_recv_entity(m_conn); 
        }
        return results; 
    } catch (MpdConnectionException &e){
        qDebug() << "Error: " << e.message().c_str();
    }
    return nullptr;
}

playlist_t* MpdConnector::getPlaylists(){
    /* mpd_send_list_playlists - only grabs the playlists created by mpd, 
     * need to get it to find all of the other playlists in the media folder
     */
    try {
        if (m_conn == NULL){
            connect();
        }
        mpd_send_list_playlists(m_conn);
        if (mpd_connection_get_error(m_conn) != MPD_ERROR_SUCCESS){
            connect();
            mpd_send_list_playlists(m_conn);
        }
        if (mpd_connection_get_error(m_conn) != MPD_ERROR_SUCCESS)
           throw MpdConnectionException(*(new std::string(mpd_connection_get_error_message(m_conn))));
        playlist_t *results = new playlist_t();
        mpd_playlist *playlist_entity = mpd_recv_playlist(m_conn);
        while (playlist_entity != NULL){
            MpdPlaylist *playlist = new MpdPlaylist(playlist_entity);
            results->push_back(playlist);
            playlist_entity = mpd_recv_playlist(m_conn);
        }

        return results;
    } catch (MpdConnectionException &e){
        qDebug() << "Error: " << e.message().c_str();
    }
    return nullptr;
}

MusicCollection* MpdConnector::getList() {
    try {
        if (m_conn == NULL){
            connect();
        }
        mpd_send_list_all(m_conn, NULL);
        if (mpd_connection_get_error(m_conn) != MPD_ERROR_SUCCESS){
            connect();
            mpd_send_list_all(m_conn, NULL);
        }
        if (mpd_connection_get_error(m_conn) != MPD_ERROR_SUCCESS)
           throw MpdConnectionException(*(new std::string(mpd_connection_get_error_message(m_conn))));
        MusicCollection *collection = new MusicCollection();
        mpd_entity *entity = mpd_recv_entity(m_conn);
        while (entity != NULL){
           switch (mpd_entity_get_type(entity)){
               case MPD_ENTITY_TYPE_DIRECTORY:
                   {
                       MpdDirectory *dir = new MpdDirectory(entity);
                   }
                   collection->addDirectory(entity);
                   break;
               case MPD_ENTITY_TYPE_SONG:
                   collection->addSong(entity);
                   break;
               case MPD_ENTITY_TYPE_PLAYLIST:
                   collection->addPlaylist(entity);
                   break;
           }              
           entity = mpd_recv_entity(m_conn);
        }
        return collection; 
    } catch (MpdConnectionException &e){
        qDebug() << "Error: " << e.message().c_str();
    }
    return nullptr;
}

MpdConnector::~MpdConnector() {
    mpd_connection_free(m_conn);
    m_lastMessage = "Successfully closed connection";
}

songlist_t* MpdConnector::getCurrentPlaylist() {
    songlist_t *currentPlaylist = new songlist_t();
    try {
        if (m_conn == NULL){
           connect();
        }
        mpd_send_list_queue_meta(m_conn);
        if (mpd_connection_get_error(m_conn) != MPD_ERROR_SUCCESS){
            connect();
            mpd_send_list_queue_meta(m_conn);
        }
        if (mpd_connection_get_error(m_conn) != MPD_ERROR_SUCCESS)
           throw MpdConnectionException(*(new std::string(mpd_connection_get_error_message(m_conn))));
        mpd_entity *entity = mpd_recv_entity(m_conn);
        while (entity != NULL){
           if (mpd_entity_get_type(entity)){
               MpdSong *song = new MpdSong(entity);
               currentPlaylist->push_back(song);
               entity = mpd_recv_entity(m_conn);
           }
        }
    } catch (MpdConnectionException &e) {
        qDebug() << "Error: " << e.message().c_str();
    }
    return currentPlaylist;
}

void MpdConnector::playSongById(std::string songId){
    int songIdInt = std::stoi(songId);

    mpd_run_play_id(m_conn, songIdInt);
    if (mpd_connection_get_error(m_conn) != MPD_ERROR_SUCCESS){
        connect();
        mpd_run_play_id(m_conn, songIdInt);
    }
    if (mpd_connection_get_error(m_conn) != MPD_ERROR_SUCCESS)
        throw MpdConnectionException(*(new std::string(mpd_connection_get_error_message(m_conn))));
}
        
stringmap_t* MpdConnector::getCurrentStatus(){
    stringmap_t *statusMap = new stringmap_t;
    try {
        if (m_conn == NULL){
           connect();
        }
        mpd_send_status(m_conn);
        if (mpd_connection_get_error(m_conn) != MPD_ERROR_SUCCESS){
            connect();
            mpd_send_status(m_conn);
        }
        struct mpd_status *status = mpd_recv_status(m_conn);
        if (mpd_connection_get_error(m_conn) != MPD_ERROR_SUCCESS)
            throw MpdConnectionException(*(new std::string(mpd_connection_get_error_message(m_conn))));
        (*statusMap)["volume"]=std::to_string(mpd_status_get_volume(status));
        (*statusMap)["repeat"]=(mpd_status_get_repeat(status)==1?"true":"false");
        (*statusMap)["random"]=(mpd_status_get_random(status)==1?"true":"false");
        (*statusMap)["crossfade"]=(mpd_status_get_crossfade(status)==1?"true":"false");
        (*statusMap)["songPosition"]=mpd_status_get_song_id(status);
        switch (mpd_status_get_state(status)){
            case MPD_STATE_PLAY:
                (*statusMap)["state"]="PLAY";
                break;
            case MPD_STATE_PAUSE:
                (*statusMap)["state"]="PAUSE";
                break;
            case MPD_STATE_STOP:
                (*statusMap)["state"]="STOP";
                break;
            default:
                (*statusMap)["state"]="UNKNOWN";
        }
        (*statusMap)["bitRate"]=std::to_string(mpd_status_get_kbit_rate(status));
        (*statusMap)["elapsedTime"]=std::to_string(mpd_status_get_elapsed_time(status));
        (*statusMap)["totalTime"]=std::to_string(mpd_status_get_total_time(status));
 
        mpd_status_free(status);
        mpd_song *song = mpd_run_current_song(m_conn);
        if (song!= NULL){
            MpdSong *songObject = new MpdSong(song);
            (*statusMap)["songTitle"]=songObject->getTitle();
            (*statusMap)["artist"]=songObject->getArtist();
            (*statusMap)["album"]=songObject->getAlbum();
            (*statusMap)["fullPath"]=songObject->getFullPath();
        }
    } catch (MpdConnectionException &e) {
        qDebug() << "Error: " << e.message().c_str();
        (*statusMap)["state"]="STOP";
    }

    return statusMap;
}

unsigned MpdConnector::getVolumeLevel(){
    if (m_conn == NULL){
        connect();
    }
    mpd_send_status(m_conn);
    if (mpd_connection_get_error(m_conn) != MPD_ERROR_SUCCESS){
        connect();
        mpd_send_status(m_conn);
    }
    struct mpd_status *status = mpd_recv_status(m_conn);
    if (mpd_connection_get_error(m_conn) != MPD_ERROR_SUCCESS)
        throw MpdConnectionException(*(new std::string(mpd_connection_get_error_message(m_conn))));
    unsigned level = mpd_status_get_volume(status);
    mpd_status_free(status);
    return level;
}

void MpdConnector::volumeDown(){
    volumeDown(1);
}

void MpdConnector::volumeDown(int volumePoints){
    unsigned level = getVolumeLevel();
    level = level - volumePoints;
    if (level<0) level = 0;
    changeVolume(level);
}

void MpdConnector::volumeUp(){
    volumeUp(1);
}

void MpdConnector::volumeUp(int volumePoints){
    unsigned level = getVolumeLevel();
    level = level + volumePoints;
    if (level>100) level=100;
    changeVolume(level);
}

void MpdConnector::changeVolume(int level){
    if (m_conn == NULL){
        connect();
    }
    mpd_send_set_volume(m_conn, level); 
    if (mpd_connection_get_error(m_conn) != MPD_ERROR_SUCCESS){
        connect();
        mpd_send_set_volume(m_conn, level);
    }
    mpd_response_finish(m_conn);
    m_mutedVolume=level;
    //qDebug() << "Volume Level: " << level; 
}

void MpdConnector::muteVolume(){
    if (m_conn == NULL){
        connect();
    }
    unsigned currentVolume = getVolumeLevel();
    if (currentVolume==0){
        mpd_send_set_volume(m_conn, m_mutedVolume); 
    } else {
        mpd_send_set_volume(m_conn, 0); 
    }
    mpd_response_finish(m_conn);
}

void MpdConnector::toggleRepeat(){
    if (m_conn == NULL){
        connect();
    }
    mpd_send_status(m_conn);
    if (mpd_connection_get_error(m_conn) != MPD_ERROR_SUCCESS){
        connect();
        mpd_send_status(m_conn);
    }
    struct mpd_status *status = mpd_recv_status(m_conn);
    if (mpd_connection_get_error(m_conn) != MPD_ERROR_SUCCESS)
        throw MpdConnectionException(*(new std::string(mpd_connection_get_error_message(m_conn))));
    bool repeat = mpd_status_get_repeat(status);
    mpd_status_free(status);
    mpd_send_repeat(m_conn, !repeat);
    mpd_response_finish(m_conn);
}

void MpdConnector::toggleRandom(){
    if (m_conn == NULL){
        connect();
    }
    mpd_send_status(m_conn);
    if (mpd_connection_get_error(m_conn) != MPD_ERROR_SUCCESS){
        connect();
        mpd_send_status(m_conn);
    }
    struct mpd_status *status = mpd_recv_status(m_conn);
    if (mpd_connection_get_error(m_conn) != MPD_ERROR_SUCCESS)
        throw MpdConnectionException(*(new std::string(mpd_connection_get_error_message(m_conn))));
    bool random = mpd_status_get_random(status);
    mpd_status_free(status);
    mpd_send_random(m_conn, !random);
    mpd_response_finish(m_conn);
}

void MpdConnector::nextSong(){
    if (m_conn == NULL){
        connect();
    }
    mpd_run_next(m_conn);
    if (mpd_connection_get_error(m_conn) != MPD_ERROR_SUCCESS){
        connect();
        mpd_run_next(m_conn);
    }
    if (mpd_connection_get_error(m_conn) != MPD_ERROR_SUCCESS)
        throw MpdConnectionException(*(new std::string(mpd_connection_get_error_message(m_conn))));
    mpd_response_finish(m_conn);
}

void MpdConnector::previousSong(){
    if (m_conn == NULL){
        connect();
    }
    mpd_run_previous(m_conn);
    if (mpd_connection_get_error(m_conn) != MPD_ERROR_SUCCESS){
        connect();
        mpd_run_previous(m_conn);
    }
    if (mpd_connection_get_error(m_conn) != MPD_ERROR_SUCCESS)
        throw MpdConnectionException(*(new std::string(mpd_connection_get_error_message(m_conn))));
    mpd_response_finish(m_conn);
}

void MpdConnector::pausePlayback(){
    if (m_conn == NULL){
        connect();
    }
    // Get Status, If Stopped and queue has a song, play
    stringmap_ptr status = stringmap_ptr(this->getCurrentStatus());
    if ((*status)["state"]=="STOP"){
        mpd_run_play(m_conn);
        if (mpd_connection_get_error(m_conn) != MPD_ERROR_SUCCESS){
            connect();
            mpd_run_play(m_conn);
        }
        if (mpd_connection_get_error(m_conn) != MPD_ERROR_SUCCESS)
            throw MpdConnectionException(*(new std::string(mpd_connection_get_error_message(m_conn))));
    } else {
        // If Paused toggle pause
        mpd_run_toggle_pause(m_conn);
        if (mpd_connection_get_error(m_conn) != MPD_ERROR_SUCCESS){
            connect();
            mpd_run_toggle_pause(m_conn);
        }
        if (mpd_connection_get_error(m_conn) != MPD_ERROR_SUCCESS)
            throw MpdConnectionException(*(new std::string(mpd_connection_get_error_message(m_conn))));
    }
    mpd_response_finish(m_conn);
}

artistlist_t* MpdConnector::getArtists(){
    artistlist_t *artistList = new artistlist_t();

    if (m_conn == NULL){
        connect();
    }
    mpd_search_db_tags(m_conn, MPD_TAG_ALBUM_ARTIST_SORT);
    mpd_search_commit(m_conn);
    if (mpd_connection_get_error(m_conn) != MPD_ERROR_SUCCESS){
        connect();
        mpd_search_db_tags(m_conn, MPD_TAG_ALBUM_ARTIST_SORT);
        mpd_search_commit(m_conn);
    }
    if (mpd_connection_get_error(m_conn) != MPD_ERROR_SUCCESS)
        throw MpdConnectionException(*(new std::string(mpd_connection_get_error_message(m_conn))));
    struct mpd_pair *pair;
    while ((pair = mpd_recv_pair_tag(m_conn, MPD_TAG_ALBUM_ARTIST_SORT)) != NULL){
        MpdArtist *newArtist = new MpdArtist(pair->value);
        artistList->push_back(newArtist);
        mpd_return_pair(m_conn, pair);
    }

    return artistList;
}

albumlist_t* MpdConnector::getAlbums(std::string artistName){
    albumlist_t *albumList = new albumlist_t();

    if (m_conn == NULL){
        connect();
    }
    mpd_search_db_tags(m_conn, MPD_TAG_ALBUM_SORT);
    mpd_search_add_tag_constraint(m_conn, MPD_OPERATOR_DEFAULT, MPD_TAG_ALBUM_ARTIST, artistName.c_str());
    mpd_search_commit(m_conn);
    if (mpd_connection_get_error(m_conn) != MPD_ERROR_SUCCESS){
        connect();
        mpd_search_db_tags(m_conn, MPD_TAG_ALBUM_SORT);
        mpd_search_add_tag_constraint(m_conn, MPD_OPERATOR_DEFAULT, MPD_TAG_ALBUM_ARTIST, artistName.c_str());
        mpd_search_commit(m_conn);
    }
    if (mpd_connection_get_error(m_conn) != MPD_ERROR_SUCCESS)
        throw MpdConnectionException(*(new std::string(mpd_connection_get_error_message(m_conn))));

    struct mpd_pair *pair;
    while ((pair = mpd_recv_pair_tag(m_conn, MPD_TAG_ALBUM_SORT)) != NULL){
        MpdAlbum *newAlbum = new MpdAlbum(pair->value);
        albumList->push_back(newAlbum);
        mpd_return_pair(m_conn, pair);
    }

    return albumList;
}

MpdAlbum* MpdConnector::getAlbum(std::string artistName, std::string albumName){
    MpdAlbum *album = new MpdAlbum(albumName);

    if (m_conn == NULL){
        connect();
    }
    mpd_search_db_songs(m_conn, true);
    mpd_search_add_tag_constraint(m_conn, MPD_OPERATOR_DEFAULT, MPD_TAG_ALBUM_ARTIST, artistName.c_str());
    mpd_search_add_tag_constraint(m_conn, MPD_OPERATOR_DEFAULT, MPD_TAG_ALBUM_SORT, albumName.c_str());
    mpd_search_commit(m_conn);
    if (mpd_connection_get_error(m_conn) != MPD_ERROR_SUCCESS){
        connect();
        mpd_search_db_songs(m_conn, true);
        mpd_search_add_tag_constraint(m_conn, MPD_OPERATOR_DEFAULT, MPD_TAG_ALBUM_ARTIST, artistName.c_str());
        mpd_search_add_tag_constraint(m_conn, MPD_OPERATOR_DEFAULT, MPD_TAG_ALBUM_SORT, albumName.c_str());
        mpd_search_commit(m_conn);
    }
    if (mpd_connection_get_error(m_conn) != MPD_ERROR_SUCCESS)
        throw MpdConnectionException(*(new std::string(mpd_connection_get_error_message(m_conn))));

    struct mpd_song *song;
    while ((song = mpd_recv_song(m_conn)) != NULL){
        album->addSong(new MpdSong(song));
        if (album->getReleaseDate()=="" && mpd_song_get_tag(song, MPD_TAG_DATE,0)!="" && mpd_song_get_tag(song, MPD_TAG_DATE,0)!=NULL){
            album->setReleaseDate(std::string(mpd_song_get_tag(song, MPD_TAG_DATE,0)));
        }
    }

    return album;
}

std::string* MpdConnector::getAlbumArt(std::string url){
    std::string *imageData = new std::string;

    unsigned long long offset = 0;
    unsigned long long total_size;

    if (m_conn == NULL){
        connect();
    }
    do {
        char offset_s[32]="0";
        mpd_send_command(m_conn, "readpicture", url.c_str(), offset_s, NULL);
        if (mpd_connection_get_error(m_conn) != MPD_ERROR_SUCCESS){
            connect();
            mpd_send_command(m_conn, "readpicture", url.c_str(), offset_s, NULL);
        }
        if (mpd_connection_get_error(m_conn) != MPD_ERROR_SUCCESS){
            throw MpdConnectionException(*(new std::string(mpd_connection_get_error_message(m_conn))));
        }
        struct mpd_pair *pair = mpd_recv_pair_named(m_conn, "size");
        if (pair == NULL){
            if (mpd_connection_get_error(m_conn) != MPD_ERROR_SUCCESS){
                throw MpdConnectionException(*(new std::string(mpd_connection_get_error_message(m_conn))));
            }
        }
        mpd_return_pair(m_conn, pair);
        pair = mpd_recv_pair_named(m_conn, "binary");
        if (pair == NULL){
            if (mpd_connection_get_error(m_conn) != MPD_ERROR_SUCCESS){
                throw MpdConnectionException(*(new std::string(mpd_connection_get_error_message(m_conn))));
            }
        }
        const unsigned long long chunk_size = strtoull(pair->value, NULL, 10);
        mpd_return_pair(m_conn, pair);
        char *p = (char*)malloc(chunk_size);
        mpd_recv_binary(m_conn, p, chunk_size);
        
        mpd_response_finish(m_conn);
        imageData->append(p);
        free(p);
        offset += chunk_size;
    } while (offset<total_size);

    if (imageData->size()==0){
        throw MpdConnectionException(*(new std::string("No image found")));
    }

    return imageData;
}

void MpdConnector::skipToSong (std::string url){
    std::unique_ptr<songlist_t> currentPlaylist = std::unique_ptr<songlist_t>(this->getCurrentPlaylist());

    auto currentPlaylistIt = currentPlaylist->begin();
    int songPos=0;
    while (currentPlaylistIt != currentPlaylist->end()){
        if (url==(*currentPlaylistIt)->getName()){
            currentPlaylistIt=currentPlaylist->end();
            if (m_conn == NULL){
                connect();
            }
            mpd_run_play_pos(m_conn, songPos);
            if (mpd_connection_get_error(m_conn) != MPD_ERROR_SUCCESS){
                connect();
                mpd_run_play_pos(m_conn, songPos);
            }
            if (mpd_connection_get_error(m_conn) != MPD_ERROR_SUCCESS)
                throw MpdConnectionException(*(new std::string(mpd_connection_get_error_message(m_conn))));
        }
        songPos++;
        if (currentPlaylistIt!= currentPlaylist->end()){
            currentPlaylistIt++;
        }
    }
}

void MpdConnector::clearQueueAndPlay(std::string url){
    if (m_conn == NULL){
        connect();
    }
    mpd_run_clear(m_conn);
    if (mpd_connection_get_error(m_conn) != MPD_ERROR_SUCCESS){
        connect();
        mpd_run_clear(m_conn);
    }
    if (mpd_connection_get_error(m_conn) != MPD_ERROR_SUCCESS)
        throw MpdConnectionException(*(new std::string(mpd_connection_get_error_message(m_conn))));

    mpd_run_add(m_conn,url.c_str());
    if (mpd_connection_get_error(m_conn) != MPD_ERROR_SUCCESS){
        connect();
        mpd_run_add(m_conn,url.c_str());
    }
    if (mpd_connection_get_error(m_conn) != MPD_ERROR_SUCCESS){

        std::string errorMessage = std::string(mpd_connection_get_error_message(m_conn))+" - "+url;
        throw MpdConnectionException(*(new std::string(errorMessage)));
    }
    mpd_run_play(m_conn);
    if (mpd_connection_get_error(m_conn) != MPD_ERROR_SUCCESS){
        connect();
        mpd_run_play(m_conn);
    }
    if (mpd_connection_get_error(m_conn) != MPD_ERROR_SUCCESS)
        throw MpdConnectionException(*(new std::string(mpd_connection_get_error_message(m_conn))));
}