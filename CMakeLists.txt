cmake_minimum_required(VERSION 3.12)
project(mapeditor)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_VS_PLATFORM_TOOLSET v143)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE "Debug")
endif()


list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

add_subdirectory(editor)
add_subdirectory(rawdata-proxy)
add_subdirectory(launcher)