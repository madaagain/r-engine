cmake_minimum_required(VERSION 3.15...3.22)

project(client VERSION 1.0
            DESCRIPTION "Client RType"
            LANGUAGES CXX)

include_directories(include)
include(cmake/CPM.cmake)

CPMAddPackage(
    NAME asio
    VERSION 1.30.2
    GITHUB_REPOSITORY chriskohlhoff/asio
    GIT_TAG asio-1-30-2
)

CPMAddPackage(
    NAME SFML
    GITHUB_REPOSITORY SFML/SFML
    VERSION 2.6.1
    GIT_TAG 2.6.1
)

find_package(SFML 2.5 COMPONENTS system window graphics audio REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

set(SOURCES
    src/Main.cpp
    src/Client.cpp
    src/InitGame.cpp
    src/Screens/Menu.cpp
    src/Screens/Options.cpp
)

add_executable(r-type_client ${SOURCES})

if(MSVC)
    target_compile_options(r-type_client PRIVATE /W4 /WX)
else()
    target_compile_options(r-type_client PRIVATE -Werror -Wall -Wextra)
endif()

target_link_libraries(r-type_client PRIVATE
    sfml-system
    sfml-window
    sfml-graphics
    sfml-audio
)

include_directories(${asio_SOURCE_DIR}/asio/include)
target_include_directories(r-type_client PRIVATE ${asio_SOURCE_DIR}/asio/include)

if(WIN32)
    target_link_libraries(r-type_client PRIVATE ws2_32)
endif()

if(WIN32)
    add_custom_command(TARGET r-type_client POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:sfml-system>
            $<TARGET_FILE:sfml-window>
            $<TARGET_FILE:sfml-graphics>
            $<TARGET_FILE:sfml-audio>
            $<TARGET_FILE_DIR:r-type_client>)
endif()

if(MSVC)
    target_compile_options(r-type_client PRIVATE /W4 /WX)
else()
    target_compile_options(r-type_client PRIVATE -Werror -Wall -Wextra)
endif()

if(MSVC)
    message(STATUS "MSVC detected, adding compile flags")
    list(APPEND CLIENT_COMPILE_OPTIONS "/std:c++14")
    foreach(ITEM ${CLIENT_COMPILE_OPTIONS})
        message("    ---> CLIENT FLAGS  ${ITEM}")
        set_source_files_properties(${SOURCES} PROPERTIES COMPILE_FLAGS ${ITEM})
    endforeach()
endif()
