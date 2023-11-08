find_package(miniz REQUIRED)
find_package(tinyxml2 REQUIRED)
find_package(nfd REQUIRED)
find_package(raylib REQUIRED)
find_package(imgui REQUIRED)
find_package(rlImGui REQUIRED)

add_executable(bh-mapeditor
	"main.cpp"
	"util/imageresource.cpp"
	"util/logger.cpp"
	"util/selectimage.cpp"
	"editor/editor.cpp"
	"editor/ui/mapinfo.cpp"
	"editor/ui/objectlist.cpp"
	"editor/ui/objectproperties.cpp"
	"editor/ui/spawnpopup.cpp"
	"map/map.cpp"
	"map/serializer.cpp"
	"map/object/objecttypes.cpp"
	"swz/decrypt.cpp"
	"swz/encrypt.cpp"
	"swz/swz.cpp"
	"swz/well512.cpp"
)

target_include_directories(bh-mapeditor PRIVATE 
	${CMAKE_SOURCE_DIR}/editor
	${CMAKE_SOURCE_DIR}/editor/editor
	${CMAKE_SOURCE_DIR}/editor/editor/ui
	${CMAKE_SOURCE_DIR}/editor/swz/
	${CMAKE_SOURCE_DIR}/editor/map
	${CMAKE_SOURCE_DIR}/editor/map/object
	${CMAKE_SOURCE_DIR}/editor/util
	${CMAKE_SOURCE_DIR}/deps/imgui
)

target_link_libraries(bh-mapeditor PRIVATE
	raylib
	imgui 
	rlImGui 
	nfd 
	tinyxml2 
	miniz
)

if (MSVC)
	target_compile_options(bh-mapeditor PRIVATE /W3)
else()
	target_compile_options(bh-mapeditor PRIVATE -Wall -Wextra)
endif()
