cmake_minimum_required(VERSION 3.15...3.22)

project(server VERSION 1.0
                DESCRIPTION "Server RType"
                LANGUAGES CXX)

include_directories(include)
include(cmake/CPM.cmake)

CPMAddPackage(
    NAME asio
    VERSION 1.30.2
    GITHUB_REPOSITORY chriskohlhoff/asio
    GIT_TAG asio-1-30-2
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

set(SOURCES
        src/Main.cpp
        src/Server.cpp
        src/NetworkManager.cpp
)

add_executable(r-type_server ${SOURCES})

if(MSVC)
    target_compile_options(r-type_server PRIVATE /W4 /WX)
else()
    target_compile_options(r-type_server PRIVATE -Werror -Wall -Wextra)
endif()

target_include_directories(r-type_server PRIVATE ${asio_SOURCE_DIR}/asio/include ../ecs/include ../ecs/components)

if(WIN32)
    target_link_libraries(r-type_server PRIVATE ws2_32)
endif()

if(MSVC)
    message(STATUS "MSVC detected, adding compile flags")
    list(APPEND SERVER_COMPILE_OPTIONS "/std:c++14" "-D_WIN32_WINNT=0x0A00")
    foreach(ITEM ${SERVER_COMPILE_OPTIONS})
        message("    ---> SERVER FLAGS  ${ITEM}")
        set_source_files_properties(${SOURCES} PROPERTIES COMPILE_FLAGS ${ITEM})
    endforeach()
endif()
