# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(truck_stereo)
target_add_binary_data(${CMAKE_PROJECT_NAME}.elf "main/html/index.html" TEXT)
