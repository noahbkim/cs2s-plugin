cmake_minimum_required(VERSION 3.24)
set(CMAKE_CXX_STANDARD 20)  # You can change this to 17 if you don't need/have 20
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON CACHE INTERNAL "enable position-independent code globally")
project(cs2s-plugin)

# Platform detection polyfills.
include(cmake/platform.cmake)

# Declare our plugin CMake target. The name of the library is independent of
# the name you chose above. I'd recommend leaving it as `plugin` so it's
# convenient to refer to in the CMake files.
add_library(plugin SHARED)

# We structure out output directory similarly to metamod; there'll be a package
# directory containing a structure mirroring the game installation.
set(plugin_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/package)

# Begin configurable options. #################################################

# The name of the plugin. Automatically made available as a compiler definition
# by the second line, which emits `-DPLUGIN_NAME=${plugin_NAME}`.
set(plugin_NAME "cs2s-plugin")
target_compile_definitions(plugin PRIVATE PLUGIN_NAME=${plugin_NAME})

# By default, sources included in the `plugin` target are determined by
# recursively globbing for `*.cpp`. If this is insufficient, you can add your
# sources manually.
file(GLOB_RECURSE plugin_SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)
target_sources(plugin PRIVATE ${plugin_SOURCES})

# This is a simple rule to interpolate variables in src/plugin.vdf file and
# copy it into the build as package/addons/${plugin_NAME}.vdf. Any variables
# you define in this CMake scope will be available in the file.
set(plugin_OUTPUT_VDF_FILE ${plugin_OUTPUT_DIR}/addons/${plugin_NAME}.vdf)
configure_file(src/plugin.vdf ${plugin_OUTPUT_VDF_FILE})

# The CS2S SDK is a collection of utilities I've written that make common
# plugin operations more convenient. It's entirely optional, but I've included
# it by default to get new plugin developers off the ground.
include(vendor/cs2s-sdk/manifest.cmake)
target_sources(plugin PRIVATE ${cs2s_sdk_SOURCES})
target_include_directories(plugin PRIVATE ${cs2s_sdk_INCLUDE_DIR})

# In order to interact with serialized server data (user messages, etc.),
# you're gonna need protobuf. Specifically, you're going to need protobuf
# 3.21.8 compiled with `_GLIBCXX_USE_CXX11_ABI=0` if on gcc. This is already
# handled for you via our custom Conan recipe, instructions in the README.md.
# https://github.com/noahbkim/cs2s-protobuf
find_package(protobuf REQUIRED)
target_link_libraries(plugin protobuf::protobuf)

# You're also gonna need the proto definitions, which we've vendored from
# GameTracking-CS2. The following rule builds a static library out of the CS2
# protos and puts the headers on the include path. If you opt out of using
# protobuf, make sure you comment this out too.
# https://github.com/SteamDatabase/GameTracking-CS2/
include(cmake/protos.cmake)
target_link_libraries(plugin protos)

# Abseil is a set of useful data structures and abstractions maintained by
# Google. I've included it by default as an alternative to the STL containers
# but using it is optional. Provided via Conan (see conanfile.txt).
# https://github.com/abseil/abseil-cpp
# TODO: https://github.com/abseil/abseil-cpp/issues/1554
#find_package(absl REQUIRED)
#target_link_libraries(plugin abseil::abseil)
#target_include_directories(plugin PRIVATE ${absl_INCLUDE_DIRS})

# Whereami allows you to concisely determine the path of the dynamic library
# your code is running from. It takes the guesswork out of retrieving relative
# files (e.g. configs or assets).
# https://github.com/gpakosz/whereami
#include(cmake/whereami.cmake)
#target_link_libraries(plugin whereami)

# End configurable options ####################################################

# Target C++20. TODO: check if this is needed
target_compile_features(plugin PRIVATE cxx_std_20)

# Counter-Strike 2 compatibility. These are global definitions because they are
# required for interoperability with the server runtime.
# TODO: determine whether _ITERATOR_DEBUG_LEVEL changes the ABI
#add_compile_definitions(_ITERATOR_DEBUG_LEVEL=0)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_definitions(_GLIBCXX_USE_CXX11_ABI=0)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # TODO: determine whether this is necessary and why it doesn't play nice with _ITERATOR_DEBUG_LEVEL
    #add_compile_options(plugin PRIVATE /MT)
endif()

# Linker options deduced from AMBuild in CS2Fixes etc.
if(LINUX)
    target_link_options(plugin PRIVATE -static-libgcc)
    target_compile_definitions(plugin PRIVATE stricmp=strcasecmp _stricmp=strcasecmp _snprintf=snprintf _vsnprintf=vsnprintf _alloca=alloca)
    target_compile_options(plugin PRIVATE)
elseif(WIN32)
    target_compile_definitions(plugin PRIVATE NOMINMAX)
else()
    message(WARNING "plugin is not configured for this platform!")
endif()

# Include hl2sdk (vendored).
include(cmake/hl2sdk.cmake)
target_link_libraries(plugin hl2sdk::interfaces hl2sdk::tier0 hl2sdk::tier1 hl2sdk::steam)

# Include metamod (vendored).
include(cmake/metamod.cmake)
target_link_libraries(plugin metamod)

# For convenience, we'll copy the metamod artifacts into our package directory.
add_custom_target(metamod_copy COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${metamod_PACKAGE_DIR} ${plugin_OUTPUT_DIR})
add_dependencies(metamod_copy metamod_build)
add_dependencies(plugin metamod_copy)

# Rename the generated library to remove the `lib` prefix on unix and disable
# the wonky directory structure created by default on Windows.
set_target_properties(plugin PROPERTIES
        PREFIX ""  # Remove lib prefix
        LIBRARY_OUTPUT_NAME plugin  # Always name the library file "plugin"
        RUNTIME_OUTPUT_NAME plugin  # Always name the library file "plugin"
        LIBRARY_OUTPUT_DIRECTORY ${plugin_OUTPUT_DIR}/addons/${plugin_NAME}
        RUNTIME_OUTPUT_DIRECTORY ${plugin_OUTPUT_DIR}/addons/${plugin_NAME}
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${plugin_OUTPUT_DIR}/addons/${plugin_NAME}  # Windows
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${plugin_OUTPUT_DIR}/addons/${plugin_NAME}
)
