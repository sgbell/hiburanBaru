/**
 * @author      : Sam Bell (sam@mimpidev.com)
 * @file        : MpdPlaylist
 * @created     : Tuesday Jun 25, 2019 03:39:22 UTC
 */

#include "MpdPlaylist.h"
#include <mpd/directory.h>
#include <mpd/playlist.h>

MpdPlaylist::MpdPlaylist()
{
    
}

MpdPlaylist::MpdPlaylist(mpd_entity* entity)
{
    const mpd_playlist *playlistObject = mpd_entity_get_playlist(entity);
    m_pathName = mpd_playlist_get_path(playlistObject);
}

MpdPlaylist::MpdPlaylist(mpd_playlist* playlist)
{
    m_pathName = mpd_playlist_get_path(playlist);
}

MpdPlaylist::~MpdPlaylist()
{
}

