/**
 * @author      : Sam Bell (sam@mimpidev.com)
 * @file        : MusicCollection
 * @created     : Monday Jun 24, 2019 12:44:32 UTC
 */

#ifndef MUSICCOLLECTION_H

#define MUSICCOLLECTION_H
#include <mpd/entity.h>
#include <memory>
#include "Typedef.h"

class MusicCollection
{
    public:
        MusicCollection ();
        virtual ~MusicCollection ();
        void addDirectory(mpd_entity* entity);
        void addSong(mpd_entity* entity);
        void addPlaylist(mpd_entity* entity);
        std::shared_ptr<mpdentity_t> getDirectoryList() { return m_directoryList; }
        std::shared_ptr<mpdentity_t> getPlayList() { return m_playList; }
        std::shared_ptr<mpdentity_t> getArtistList() { return m_artistList; }
        std::shared_ptr<mpdentitymap_t> getEntityList() { return m_entityMap; }
        std::shared_ptr<mpdentity_t> at(std::string typeName);
        MpdEntity* findDirectory(std::string directory);
        MpdEntity* findArtist(std::string artistName);
        void copy(MusicCollection *collection);
    private:
        std::shared_ptr<mpdentity_t> m_directoryList = std::make_shared<mpdentity_t>();
        std::shared_ptr<mpdentity_t> m_playList = std::make_shared<mpdentity_t>();
        std::shared_ptr<mpdentity_t> m_artistList = std::make_shared<mpdentity_t>();
        std::shared_ptr<mpdentitymap_t> m_entityMap = std::make_shared<mpdentitymap_t>();
};

#endif /* end of include guard MUSICCOLLECTION_H */

