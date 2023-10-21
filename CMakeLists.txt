cmake_minimum_required(VERSION 3.23)
set(CMAKE_CXX_STANDARD 17)
project(cs2s-plugin)

# Unless you're adding features to this repository, you shouldn't need to edit
# anything in this file. Instead, use `plugin.cmake`, which extracts all of the
# configurable options for this template into one place..

# Include CMake configurations for components of the project.
include(cmake/platform.cmake)

# Create our plugin target; outputs a shared library.
add_library(plugin SHARED ${plugin_SOURCES} ${plugin_LIB_SOURCES})

# Prevent prepending "lib" and rename output
if(NOT DEFINED plugin_OUTPUT_DIR)
    set(plugin_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/package)
endif()

# Linker options deduced from AMBuild in CS2Fixes etc.
if(LINUX)
    target_link_options(plugin PRIVATE -static-libgcc)
    target_compile_definitions(plugin PRIVATE stricmp=strcasecmp _stricmp=strcasecmp _snprintf=snprintf _vsnprintf=vsnprintf _alloca=alloca)
    target_compile_options(plugin PRIVATE -fPIC -fno-exceptions -fno-rtti -msse -fno-strict-aliasing)
else()
    message(WARNING "plugin is not configured for this platform!")
endif()

# Include funchook (FetchContent)
include(cmake/funchook.cmake)
target_link_libraries(funchook-static)
target_include_directories(plugin PRIVATE ${funchook_INCLUDE_DIRS})

# Include hl2sdk (vendored)
include(cmake/hl2sdk.cmake)
target_link_libraries(plugin hl2sdk::interfaces hl2sdk::tier0 hl2sdk::tier1)

# Include metamod (vendored)
include(cmake/metamod.cmake)
target_link_libraries(plugin metamod)

# Also copy the metamod stuff into our package dir
add_custom_target(metamod_copy COMMAND
        ${CMAKE_COMMAND} -E
        copy_directory_if_different ${metamod_PACKAGE_DIR} ${plugin_OUTPUT_DIR}
)
add_dependencies(metamod_copy metamod_build)
add_dependencies(plugin metamod_copy)

# Include `plugin.cmake` after we create the target.
include(plugin.cmake)

# Ensure `plugin_NAME` is defined in `plugin.cmake` and define.
if(NOT DEFINED plugin_NAME)
    message(FATAL_ERROR "plugin_NAME must be defined!")
endif()
target_compile_definitions(plugin PRIVATE PLUGIN_NAME=${plugin_NAME})

# Rename the generated library
set_target_properties(plugin PROPERTIES
        PREFIX ""  # Remove lib prefix
        LIBRARY_OUTPUT_NAME plugin  # Always name the library file "plugin"
        LIBRARY_OUTPUT_DIRECTORY ${plugin_OUTPUT_DIR}/addons/${plugin_NAME}
)

# Rewrite the plugin.vdf file to build
set(plugin_OUTPUT_VDF_FILE ${plugin_OUTPUT_DIR}/addons/${plugin_NAME}.vdf)
configure_file(src/plugin.vdf ${plugin_OUTPUT_VDF_FILE})
