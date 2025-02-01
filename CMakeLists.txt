cmake_minimum_required(VERSION 3.16)
project(BlockchainProject)

# Where the executables will be placed
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/lib")

# Specify C++ standard
set(CMAKE_CXX_STANDARD	20)
set(CMAKE_CXX_STANDARD_REQUIRED	ON)

add_subdirectory(src)
add_subdirectory(tests)

