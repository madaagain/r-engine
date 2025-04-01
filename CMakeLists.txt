cmake_minimum_required(VERSION 3.15...3.22)

project(r-type VERSION 1.0
            DESCRIPTION "R-TYPE"
            LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(client-enet)
add_subdirectory(server-enet)