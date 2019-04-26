/**
 * @author      : Sam Bell (sam@mimpidev.com)
 * @file        : MpdAlbum
 * @created     : Thursday Dec 24, 2020 23:17:14 AEDT
 */

#include "MpdAlbum.h"
#include "MpdSong.h"

MpdAlbum::MpdAlbum() {
    
}

MpdAlbum::MpdAlbum(std::string albumName){
    m_albumName = albumName;
}

void MpdAlbum::addSong(MpdSong* song){
    auto songlistIt = m_songList->begin();
    bool found=false;
    while (songlistIt!=m_songList->end()){
        if ((*songlistIt)->getFullPath()==song->getFullPath()){
            found=true;
        }
        songlistIt++;
    }
    if (!found){
        m_songList->push_back(song);
    }
}
