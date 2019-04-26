/**
 * @author      : Sam Bell (sam@mimpidev.com)
 * @file        : MpdDirectory
 * @created     : Tuesday Jun 25, 2019 03:34:33 UTC
 */

#include "MpdDirectory.h"
#include <mpd/directory.h>
#include "MpdSong.h"
#include <QDebug>

MpdDirectory::MpdDirectory(){
}

MpdDirectory::MpdDirectory(mpd_entity* entity){
    const mpd_directory *dirObject = mpd_entity_get_directory(entity);
    m_pathName = mpd_directory_get_path(dirObject);
}

MpdDirectory::MpdDirectory(std::string fullPath) {
    m_pathName = fullPath;
}

MpdDirectory::~MpdDirectory()
{
}

std::string MpdDirectory::getDirectoryName(){
    std::string directoryName = m_pathName.substr(m_pathName.find_last_of("/")+1);
    return directoryName;
}

void MpdDirectory::addDirectory(MpdDirectory* subDirectory){
    m_subDirectoryList->push_back(subDirectory);
}

MpdEntity* MpdDirectory::findDirectory(std::string path){
    MpdEntity *directory = nullptr;
    if (path==getFullPath()){
        return this;
    }
    // qDebug() << "MpdDirectory->findDirectory Searching: " << path.c_str();
    std::string subPath = path.substr(getFullPath().length()+1);
    if (subPath.find_first_of("/")!=std::string::npos){
        subPath = subPath.substr(0,subPath.find_first_of("/")+1);
    }
    if (subPath.find_first_of("/")!=std::string::npos){
        subPath = subPath.substr(0,subPath.find_first_of("/"));
    }
    // qDebug() << "Sub Path: " << subPath.c_str();
    auto directoryIt = m_subDirectoryList->begin();
    bool found=false;
    while (directoryIt != m_subDirectoryList->end() && !found){
        //qDebug() << "MpdDirectory->findDirectory while: " << (*directoryIt)->getFullPath().c_str();
        
        if ((*directoryIt)->getName()==subPath){
            directory=(*directoryIt)->findDirectory(path);
            //qDebug() << "MpdDirectory->findDirectory found: " << path.c_str() << "=" << directory->getFullPath().c_str();
            found=true;
        }
        directoryIt++;
    }

    return directory;
}

void MpdDirectory::addSong(MpdSong *song){
    m_songList->push_back(song);
}

void MpdDirectory::addPlaylist(MpdPlaylist *playlist){
    m_playList->push_back(playlist);
}