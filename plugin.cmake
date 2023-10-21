# The name of the plugin. Automatically made available as a compiler definition
# in `CMakeLists.txt` via PLUGIN_NAME=${plugin_NAME}.
set(plugin_NAME "cs2s-plugin")

# By default, sources included in the `plugin` target are determined by
# recursively globbing for `*.cpp`. If this is insufficient, you can add your
# sources manually.
file(GLOB_RECURSE plugin_SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)
target_sources(plugin PRIVATE ${plugin_SOURCES})

# This includes the sources in `lib/`, which provide useful interfaces for
# developing plugins. Can be safely omitted if you're not using them.
file(GLOB_RECURSE plugin_LIB_SOURCES ${CMAKE_SOURCE_DIR}/lib/*.cpp)
target_sources(plugin PRIVATE ${plugin_LIB_SOURCES})
target_include_directories(plugin PRIVATE ${CMAKE_SOURCE_DIR}/lib/)

# Abseil is a set of useful data structures and abstractions maintained by
# Google. I've included it by default as an alternative to the STL containers
# but you should be able to remove it. Provided via Conan.
find_package(absl REQUIRED)
target_link_libraries(plugin abseil::abseil)
target_include_directories(plugin PRIVATE ${absl_INCLUDE_DIRS})

# The following links against the protobuf library. If you're not using
# protobuf and you don't need the vendored CS2 protos, you can omit this.
# Provided via Conan.
find_package(protobuf REQUIRED)
target_link_libraries(plugin protobuf::protobuf)
target_include_directories(plugin PRIVATE ${protobuf_INCLUDE_DIRS})

# This builds the vendored CS2 protos to your build and adds the resulting
# include path.
include(cmake/proto.cmake)
target_link_libraries(plugin proto)
target_include_directories(plugin PRIVATE ${proto_INCLUDE_DIRS})
