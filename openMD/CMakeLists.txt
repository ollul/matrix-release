cmake_minimum_required(VERSION 3.5)

project(Matrix LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenMP)

if(NOT OpenMP_CXX_FOUND)
    message(FATAL_ERROR "OpenMP not found")
endif()

set(OMP_DYNAMIC false)

add_executable(Matrix main.cpp)

target_link_libraries(Matrix OpenMP::OpenMP_CXX)
