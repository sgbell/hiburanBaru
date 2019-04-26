/**
 * @author      : Sam Bell (sam@mimpidev.com)
 * @file        : MpdArtist
 * @created     : Wednesday Dec 23, 2020 15:46:21 AEDT
 */

#include "MpdArtist.h"
#include "MpdSong.h"
#include "MpdAlbum.h"

MpdArtist::MpdArtist(){
    
}

MpdArtist::MpdArtist(MpdSong *song){
    m_artistName = song->getArtist();
    if (m_artistName=="") {
        m_artistName="Unknown";
    }
}

MpdArtist::MpdArtist(std::string artistName){
    m_artistName = artistName;
}

MpdArtist::~MpdArtist(){
    
}
