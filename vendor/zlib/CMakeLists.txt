# Taken from https://github.com/madler/zlib/issues/759

include(FetchContent)
FetchContent_Declare(
        zlib
        GIT_REPOSITORY https://github.com/madler/zlib
        GIT_TAG v1.3
        OVERRIDE_FIND_PACKAGE
)
FetchContent_MakeAvailable(zlib)

# Publish
add_library(ZLIB::ZLIB ALIAS zlib)
target_include_directories(zlib INTERFACE ${zlib_BINARY_DIR} ${zlib_SOURCE_DIR})

# 1. Set the variables that are usually set by FindZLIB.cmake.
# 2. Add the module that stubs out `find_package(ZLIB ...)` calls.
set(ZLIB_FOUND ON PARENT_SCOPE)
set(ZLIB_LIBRARY ZLIB::ZLIB PARENT_SCOPE)
set(ZLIB_LIBRARIES ZLIB::ZLIB PARENT_SCOPE)
set(ZLIB_INCLUDE_DIR ${zlib_SOURCE_DIR} ${zlib_BINARY_DIR} PARENT_SCOPE)
set(ZLIB_INCLUDE_DIRS ${zlib_SOURCE_DIR} ${zlib_BINARY_DIR} PARENT_SCOPE)
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_LIST_DIR}/CMake" PARENT_SCOPE)
