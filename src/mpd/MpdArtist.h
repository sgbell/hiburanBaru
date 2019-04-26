/**
 * @author      : Sam Bell (sam@mimpidev.com)
 * @file        : MpdArtist
 * @created     : Wednesday Dec 23, 2020 15:46:14 AEDT
 */

#ifndef MPDARTIST_H
#define MPDARTIST_H
#include "MpdEntity.h"
#include "Typedef.h"
#include <memory>

class MpdSong;

class MpdArtist : public MpdEntity
{
    public:
        MpdArtist ();
		MpdArtist (MpdSong *song);
		MpdArtist (std::string artistName);
		virtual ~MpdArtist ();
		std::string getName() { return m_artistName; }
	    std::string type() { return "MpdArtist"; }
    private:
		std::string m_artistName;
};

#endif /* end of include guard MPDARTIST_H */

