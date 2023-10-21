include_guard()

# https://stackoverflow.com/questions/9160335/os-specific-instructions-in-cmake-how-to
if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

if(NOT (LINUX OR WIN32 OR APPLE))
    message(FATAL_ERROR "unrecognized platform!")
endif()
