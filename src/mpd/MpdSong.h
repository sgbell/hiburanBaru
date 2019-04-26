/**
 * @author      : Sam Bell (sam@mimpidev.com)
 * @file        : MpdSong
 * @created     : Monday Jun 24, 2019 12:54:50 UTC
 */

#ifndef MPDSONG_H
#define MPDSONG_H
#include "MpdEntity.h"
#include "Typedef.h"
#include <string>

class MpdSong: public MpdEntity
{
    public:
        MpdSong ();
        MpdSong (mpd_entity *entity);
        MpdSong (mpd_song *song);
        std::string getTag(std::string tagName);
        std::string getAlbum() { return m_properties["album"]; }
        std::string getTitle() { return m_properties["title"]; }
	std::string getArtist() { return m_properties["artist"]; }
        virtual ~MpdSong ();
        std::string type() { return "MpdSong"; }
    private:
        /* private data */
        unsigned m_duration;
        stringmap_t m_properties;
        void processSongData(const mpd_song *songObject);
};

#endif /* end of include guard MPDSONG_H */

