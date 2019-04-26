cmake_minimum_required (VERSION 2.8.12)
set (CMAKE_CXX_STANDARD 14)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.

file(GLOB Debug_SRC
    "*.h"
    "*.cpp"
)

add_library(Debug ${Debug_SRC})
