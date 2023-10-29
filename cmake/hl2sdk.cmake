include_guard()

set(hl2sdk_SOURCE_DIR ${CMAKE_SOURCE_DIR}/vendor/hl2sdk)
set(hl2sdk_INCLUDE_DIRS
        ${hl2sdk_SOURCE_DIR}/public
        ${hl2sdk_SOURCE_DIR}/public/engine
        ${hl2sdk_SOURCE_DIR}/public/entity2
        ${hl2sdk_SOURCE_DIR}/public/mathlib
        ${hl2sdk_SOURCE_DIR}/public/vstdlib
        ${hl2sdk_SOURCE_DIR}/public/tier0
        ${hl2sdk_SOURCE_DIR}/public/tier1
)

add_library(hl2sdk::interfaces STATIC IMPORTED)
add_library(hl2sdk::tier0 SHARED IMPORTED)
add_library(hl2sdk::tier1 STATIC IMPORTED)

target_include_directories(hl2sdk::interfaces INTERFACE ${hl2sdk_INCLUDE_DIRS})
target_include_directories(hl2sdk::tier0 INTERFACE ${hl2sdk_INCLUDE_DIRS})
target_include_directories(hl2sdk::tier1 INTERFACE ${hl2sdk_INCLUDE_DIRS})

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    target_compile_definitions(hl2sdk::interfaces INTERFACE PLATFORM_64BITS)
    target_compile_definitions(hl2sdk::tier0 INTERFACE PLATFORM_64BITS)
    target_compile_definitions(hl2sdk::tier1 INTERFACE PLATFORM_64BITS)
endif()

if(LINUX)
    set_target_properties(hl2sdk::interfaces PROPERTIES IMPORTED_LOCATION ${hl2sdk_SOURCE_DIR}/lib/linux64/interfaces.a)
    set_target_properties(hl2sdk::tier0 PROPERTIES IMPORTED_LOCATION ${hl2sdk_SOURCE_DIR}/lib/linux64/libtier0.so)
    set_target_properties(hl2sdk::tier1 PROPERTIES IMPORTED_LOCATION ${hl2sdk_SOURCE_DIR}/lib/linux64/tier1.a)
    target_compile_definitions(hl2sdk::interfaces INTERFACE POSIX _LINUX)
    target_compile_definitions(hl2sdk::tier0 INTERFACE POSIX _LINUX)
    target_compile_definitions(hl2sdk::tier1 INTERFACE POSIX _LINUX)

elseif(WIN32)
    set_target_properties(hl2sdk::interfaces PROPERTIES IMPORTED_IMPLIB ${hl2sdk_SOURCE_DIR}/lib/public/win64/interfaces.lib)
    set_target_properties(hl2sdk::interfaces PROPERTIES IMPORTED_LOCATION ${hl2sdk_SOURCE_DIR}/lib/public/win64/interfaces.lib)
    set_target_properties(hl2sdk::tier0 PROPERTIES IMPORTED_IMPLIB ${hl2sdk_SOURCE_DIR}/lib/public/win64/tier0.lib)
    set_target_properties(hl2sdk::tier0 PROPERTIES IMPORTED_LOCATION ${hl2sdk_SOURCE_DIR}/lib/public/win64/tier0.lib)
    set_target_properties(hl2sdk::tier1 PROPERTIES IMPORTED_IMPLIB ${hl2sdk_SOURCE_DIR}/lib/public/win64/tier1.lib)
    set_target_properties(hl2sdk::tier1 PROPERTIES IMPORTED_LOCATION ${hl2sdk_SOURCE_DIR}/lib/public/win64/tier1.lib)
    # WIN32 is already defined

else()
    message(SEND_ERROR "hl2sdk has not been configured for this platform!")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_definitions(hl2sdk::interfaces INTERFACE COMPILER_GCC GNUC)
    target_compile_definitions(hl2sdk::tier0 INTERFACE COMPILER_GCC GNUC)
    target_compile_definitions(hl2sdk::tier1 INTERFACE COMPILER_GCC GNUC)

elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
#    message(WARNING "cs2metamod configuration is incomplete for this compiler!")
    target_compile_definitions(hl2sdk::interfaces INTERFACE COMPILER_MSVC)
    target_compile_definitions(hl2sdk::tier0 INTERFACE COMPILER_MSVC)
    target_compile_definitions(hl2sdk::tier1 INTERFACE COMPILER_MSVC)

    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        target_compile_definitions(hl2sdk::interfaces INTERFACE COMPILER_MSVC64 _AMD64_)
        target_compile_definitions(hl2sdk::tier0 INTERFACE COMPILER_MSVC64 _AMD64_)
        target_compile_definitions(hl2sdk::tier1 INTERFACE COMPILER_MSVC64 _AMD64_)
    else()
        target_compile_definitions(hl2sdk::interfaces INTERFACE COMPILER_MSVC32 _X86_)
        target_compile_definitions(hl2sdk::tier0 INTERFACE COMPILER_MSVC32 _X86_)
        target_compile_definitions(hl2sdk::tier1 INTERFACE COMPILER_MSVC32 _X86_)
    endif()
else()
    message(FATAL_ERROR "cs2metamod has not been configured for this compiler!")
endif()
