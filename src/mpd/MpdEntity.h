/**
 * @author      : Sam Bell (sam@mimpidev.com)
 * @file        : MpdEntity
 * @created     : Tuesday Jun 25, 2019 03:30:42 UTC
 */

#ifndef MPDENTITY_H

#define MPDENTITY_H
#include <mpd/entity.h>
#include <string>

class MpdEntity
{
    public:
        MpdEntity (){}
        MpdEntity (mpd_entity* entity) {}
        virtual ~MpdEntity ();
        unsigned getId() { return m_id; }
        std::string getIdString() { return std::to_string(m_id); }
        std::string getFullPath() { return m_pathName; }
        std::string getName();
        std::string getParentPath();
        static unsigned m_idStash;
        virtual std::string type() { return "MpdEntity"; }
    protected:
        std::string m_pathName;
        unsigned getNextId() { return m_idStash++; }
        unsigned m_id;
};

#endif /* end of include guard MPDENTITY_H */

