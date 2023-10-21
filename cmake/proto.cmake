include_guard()

find_package(Protobuf REQUIRED)

set(proto_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/proto)
set(proto_INCLUDE_DIRS ${proto_BUILD_DIR})

# The actual static library containing all the built protos
file(GLOB proto_SOURCES ${CMAKE_SOURCE_DIR}/vendor/GameTracking-CS2/Protobufs/*.proto)
add_library(proto STATIC ${proto_SOURCES})

# A directory to write generated .pb.h and .pb.cc files
add_custom_target(
        proto_directory ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory ${proto_BUILD_DIR}
)

# We need this for the protobuf generation step
include_directories(${protobuf_INCLUDE_DIRS})

# Generation step for .pb.h and .pb.cc files
protobuf_generate(
        TARGET proto
        DEPENDENCIES proto_directory
        LANGUAGE cpp
        PROTOC_OUT_DIR ${proto_BUILD_DIR}
        APPEND_PATH
)
