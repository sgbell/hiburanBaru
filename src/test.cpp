/**
 * @author      : Sam Bell (sam@mimpidev.com)
 * @file        : test
 * @created     : Friday Jul 05, 2019 03:15:42 UTC
 */

#include <iostream>
#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QSettings>
#include <memory>
#include <cstdlib>

#include "tpl_engine/Template.h"
#include "tpl_engine/TemplateMap.h"
#include "tpl_engine/TemplateList.h"
#include "tpl_engine/TemplateString.h"
#include "tpl_engine/TemplateParserException.h"

#include "mpd/MpdConnector.h"
#include "mpd/MusicCollection.h"
#include "mpd/MpdDirectory.h"
#include "mpd/MpdSong.h"

#include "debug/Debug.h"

bool Debug::m_debugOn=true;
Debug::LogLevel Debug::m_currentLevel=Debug::LOG_INFO;

void testTemplateEngine(){
    std::cout << "Testing templateEngine" << std::endl;
    Template *tpl_instance;
    try {
        tpl_instance = new Template("../src/resources/pages/library_tpl.html");
    } catch (TemplateParserException *ex) {
        debug_error(ex->message());
        exit(1);
    }
    TemplateMap* tpl = new TemplateMap();
    TemplateList* tplList = new TemplateList();
    tpl->put("entityType", tplList);
    TemplateMap* tplMap = new TemplateMap();
    tplMap->put("entityHeading", "Directory");
    tplMap->put("entityClass", "directory");
    TemplateList *directoryList = new TemplateList();
    tplMap->put("directory", directoryList);
    TemplateMap *dadDirectoryEntry = new TemplateMap();
    dadDirectoryEntry->put("entityClass", "directory");
    dadDirectoryEntry->put("fullName", "/mnt/sdc/dad");
    dadDirectoryEntry->put("name", "dad");
    TemplateList *subDirectoryList = new TemplateList();
    dadDirectoryEntry->put("directory", subDirectoryList);
    directoryList->push(*dadDirectoryEntry);
    TemplateMap *directoryEntry = new TemplateMap();
    directoryEntry->put("entityClass", "directory");
    directoryEntry->put("fullName", "/mnt/sdc/sam");
    directoryEntry->put("name", "sam");
    directoryList->push(*directoryEntry);
    directoryEntry = new TemplateMap();
    directoryEntry->put("entityClass", "song");
    directoryEntry->put("songName", "Dont go Breaking my heart");
    directoryList->push(*directoryEntry);
    directoryEntry = new TemplateMap();
    directoryEntry->put("entityClass", "directory");
    directoryEntry->put("fullName", "/mnt/sdc/dad/temp");
    directoryEntry->put("name", "temp");
    subDirectoryList->push(*directoryEntry);
    
    tplList->push(*tplMap);

    tplMap = new TemplateMap();
    tplMap->put("entityHeading", "Playlist");
    tplMap->put("entityClass", "playlist");
    tplList->push(*tplMap);

    try {
        std::cout << *(tpl_instance->compile(tpl)) << std::endl;
    } catch (TemplateParserException e){
        std::cerr << "Oops: " << e.message() << std::endl;
    }
}

TemplateMap* convertMap(MpdDirectory* directory){
    TemplateMap* returnMap = new TemplateMap();
    returnMap->put("entityClass", "directory");
    returnMap->put("fullName", directory->getFullPath().c_str());
    returnMap->put("name", directory->getName().c_str());
    returnMap->put("directory", new TemplateList());
    auto subDirectoryListIt = directory->getSubdirectories()->begin();
    while (subDirectoryListIt != directory->getSubdirectories()->end()){
        dynamic_cast<TemplateList*>(returnMap->get("directory"))->push(*convertMap((*subDirectoryListIt)));
        subDirectoryListIt++;
    }
    auto songListIt = directory->getSongs()->begin();
    while (songListIt != directory->getSongs()->end()){
        TemplateMap* songMap = new TemplateMap();
	songMap->put("id", (*songListIt)->getIdString().c_str());
	songMap->put("filename", (*songListIt)->getName().c_str());
	songMap->put("fullName", (*songListIt)->getFullPath().c_str());
	songMap->put("songName", (*songListIt)->getTitle().c_str());
	songMap->put("album", (*songListIt)->getAlbum().c_str());
	songMap->put("entityClass", "song");
        dynamic_cast<TemplateList*>(returnMap->get("directory"))->push(*songMap);
        songListIt++;
    }
    return returnMap;
}

void testFetch(){
    std::cout << "Testing convert maps to template" << std::endl;
    QFile *etcConf = new QFile("/etc/hiburan.conf");
    QSettings *settings = nullptr;
    if (etcConf->exists()){
        settings = new QSettings("/etc/hiburan.conf", QSettings::NativeFormat);
    } else {
        settings = new QSettings(std::string(std::getenv("HOME")).append("/.config/hiburan.conf").c_str(), QSettings::NativeFormat);
    }

    MpdConnector *mpdConnection = new MpdConnector(*settings);
    std::unique_ptr<MusicCollection> entityList = std::unique_ptr<MusicCollection>(mpdConnection->getList());

    Template *tpl_instance;
    try {
        tpl_instance = new Template("../src/resources/pages/library_tpl.html");
    } catch (TemplateParserException *ex) {
        debug_error(ex->message());
        exit(1);
    }
    TemplateMap* tplData = new TemplateMap();
    TemplateList* tplList = new TemplateList();
    tplData->put("entityType", tplList);
    
    stringmap_t entityTypes;
    entityTypes.insert({"playlist","Playlist"});
    entityTypes.insert({"directory","Directories"});
    auto entityTypeListIt = entityTypes.begin();
    while (entityTypeListIt!= entityTypes.end()){
        TemplateMap* tplMap = new TemplateMap();
        tplMap->put("entityHeading", entityTypeListIt->second);
        tplMap->put("entityClass", entityTypeListIt->first);
	tplMap->put(entityTypeListIt->first, new TemplateList());
        auto entityListIt = entityList->at(entityTypeListIt->first)->begin();
        while (entityListIt != entityList->at(entityTypeListIt->first)->end()){
            if ((entityTypeListIt->first)=="directory"){
	        dynamic_cast<TemplateList*>(tplMap->get(entityTypeListIt->first))->push(*convertMap((MpdDirectory*)(*entityListIt)));
            } else {
                dynamic_cast<TemplateList*>(tplMap->get(entityTypeListIt->first))->push(*(new TemplateString((*entityListIt)->getName())));
            }
	    entityListIt++;
        }
        entityTypeListIt++;
	tplList->push(*tplMap);
    }

    try {
        std::cout << *(tpl_instance->compile(tplData)) << std::endl;
    } catch (TemplateParserException e){
        std::cerr << "Oops: " << e.message() << std::endl;
    }
}

int main(int argc, char* argv[]) {

   for (int c=0; c<argc; c++){
       if (std::string("--template").compare(argv[c])==0){
           testTemplateEngine();
       }
       if (std::string("--fetch").compare(argv[c])==0){
           std::cout << "Test Fetch" << std::endl;
           testFetch();
       }
   }
   if (argc==1){
       testTemplateEngine();
       testFetch();
   }
}
