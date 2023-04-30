cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
project(SDLIsing)

include(cmake/CPM.cmake)

CPMAddPackage("gh:fmtlib/fmt#9.1.0")
CPMAddPAckage("gh:libsdl-org/SDL#release-2.26.5")

add_executable(main main.cpp)
target_link_libraries(main fmt::fmt SDL2::SDL2)

