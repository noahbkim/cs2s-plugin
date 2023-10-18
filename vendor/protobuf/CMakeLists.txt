FetchContent_Declare(
        protobuf
        GIT_REPOSITORY https://github.com/protocolbuffers/protobuf
        GIT_TAG v24.4
        OVERRIDE_FIND_PACKAGE
)

# Avoid additional dependencies of tests
set(protobuf_BUILD_TESTS OFF)

# Requires ../zlib and ../absl
FetchContent_MakeAvailable(protobuf)

# Make generate commands available
# https://stackoverflow.com/questions/74844369/how-to-i-use-the-cmake-command-protobuf-generate-when-installing-protobuf-throug
FetchContent_GetProperties(protobuf SOURCE_DIR Protobuf_SOURCE_DIR)
include(${protobuf_SOURCE_DIR}/cmake/protobuf-generate.cmake)

# Set the .proto include directory manually
# TODO see if there's a better way to do this
set(protobuf_IMPORT_DIRS ${protobuf_SOURCE_DIR}/src PARENT_SCOPE)
set(protobuf_INCLUDE_DIRS ${protobuf_SOURCE_DIR}/src PARENT_SCOPE)
