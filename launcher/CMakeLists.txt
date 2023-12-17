find_package(tinyxml2 REQUIRED)

add_executable(bhlauncher
    "main.cpp"
)

target_link_libraries(bhlauncher PRIVATE
    tinyxml2
)

target_include_directories(bhlauncher PRIVATE
    ${CMAKE_SOURCE_DIR}/bhlauncher
)