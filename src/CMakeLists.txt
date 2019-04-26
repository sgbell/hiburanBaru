cmake_minimum_required (VERSION 2.8.12)
project (hiburanBaru)
set (CMAKE_CXX_STANDARD 14)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

set(Qt5_DIR /opt/Qt/5.12.5/gcc_64/lib/cmake/Qt5)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

message(STATUS "CMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}")

find_package(Qt5 "5.12.5" CONFIG REQUIRED COMPONENTS
         Core
         Widgets
         WebEngineWidgets
         QuickControls2
)

set(ui_SRCS
       MainWindow.cpp
       MainWindow.h
       WebUiHandler.h
       WebUiHandler.cpp)

set(RESOURCES
       basetemplate.qrc)


QT5_ADD_RESOURCES(RES_SOURCES ${RESOURCES})

add_subdirectory(mpd)
add_subdirectory(tpl_engine)
add_subdirectory(debug)

add_executable (hiburanBaru main.cpp ${ui_SRCS} ${RES_SOURCES})

target_link_libraries (hiburanBaru
                          Qt5::Widgets
                          Qt5::WebEngineWidgets
                          Qt5::QuickControls2
			  TplEngine
                          MpdLibrary)

add_executable (test test.cpp)

target_link_libraries (test
                          Qt5::Widgets
                          MpdLibrary
                          TplEngine
			  Debug)
