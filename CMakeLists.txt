cmake_minimum_required(VERSION 3.0)
project(mazecast)

set (SOURCES
    src/main.cpp
    src/mazemap.cpp
    src/player.cpp
    src/renderer.cpp
    src/screen.cpp
)

set (CMAKE_CXX_FLAGS "-std=c++11 -Wall -pedantic")

add_executable(mazecast ${SOURCES})
target_link_libraries(mazecast  ncursesw)
