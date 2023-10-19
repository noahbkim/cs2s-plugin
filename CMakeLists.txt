cmake_minimum_required(VERSION 3.23)
set(CMAKE_CXX_STANDARD 17)

project(cs2s-plugin)

include(cmake/funchook.cmake)
include(cmake/cs2metamod.cmake)
include(cmake/cs2proto.cmake)

# https://stackoverflow.com/questions/9160335/os-specific-instructions-in-cmake-how-to
if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

# https://stackoverflow.com/questions/51154151/how-to-check-what-compiler-cmake-is-using
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_definitions(
            stricmp=strcasecmp
            _stricmp=strcasecmp
            _snprintf=snprintf
            _vsnprintf=vsnprintf
            _alloca=alloca
            GNUC
    )
    add_compile_options(
            -fPIC
            -fno-exceptions
            -fno-rtti
            -msse
            -fno-strict-aliasing
    )
else()
    message(WARNING "Unsupported platform!")
endif()

if(LINUX)
    add_compile_definitions(POSIX _LINUX COMPILER_GCC)
    add_link_options(-shared)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        add_link_options(-static-libgcc)
    endif()
else()
    message(WARNING "Unsupported platform!")
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    add_compile_definitions(PLATFORM_64BITS)
endif()

add_compile_definitions(META_IS_SOURCE2)

# Prevent prepending "lib"
set(CMAKE_SHARED_LIBRARY_PREFIX "")
add_library(cs2s-plugin SHARED
        src/plugin.cpp
)

add_dependencies(cs2s-plugin cs2metamod cs2proto)
target_include_directories(cs2s-plugin
        PRIVATE ${CMAKE_SOURCE_DIR}/vendor/hl2sdk/public
        PRIVATE ${CMAKE_SOURCE_DIR}/vendor/hl2sdk/public/engine
        PRIVATE ${CMAKE_SOURCE_DIR}/vendor/hl2sdk/public/mathlib
        PRIVATE ${CMAKE_SOURCE_DIR}/vendor/hl2sdk/public/vstdlib
        PRIVATE ${CMAKE_SOURCE_DIR}/vendor/hl2sdk/public/tier0
        PRIVATE ${CMAKE_SOURCE_DIR}/vendor/hl2sdk/public/tier1
        PRIVATE ${CMAKE_SOURCE_DIR}/vendor/metamod-source/core
        PRIVATE ${CMAKE_SOURCE_DIR}/vendor/metamod-source/core/sourcehook
)

target_link_libraries(cs2s-plugin cs2proto cs2interfaces cs2libtier0 cs2tier1)
target_include_directories(
        cs2s-plugin
        PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
        PRIVATE ${protobuf_INCLUDE_DIRS} ${absl_INCLUDE_DIRS}
)
