cmake_minimum_required(VERSION 3.16)
project(BlockchainProject)

# Specify C++ standard
set(CMAKE_CXX_STANDARD	20)
set(CMAKE_CXX_STANDARD_REQUIRED	ON)

add_subdirectory(src/data_structures)
add_subdirectory(src)
add_subdirectory(tests)

