/**
 * @author      : Sam Bell (sam@mimpidev.com)
 * @file        : MpdDirectory
 * @created     : Monday Jun 24, 2019 12:54:44 UTC
 */

#ifndef MPDDIRECTORY_H
#define MPDDIRECTORY_H
#include "MpdEntity.h"
#include <memory>
#include "Typedef.h"

class MpdDirectory : public MpdEntity
{
    public:
        MpdDirectory ();
        MpdDirectory (mpd_entity* entity);
        MpdDirectory (std::string fullPath);
        virtual ~MpdDirectory ();
        std::string getDirectoryName();
        std::shared_ptr<directorylist_t> getSubdirectories(){ return m_subDirectoryList; }
        MpdEntity* findDirectory(std::string path);
        void addSong(MpdSong* song);
        std::shared_ptr<songlist_t> getSongs(){ return m_songList; }
        std::string type() { return "MpdDirectory"; }
        void setParentDirectoryId(std::string parentDirId) { if (m_parentDirectoryId=="") m_parentDirectoryId=parentDirId; }
        std::string getParentDirectoryId() { return m_parentDirectoryId; }
        void addDirectory(MpdDirectory* subDirectory);
        void addPlaylist(MpdPlaylist* playlist);
    private:
        std::shared_ptr<directorylist_t> m_subDirectoryList = std::shared_ptr<directorylist_t>(new directorylist_t);
        std::shared_ptr<songlist_t> m_songList = std::shared_ptr<songlist_t>(new songlist_t);
        std::shared_ptr<playlist_t> m_playList = std::make_shared<playlist_t>();
        std::string m_parentDirectoryId="";
};

#endif /* end of include guard MPDDIRECTORY_H */

