cmake_minimum_required(VERSION 3.14)
project(CGIS)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

include_directories(include)
add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(experiments)