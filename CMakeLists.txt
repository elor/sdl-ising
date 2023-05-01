cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
project(SDLIsing)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(cmake/CPM.cmake)

cpmaddpackage("gh:fmtlib/fmt#9.1.0")
cpmaddpackage("gh:libsdl-org/SDL#release-2.26.5")

add_executable(main main.cpp)
target_link_libraries(main fmt::fmt SDL2::SDL2)
