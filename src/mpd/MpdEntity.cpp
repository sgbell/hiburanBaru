/**
 * @author      : Sam Bell (sam@mimpidev.com)
 * @file        : MpdEntity
 * @created     : Wednesday Jun 26, 2019 03:59:41 UTC
 */

#include "MpdEntity.h"

std::string MpdEntity::getName(){
   std::string directoryName = m_pathName.substr(m_pathName.find_last_of("/")+1);
   return directoryName;
}

std::string MpdEntity::getParentPath(){
   std::string parentPath = m_pathName.substr(0,m_pathName.find_last_of("/"));
   if (parentPath==m_pathName){
      parentPath="";
   }
   return parentPath;
}

MpdEntity::~MpdEntity()
{
}

unsigned MpdEntity::m_idStash = 10000;
