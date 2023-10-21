cmake_minimum_required(VERSION 3.23)
set(CMAKE_CXX_STANDARD 17)
project(cs2s-plugin)

# Include user definitions and validate
include(plugin.cmake)

if(NOT DEFINED plugin_NAME)
    message(FATAL_ERROR "plugin_NAME must be defined!")
endif()

# Include CMake configurations for components of the project.
include(cmake/platform.cmake)

# If the user doesn't define their own source files, glob src/ recursively.
if(NOT DEFINED plugin_SOURCES)
    file(GLOB_RECURSE plugin_SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)
endif()

# Also glob for everything in lib. Passing source files around seems like the
# best option for now; building a static library would require twice the work.
file(GLOB_RECURSE plugin_LIB_SOURCES ${CMAKE_SOURCE_DIR}/lib/*.cpp)

# Create our plugin target; outputs a shared library.
add_library(plugin SHARED ${plugin_SOURCES} ${plugin_LIB_SOURCES})
target_compile_definitions(plugin PRIVATE PLUGIN_NAME=${plugin_NAME})

if(LINUX)
    target_link_options(plugin PRIVATE -static-libgcc)
    target_compile_definitions(plugin PRIVATE stricmp=strcasecmp _stricmp=strcasecmp _snprintf=snprintf _vsnprintf=vsnprintf _alloca=alloca)
    target_compile_options(plugin PRIVATE -fPIC -fno-exceptions -fno-rtti -msse -fno-strict-aliasing)
else()
    message(WARNING "plugin is not configured for this platform!")
endif()

# Configure the lib stuff modularly, we'll extract this into a lib maybe later
target_include_directories(plugin PRIVATE ${CMAKE_SOURCE_DIR}/lib/)

# Include abseil (Conan)
find_package(absl REQUIRED)
target_link_libraries(plugin abseil::abseil)
target_include_directories(plugin PRIVATE ${absl_INCLUDE_DIRS})

# Include protobuf (Conan)
find_package(protobuf REQUIRED)
target_link_libraries(plugin protobuf::protobuf)
target_include_directories(plugin PRIVATE ${protobuf_INCLUDE_DIRS})

# Include funchook (FetchContent)
include(cmake/funchook.cmake)
target_link_libraries(funchook-static)
target_include_directories(plugin PRIVATE ${funchook_INCLUDE_DIRS})

# Include cs2proto (vendored)
include(cmake/proto.cmake)
target_link_libraries(plugin proto)
target_include_directories(plugin PRIVATE ${proto_INCLUDE_DIRS})

# Include hl2sdk (vendored)
include(cmake/hl2sdk.cmake)
target_link_libraries(plugin hl2sdk::interfaces hl2sdk::tier0 hl2sdk::tier1)

# Include metamod (vendored)
include(cmake/metamod.cmake)
target_link_libraries(plugin metamod)

# Prevent prepending "lib" and rename output
if(NOT DEFINED plugin_OUTPUT_DIR)
    set(plugin_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/package)
endif()

# Rename the generated library
set_target_properties(plugin PROPERTIES
        PREFIX ""  # Remove lib prefix
        LIBRARY_OUTPUT_NAME plugin  # Always name the library file "plugin"
        LIBRARY_OUTPUT_DIRECTORY ${plugin_OUTPUT_DIR}/addons/${plugin_NAME}
)

# Rewrite the plugin.vdf file to build
set(plugin_OUTPUT_VDF_FILE ${plugin_OUTPUT_DIR}/addons/${plugin_NAME}.vdf)
configure_file(src/plugin.vdf ${plugin_OUTPUT_VDF_FILE})

## Also copy the metamod stuff into our package dir while we're at it
add_custom_target(metamod_copy COMMAND
        ${CMAKE_COMMAND} -E
            copy_directory_if_different ${metamod_PACKAGE_DIR} ${plugin_OUTPUT_DIR}
)
add_dependencies(metamod_copy metamod_build)
add_dependencies(plugin metamod_copy)
