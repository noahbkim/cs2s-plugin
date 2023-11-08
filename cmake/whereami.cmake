include_guard()

set(whereami_SOURCE_DIR ${CMAKE_SOURCE_DIR}/vendor/whereami/src/)
set(whereami_INCLUDE_DIRS ${whereami_SOURCE_DIR})

add_library(whereami STATIC ${whereami_SOURCE_DIR}/whereami.c)
target_include_directories(whereami PUBLIC ${whereami_INCLUDE_DIRS})
