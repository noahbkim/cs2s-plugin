FetchContent_Declare(
        Protobuf
        GIT_REPOSITORY https://github.com/protocolbuffers/protobuf
        GIT_TAG v24.4
        OVERRIDE_FIND_PACKAGE
)

# Avoid additional dependencies of tests
set(protobuf_BUILD_TESTS OFF)

# Requires ../zlib and ../absl
FetchContent_MakeAvailable(Protobuf)

# Make generation commands available
# https://stackoverflow.com/questions/74844369/how-to-i-use-the-cmake-command-protobuf-generate-when-installing-protobuf-throug
FetchContent_GetProperties(Protobuf SOURCE_DIR Protobuf_SOURCE_DIR)
include(${Protobuf_SOURCE_DIR}/cmake/protobuf-generate.cmake)
