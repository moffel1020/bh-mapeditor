cmake_minimum_required(VERSION 3.12)
project(rlImGui)

add_library(rlImGui STATIC rlImGui.cpp)
target_include_directories(rlImGui PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_compile_definitions(rlImGui PUBLIC NO_FONT_AWESOME)
target_link_libraries(rlImGui PUBLIC imgui raylib)