cmake_minimum_required (VERSION 2.8.12)
set (CMAKE_CXX_STANDARD 14)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(Qt5_DIR /opt/Qt/5.12.2/gcc_64/lib/cmake/Qt5)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/modules")

find_package(LibMPDClient REQUIRED)

find_package(Qt5 "5.12.2" CONFIG REQUIRED COMPONENTS
         Core
)

find_package(Boost REQUIRED COMPONENTS system filesystem)

set(mpd_SRC
      MpdConnectionException.cpp
      MpdConnectionException.h
      MpdConnector.cpp
      MpdConnector.h
      Typedef.h
      MpdDirectory.cpp
      MpdDirectory.h
      MpdPlaylist.cpp
      MpdPlaylist.h
      MpdSong.cpp
      MpdSong.h
      MpdEntity.h
      MusicCollection.h
      MpdEntity.cpp
      MpdArtist.cpp
      MpdArtist.h
      MpdAlbum.cpp
      MpdAlbum.h
      MusicCollection.cpp)

include_directories(${Boost_INCLUDE_DIRS})

add_library(MpdLibrary ${mpd_SRC})
target_link_libraries (MpdLibrary 
                         Qt5::Core
                         mpdclient
                         ${Boost_LIBRARIES})
