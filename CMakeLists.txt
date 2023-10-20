cmake_minimum_required(VERSION 3.23)
set(CMAKE_CXX_STANDARD 17)

# Change this to change the name of your plugin
project(cs2s-plugin)

# Don't change anything below this line unless you know what you're doing
set(cs2s_PACKAGE_DIR ${CMAKE_CURRENT_BINARY_DIR}/package)

include(cmake/funchook.cmake)
include(cmake/cs2metamod.cmake)
include(cmake/cs2proto.cmake)

# https://stackoverflow.com/questions/9160335/os-specific-instructions-in-cmake-how-to
if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

add_library(cs2s-plugin SHARED
        src/plugin.cpp
)

# Configure the lib stuff modularly, we'll extract this into a lib maybe later
target_include_directories(cs2s-plugin PRIVATE lib/)
target_sources(cs2s-plugin PRIVATE
        lib/cs2s/plugin/library.cpp
        lib/cs2s/plugin/shared.cpp
        lib/cs2s/plugin/detour.cpp
        lib/cs2s/sdk/server/recipientfilter.cpp
)

# Rewrite the plugin.vdf file to build
set(cs2s_PLUGIN_FILE ${cs2s_PACKAGE_DIR}/addons/${CMAKE_PROJECT_NAME}.vdf)
configure_file(src/plugin.vdf ${cs2s_PLUGIN_FILE})

# Prevent prepending "lib" and rename output
set(CMAKE_SHARED_LIBRARY_PREFIX "")
set_target_properties(cs2s-plugin PROPERTIES LIBRARY_OUTPUT_NAME cs2s-plugin)
set_target_properties(cs2s-plugin PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${cs2s_PACKAGE_DIR}/addons/${CMAKE_PROJECT_NAME})
target_compile_definitions(cs2s-plugin PRIVATE CMAKE_PROJECT_NAME=${CMAKE_PROJECT_NAME})

# https://stackoverflow.com/questions/51154151/how-to-check-what-compiler-cmake-is-using
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_definitions(cs2s-plugin PRIVATE
            COMPILER_GCC
            stricmp=strcasecmp
            _stricmp=strcasecmp
            _snprintf=snprintf
            _vsnprintf=vsnprintf
            _alloca=alloca
            GNUC
    )
    target_compile_options(cs2s-plugin PRIVATE
            -fPIC
            -fno-exceptions
            -fno-rtti
            -msse
            -fno-strict-aliasing
    )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_definitions(cs2s-plugin PRIVATE
            COMPILER_MSVC
    )
    message(WARNING "Unsupported platform!")
else()
    message(WARNING "Unsupported platform!")
endif()

if(LINUX)
    target_compile_definitions(cs2s-plugin PRIVATE POSIX _LINUX COMPILER_GCC)
    target_link_options(cs2s-plugin PRIVATE -shared)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_link_options(cs2s-plugin PRIVATE -static-libgcc)
    endif()
else()
    message(WARNING "Unsupported platform!")
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    target_compile_definitions(cs2s-plugin PRIVATE PLATFORM_64BITS)
endif()

add_compile_definitions(META_IS_SOURCE2)

add_dependencies(cs2s-plugin cs2metamod cs2proto)
target_include_directories(cs2s-plugin
        PRIVATE ${CMAKE_SOURCE_DIR}/vendor/  # Hail Mary for debugging path
        PRIVATE ${CMAKE_SOURCE_DIR}/vendor/hl2sdk/public
        PRIVATE ${CMAKE_SOURCE_DIR}/vendor/hl2sdk/public/engine
        PRIVATE ${CMAKE_SOURCE_DIR}/vendor/hl2sdk/public/mathlib
        PRIVATE ${CMAKE_SOURCE_DIR}/vendor/hl2sdk/public/vstdlib
        PRIVATE ${CMAKE_SOURCE_DIR}/vendor/hl2sdk/public/tier0
        PRIVATE ${CMAKE_SOURCE_DIR}/vendor/hl2sdk/public/tier1
        PRIVATE ${CMAKE_SOURCE_DIR}/vendor/metamod-source/core
        PRIVATE ${CMAKE_SOURCE_DIR}/vendor/metamod-source/core/sourcehook
)

find_package(protobuf REQUIRED)
find_package(absl REQUIRED)
target_link_libraries(cs2s-plugin protobuf::protobuf abseil::abseil)
target_link_libraries(cs2s-plugin cs2proto cs2interfaces cs2libtier0 cs2tier1)
target_include_directories(cs2s-plugin
        PRIVATE ${cs2proto_INCLUDE}
        PRIVATE ${funchook_INCLUDE_DIRS}
        PRIVATE ${absl_INCLUDE_DIRS}
        PRIVATE ${protobuf_INCLUDE_DIRS}
)
