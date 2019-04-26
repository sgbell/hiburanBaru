/**
 * @author      : Sam Bell (sam@mimpidev.com)
 * @file        : MpdPlaylist
 * @created     : Monday Jun 24, 2019 12:54:58 UTC
 */

#ifndef MPDPLAYLIST_H

#define MPDPLAYLIST_H
#include "MpdEntity.h"

class MpdPlaylist : public MpdEntity
{
    public:
        MpdPlaylist ();
        MpdPlaylist (mpd_entity* entity);
        MpdPlaylist (mpd_playlist* playlist);
        virtual ~MpdPlaylist ();
        std::string type() { return "MpdPlaylist"; }
    private:
        std::string m_path;
};

#endif /* end of include guard MPDPLAYLIST_H */

