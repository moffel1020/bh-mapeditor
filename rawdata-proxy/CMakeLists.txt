find_package(tinyxml2 REQUIRED)

add_library(rawdata-proxy SHARED
    "dllmain.cpp"
    "FlashRuntimeExtensions.h"
    "RawData.def"
)

target_link_libraries(rawdata-proxy PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/air.lib
)

target_include_directories(rawdata-proxy PRIVATE
    ${CMAKE_SOURCE_DIR}/rawdata-proxy
)

if (NOT MSVC)
    set(target_flag "-m32")
endif()

set_target_properties(rawdata-proxy PROPERTIES
    PREFIX ""
    OUTPUT_NAME "RawData-proxy"
    COMPILE_FLAGS "${target_flag}"
    LINK_FLAGS "${target_flag}"
)
