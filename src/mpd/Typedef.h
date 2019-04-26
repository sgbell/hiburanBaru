#ifndef _TYPE_DEF_H
#define _TYPE_DEF_H
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mpd/entity.h>

class MpdDirectory;
class MpdSong;
class MpdPlaylist;
class MpdEntity;
class MpdArtist;
class MpdAlbum;

typedef std::map<std::string, std::string> stringmap_t;
typedef std::vector<std::string> stringlist_t;
typedef std::unique_ptr<stringmap_t> stringmap_ptr;
typedef std::unique_ptr<stringlist_t> stringlist_ptr;
typedef std::vector<mpd_entity*> entitylist_t;
typedef std::unique_ptr<entitylist_t> entitylist_ptr;
typedef std::map<std::string, entitylist_t*> entitymap_t;
typedef std::unique_ptr<entitymap_t> entitymap_ptr;
typedef std::vector<MpdDirectory*> directorylist_t;
typedef std::vector<MpdSong*> songlist_t;
typedef std::vector<MpdPlaylist*> playlist_t;
typedef std::vector<MpdEntity*> mpdentity_t;
typedef std::vector<MpdArtist*> artistlist_t;
typedef std::vector<MpdAlbum*> albumlist_t;
typedef std::map<std::string, MpdEntity*> mpdentitymap_t;

#endif //_TYPE_DEF_H
