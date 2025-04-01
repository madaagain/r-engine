cmake_minimum_required(VERSION 3.15...3.22)

project(r-type_client VERSION 1.0
        DESCRIPTION "R-Type Client"
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

# Configure the include directories
include_directories(include
                    ../ecs/systems/headers
                    ../ecs/include
                    ../ecs/components
                    ../server-enet/include
                    ../client/include
                    ../renderer)

# Configure the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define client sources
set(SOURCES
    src/main.cpp
    src/NetworkClient.cpp
    src/ClientGameState.cpp
    src/Screens/Menu.cpp
    src/Screens/Options.cpp
    ../client/src/Game.cpp
    ../client/src/InitGame.cpp
)

# Create the client executable
add_executable(r-type_client ${SOURCES})

# Manage DLLs for Windows
if(WIN32)
    add_custom_command(TARGET r-type_client POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:sfml-system>
            $<TARGET_FILE:sfml-window>
            $<TARGET_FILE:sfml-graphics>
            $<TARGET_FILE:sfml-audio>
            $<TARGET_FILE_DIR:r-type_client>)
endif()

# Configure compiler options
if(MSVC)
    target_compile_options(r-type_client PRIVATE /W4 /WX)
else()
    target_compile_options(r-type_client PRIVATE -Werror -Wall -Wextra)
endif()

# Properly link SFML and ENet
target_include_directories(r-type_client PRIVATE ${ENET_SOURCE_DIR}/include)
target_link_libraries(r-type_client PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
    enet
)
