cmake_minimum_required(VERSION 3.15...3.22)

project(ecs VERSION 1.0
                DESCRIPTION "ECS lib for RType"
                LANGUAGES CXX
)

include(cmake/CPM.cmake)

if(UNIX)
  set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
  set(BUILD_GTEST ON CACHE BOOL "" FORCE)
  set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

  CPMAddPackage(
    NAME googletest
    GITHUB_REPOSITORY google/googletest
    GIT_TAG release-1.12.1
    VERSION 1.12.1
    OPTIONS "INSTALL_GTEST OFF" "gtest_force_shared_crt"
  )

  CPMAddPackage(
    NAME UDev
    GITHUB_REPOSITORY systemd/systemd
    GIT_TAG v249
  )
endif()

CPMAddPackage(
  NAME SFML
  GITHUB_REPOSITORY SFML/SFML
  GIT_TAG 2.5.1
)

if(UNIX) 
  enable_testing()

  set(TEST_SOURCES
      tests/SparseArrayTest.cpp
      tests/RegistryTests.cpp
  )

  add_executable(ecs_tests ${TEST_SOURCES})
  target_link_libraries(ecs_tests PRIVATE GTest::gtest GTest::gtest_main)
  target_include_directories(ecs_tests PRIVATE include)
  include(GoogleTest)
  gtest_discover_tests(ecs_tests)
endif()

include_directories(include systems/headers components ../client-enet/include ../server-enet/include ../renderer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    src/Main.cpp
)

add_executable(ecs ${SOURCES})

if(WIN32)
    add_custom_command(TARGET ecs POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:sfml-system>
            $<TARGET_FILE:sfml-window>
            $<TARGET_FILE:sfml-graphics>
            $<TARGET_FILE:sfml-audio>
            $<TARGET_FILE_DIR:ecs>)
endif()

if(MSVC)
    target_compile_options(ecs PRIVATE /W4 /WX)
else()
    target_compile_options(ecs PRIVATE -Werror -Wall -Wextra)
endif()

target_link_libraries(ecs PRIVATE sfml-graphics sfml-audio)
