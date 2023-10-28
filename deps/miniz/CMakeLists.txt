cmake_minimum_required(VERSION 3.12)
project(miniz)

add_library(miniz STATIC miniz.c)
target_include_directories(miniz PUBLIC ${CMAKE_CURRENT_LIST_DIR})