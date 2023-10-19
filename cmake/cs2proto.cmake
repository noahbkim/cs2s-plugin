find_package(protobuf REQUIRED)

file(GLOB cs2proto_SRC_FILES ${CMAKE_SOURCE_DIR}/vendor/GameTracking-CS2/Protobufs/*.proto)
add_library(cs2proto STATIC ${cs2proto_SRC_FILES})

add_custom_target(
        cs2proto-directory ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/proto
)

include_directories(${protobuf_INCLUDE_DIRS})
protobuf_generate(
        TARGET cs2proto
        DEPENDENCIES cs2proto-directory
        LANGUAGE cpp
        PROTOC_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/proto
        APPEND_PATH
)
