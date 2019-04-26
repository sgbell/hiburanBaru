/**
 * @author      : Sam Bell (sam@mimpidev.com)
 * @file        : MpdSong
 * @created     : Tuesday Jun 25, 2019 03:59:34 UTC
 */

#include "MpdSong.h"

#include <mpd/song.h>
#include <mpd/tag.h>
#include <QDebug>
#include <boost/filesystem/convenience.hpp>

MpdSong::MpdSong()
{
    
}

MpdSong::MpdSong(mpd_entity* entity)
{
    const mpd_song *songObject = mpd_entity_get_song(entity);
    processSongData(songObject);
}

MpdSong::MpdSong(mpd_song *song){
    processSongData(song);
}

void MpdSong::processSongData(const mpd_song *songObject){
    this->m_pathName = mpd_song_get_uri(songObject);
    this->m_duration = mpd_song_get_duration(songObject);
    // iterator over songObject->tags
    this->m_id = this->getNextId();
    if (mpd_song_get_tag(songObject, MPD_TAG_ARTIST, 0) != NULL){
        this->m_properties["artist"]=mpd_song_get_tag(songObject, MPD_TAG_ARTIST, 0);
    }
    if (mpd_song_get_tag(songObject, MPD_TAG_ALBUM, 0) != NULL){
        this->m_properties["album"]=mpd_song_get_tag(songObject, MPD_TAG_ALBUM, 0);
    }
    if (mpd_song_get_tag(songObject, MPD_TAG_ALBUM_ARTIST, 0) != NULL){
        this->m_properties["albumArtist"]=mpd_song_get_tag(songObject, MPD_TAG_ALBUM_ARTIST, 0);
        if (this->m_properties["artist"]==""){
            this->m_properties["artist"]=mpd_song_get_tag(songObject, MPD_TAG_ALBUM_ARTIST, 0);
        }
    }
    if (mpd_song_get_tag(songObject, MPD_TAG_TITLE, 0) != NULL){
        this->m_properties["title"]=mpd_song_get_tag(songObject, MPD_TAG_TITLE, 0);
    } else {
        this->m_properties["title"]=boost::filesystem::change_extension(this->getName(),"").string();
    }
    if (mpd_song_get_tag(songObject, MPD_TAG_NAME, 0) != NULL){
        this->m_properties["name"]=mpd_song_get_tag(songObject, MPD_TAG_NAME, 0);
    }
    if (mpd_song_get_tag(songObject, MPD_TAG_GENRE, 0) != NULL){
        this->m_properties["genre"]=mpd_song_get_tag(songObject, MPD_TAG_GENRE, 0);
    }
    if (mpd_song_get_tag(songObject, MPD_TAG_DATE, 0) != NULL){
        this->m_properties["date"]=mpd_song_get_tag(songObject, MPD_TAG_DATE, 0);
    }
    if (mpd_song_get_tag(songObject, MPD_TAG_COMMENT, 0) != NULL){
        this->m_properties["comment"]=mpd_song_get_tag(songObject, MPD_TAG_COMMENT, 0);
    }
}

MpdSong::~MpdSong()
{
}
