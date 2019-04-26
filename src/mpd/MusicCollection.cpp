/**
 * @author      : Sam Bell (sam@mimpidev.com)
 * @file        : MusicCollection
 * @created     : Tuesday Jun 25, 2019 13:34:30 UTC
 */

#include "MusicCollection.h"
#include "MpdDirectory.h"
#include "MpdPlaylist.h"
#include "MpdSong.h"
#include "MpdArtist.h"
#include <exception>
#include <QDebug>
#include <QString>
#include <boost/algorithm/string.hpp>

MusicCollection::MusicCollection() {
    
}

void MusicCollection::addDirectory(mpd_entity* entity){
    MpdDirectory *dir = new MpdDirectory(entity);
    //qDebug() << "Dir Name: " << dir->getName().c_str();
    //qDebug() << "Path: " << dir->getParentPath().c_str();
    if (dir->getParentPath()==""){
        bool directoryFound= false;
        auto directoryIt = m_directoryList->begin();
        while (directoryIt != m_directoryList->end()){
            if ((*directoryIt)->getFullPath()==dir->getFullPath()){
                directoryFound = true;
            }
            directoryIt++;
        }
        if (!directoryFound){
            m_directoryList->push_back(dir);
            (*m_entityMap)[dir->getIdString()]=dir;
            dir->setParentDirectoryId("0");
        }
    } else {
        // Add directory to it's parent directory
        MpdDirectory *parentDirectory = (MpdDirectory*)findDirectory(dir->getParentPath());
        //qDebug() << "Parent Path Found: " << parentDirectory->getFullPath().c_str();
        if (parentDirectory!=nullptr){
            parentDirectory->getSubdirectories()->push_back(dir);
            (*m_entityMap)[dir->getIdString()]=dir;
            dir->setParentDirectoryId(parentDirectory->getIdString());
        }
    }
}

MpdEntity* MusicCollection::findDirectory(std::string path){
    //qDebug() << "Path Searching for: " << path.c_str();
    std::string primaryDir = path.substr(0,path.find_first_of("/"));
    MpdEntity *directory = nullptr;
    auto directoryIt = m_directoryList->begin();
    bool found=false;
    while (directoryIt != m_directoryList->end() && !found){
        if (primaryDir==(*directoryIt)->getName()){
            directory=((MpdDirectory*)*directoryIt)->findDirectory(path);
            if (directory==nullptr){
                directory=(*directoryIt);
            }
            found=true;
            //qDebug() << "Path Found: " << directory->getFullPath().c_str();
        }
        directoryIt++;
    }

    return directory;
}

void MusicCollection::addPlaylist(mpd_entity* entity){
   MpdPlaylist *playlist = new MpdPlaylist(entity);
   m_playList->push_back(playlist);
   (*m_entityMap)[playlist->getIdString()]=playlist;
}

void MusicCollection::addSong(mpd_entity* entity){
   // Add songs to a list in the directory
   MpdSong *song = new MpdSong(entity);
   MpdDirectory *directory = (MpdDirectory*)findDirectory(song->getParentPath());
   // qDebug() << "Directory" << directory->getFullPath().c_str() << " -> " << song->getName().c_str();
   
   if (directory==nullptr){
       directory = new MpdDirectory(song->getParentPath());
       MpdDirectory *parentDirectory = (MpdDirectory*)findDirectory(directory->getParentPath());
       parentDirectory->getSubdirectories()->push_back(directory);
   }
   if (directory!=nullptr){
       directory->addSong(song);
   }
   //TODO:Need to add code here to guess the artist from the filename, and maybe look at updating the id3 tag.
   std::string artistName = song->getArtist();
   if (artistName.size()==0){
       artistName="Undefined";
   }
   MpdArtist *artist = (MpdArtist*)findArtist(artistName);
   if (artist==nullptr){
       artist = new MpdArtist(song);
       m_artistList->push_back(artist);
   }
   (*m_entityMap)[song->getIdString()]=song;
}

std::shared_ptr<mpdentity_t> MusicCollection::at(std::string typeName){
   if (typeName=="directory"){
       return m_directoryList;
   } else if (typeName=="playlist"){
       return m_playList;
   } else {
       throw new std::exception();
   }
}

MusicCollection::~MusicCollection() {
}

void MusicCollection::copy(MusicCollection *collection){
    m_directoryList = collection->getDirectoryList();
    m_playList = collection->getPlayList();
    m_artistList = collection->getArtistList();
    m_entityMap = collection->getEntityList();
}

MpdEntity* MusicCollection::findArtist(std::string artistName){
    auto artistIt = m_artistList->begin();
    while (artistIt!=m_artistList->end()){
       if ((*artistIt)->getName()==artistName){
           return *artistIt;
       }
       artistIt++;
    }
    return nullptr;
}
