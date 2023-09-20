cmake_minimum_required(VERSION 3.12)
project(imgui)

add_library(imgui STATIC
    imgui.cpp
    imgui_demo.cpp
    imgui_draw.cpp
    imgui_tables.cpp
    imgui_widgets.cpp
    misc/cpp/imgui_stdlib.cpp
)

target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_LIST_DIR})