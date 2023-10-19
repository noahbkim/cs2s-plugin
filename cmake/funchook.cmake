include(FetchContent)

FetchContent_Declare(
        funchook
        GIT_REPOSITORY https://github.com/kubo/funchook
        GIT_TAG master
        OVERRIDE_FIND_PACKAGE
)

set(FUNCHOOK_INSTALL OFF)
FetchContent_MakeAvailable(funchook)

set(funchook_INCLUDE_DIRS ${funchook_SOURCE_DIR}/include)
