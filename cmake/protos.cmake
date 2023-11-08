include_guard()

find_package(Protobuf REQUIRED)

set(protos_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/protos)
set(protos_INCLUDE_DIR ${protos_BUILD_DIR})

# The actual static library containing all the built protos
file(GLOB protos_SOURCES ${CMAKE_SOURCE_DIR}/vendor/GameTracking-CS2/Protobufs/*.proto)
add_library(protos STATIC ${protos_SOURCES})
target_link_libraries(protos PRIVATE protobuf::libprotobuf)
target_include_directories(protos PUBLIC ${protos_INCLUDE_DIR})

# A directory to write generated .pb.h and .pb.cc files
add_custom_target(protos_directory ALL COMMAND ${CMAKE_COMMAND} -E make_directory ${protos_BUILD_DIR})
add_dependencies(protos protos_directory)

# We need this for the protobuf generation step
include_directories(${protobuf_INCLUDE_DIRS})

# Generation step for .pb.h and .pb.cc files
protobuf_generate(
        TARGET protos
        DEPENDENCIES protos_directory
        LANGUAGE cpp
        PROTOC_OUT_DIR ${protos_BUILD_DIR}
        APPEND_PATH
)
