include(FetchContent)

FetchContent_Declare(
        funchook
        GIT_REPOSITORY https://github.com/kubo/funchook
        GIT_TAG master
        OVERRIDE_FIND_PACKAGE
)

set(FUNCHOOK_INSTALL OFF)
FetchContent_MakeAvailable(funchook)
