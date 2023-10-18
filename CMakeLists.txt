cmake_minimum_required(VERSION 3.23)
set(CMAKE_CXX_STANDARD 17)
include(FetchContent)

project(cs2s-plugin)

add_subdirectory(vendor/funchook)
add_subdirectory(vendor/absl)
add_subdirectory(vendor/protobuf)

add_custom_target(
        metamod-directory ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/metamod
)

add_custom_target(
        metamod
        DEPENDS metamod-directory
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/metamod
        COMMAND
            python3 ${CMAKE_SOURCE_DIR}/vendor/metamod-source/configure.py
            && ambuild ${CMAKE_CURRENT_BINARY_DIR}/metamod
)

file(GLOB cs2s_PROTO_SRC_FILES ${CMAKE_SOURCE_DIR}/vendor/GameTracking-CS2/Protobufs/*.proto)
add_library(cs2s-plugin
        src/main.cpp
        ${cs2s_PROTO_SRC_FILES}
)

add_dependencies(cs2s-plugin metamod)

add_custom_target(
        proto-directory ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/proto
)

protobuf_generate(
        TARGET cs2s-plugin
        DEPENDENCIES proto-directory
        LANGUAGE cpp
        IMPORT_DIRS ${protobuf_INCLUDE_DIRS}
        PROTOC_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/proto
        APPEND_PATH
)

target_link_libraries(cs2s-plugin PRIVATE protobuf absl)
target_include_directories(
        cs2s-plugin
        PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
        PRIVATE ${protobuf_INCLUDE_DIRS} ${absl_INCLUDE_DIRS}
)
