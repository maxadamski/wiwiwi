cmake_minimum_required(VERSION 3.14)
project(server)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(server
        game.hh
        main.cc
        util.hh)

target_link_libraries(server
        pthread)
