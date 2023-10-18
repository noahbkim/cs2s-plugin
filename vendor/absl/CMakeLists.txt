set(ABSL_PROPAGATE_CXX_STD ON)
set(ABSL_ENABLE_INSTALL ON)
FetchContent_Declare(
        absl
        GIT_REPOSITORY https://github.com/abseil/abseil-cpp
        GIT_TAG 20230802.1
        OVERRIDE_FIND_PACKAGE
)

FetchContent_MakeAvailable(absl)
