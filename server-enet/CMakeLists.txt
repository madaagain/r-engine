cmake_minimum_required(VERSION 3.15...3.22)

project(r-type_server VERSION 1.0
        DESCRIPTION "R-Type Server"
        LANGUAGES CXX)

include(cmake/CPM.cmake)

# Add SFML via CPM
CPMAddPackage(
    NAME SFML
    GITHUB_REPOSITORY SFML/SFML 
    GIT_TAG 2.5.1
)

# Add ENet via CPM
CPMAddPackage(
    NAME ENET
    GITHUB_REPOSITORY lsalzman/enet
    GIT_TAG v1.3.17
)

CPMAddPackage(
    NAME pkgconf
    GITHUB_REPOSITORY pkgconf/pkgconf
    GIT_TAG pkgconf-2.3.0
)

include_directories(include ../ecs/include ../ecs/components ../client-enet/include)

if(APPLE)
    link_directories(/usr/local/Cellar/enet/1.3.18/lib)
    include_directories(/usr/local/Cellar/enet/1.3.18/include)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

set(SOURCES
    src/NetworkServer.cpp
    src/ServerGameState.cpp
    src/main.cpp
)

add_executable(r-type_server ${SOURCES})

if(WIN32)
    add_custom_command(TARGET r-type_server POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:sfml-system>
            $<TARGET_FILE:sfml-window>
            $<TARGET_FILE:sfml-graphics>
            $<TARGET_FILE:sfml-audio>
            $<TARGET_FILE_DIR:r-type_server>)
endif()

if(MSVC)
    target_compile_options(r-type_server PRIVATE /W4 /WX)
else()
    target_compile_options(r-type_server PRIVATE -Werror -Wall -Wextra)
endif()

# Link SFML and ENet
target_include_directories(r-type_server PRIVATE ${ENET_SOURCE_DIR}/include)
target_link_libraries(r-type_server PRIVATE 
    sfml-graphics 
    sfml-window 
    sfml-system
    sfml-audio
    enet
)
