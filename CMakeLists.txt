cmake_minimum_required(VERSION 3.12)
project(bh-mapeditor)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

set(CUSTOMIZE_BUILD ON CACHE BOOL "" FORCE)
set(SUPPORT_FILEFORMAT_JPG ON CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
add_subdirectory(deps/raylib)

add_subdirectory(deps/imgui)

add_subdirectory(deps/rlImGui)

add_subdirectory(deps/tinyxml2)

add_subdirectory(deps/nativefiledialog-extended)

add_subdirectory(deps/miniz)

# set(link_src "${CMAKE_SOURCE_DIR}/assets")
# if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
# 	set(link_dst "${CMAKE_CURRENT_BINARY_DIR}/${CONFIG}/<assets") 
# else() 
# 	set(link_dst "${CMAKE_CURRENT_BINARY_DIR}/assets") 
# endif() 

# if (UNIX)
# 	add_custom_command( 
# 		TARGET ${PROJECT_NAME} 
# 		POST_BUILD COMMAND ${CMAKE_COMMAND} 
# 				-E create_symlink ${link_src} ${link_dst} 
# 		DEPENDS ${link_dst} 
# 		COMMENT "symbolic link ${link_src} => ${link_dst}" 
# 	)
# else()
# 	file(COPY ${link_src} DESTINATION ${link_dst}/..)
# 	message(STATUS "copying files ${link_src} => ${link_dst}") 
# endif()

if (APPLE)
	target_link_libraries(${PROJECT_NAME} "-framework IOKit")
	target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
	target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE src/editor src/swz deps/raylib/src deps/imgui deps/imgui/misc/cpp deps/rlImGui deps/tinyxml2 src/util src/editor/ui src/map/object src/map)
target_link_libraries(bh-mapeditor PRIVATE raylib imgui rlImGui nfd tinyxml2 miniz)

if (MSVC)
	target_compile_options(bh-mapeditor PRIVATE /W3 /std:c++20)
else()
	target_compile_options(bh-mapeditor PRIVATE -Wall -Wextra -std=c++20)
endif()
