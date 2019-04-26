/**
 * @author      : Sam Bell (sam@mimpidev.com)
 * @file        : MpdAlbum
 * @created     : Thursday Dec 24, 2020 22:53:45 AEDT
 */

#ifndef MPDALBUM_H
#define MPDALBUM_H
#include "MpdEntity.h"
#include "Typedef.h"

class MpdAlbum : public MpdEntity {
    public:
        MpdAlbum ();
	MpdAlbum (std::string albumName);
        void addSong(MpdSong* song);
	std::shared_ptr<songlist_t> getSongs() { return m_songList; }
        std::string type() { return "MpdAlbum"; }
        std::string getName() { if (m_albumName!="") { 
                                   return m_albumName; 
                                } else { 
                                   return "Unknown"; 
                                } 
                              }
        void setAlbumName(std::string albumName) { m_albumName = albumName; }
        void setReleaseDate(std::string releaseDate) { m_releaseDate = releaseDate; }
        std::string getReleaseDate() { return m_releaseDate; }
    private:
        /* private data */
	std::shared_ptr<songlist_t> m_songList = std::make_shared<songlist_t>();
	std::string m_releaseDate;
	std::string m_albumName;
};

#endif /* end of include guard MPDALBUM_H */

