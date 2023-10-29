include_guard()

include(ExternalProject)

set(metamod_SOURCE_DIR ${CMAKE_SOURCE_DIR}/vendor/metamod-source)
set(metamod_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/metamod)
set(metamod_PACKAGE_DIR ${metamod_BUILD_DIR}/package)

# We need an SDK to compile against though
if(NOT DEFINED hl2sdk_SOURCE_DIR)
    message(FATAL_ERROR "metamod requires hl2sdk_SOURCE_DIR to be set")
endif()

if(LINUX AND CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(metamod_BINARY_DIR ${metamod_BUILD_DIR}/core/metamod.2.cs2/linux-x86_64)
    set(metamod_METAMOD_BINARY ${metamod_BINARY_DIR}/metamod.2.cs2.so)
elseif(WIN32 AND CMAKE_SIZEOF_VOID_P EQUAL 8)
    # I still wanna use this for my editor
    message(WARNING "metamod binary directory is not configured for this platform!")
    set(metamod_BINARY_DIR ${metamod_BUILD_DIR}/core/metamod.2.cs2/windows-x86_64)
    set(metamod_METAMOD_BINARY ${metamod_BINARY_DIR}/metamod.2.cs2.lib)
else()
    message(FATAL_ERROR "metamod binary directory is not configured for this platform!")
endif()

ExternalProject_Add(
        metamod_build
        PREFIX metamod_build
        SOURCE_DIR ${metamod_SOURCE_DIR}
        BINARY_DIR ${metamod_BUILD_DIR}
        CONFIGURE_COMMAND
            ${CMAKE_COMMAND} -E env HL2SDKCS2=${hl2sdk_SOURCE_DIR}
                python3 ${metamod_SOURCE_DIR}/configure.py
                    --sdks cs2 --disable-auto-versioning
        BUILD_COMMAND ${CMAKE_COMMAND} -E env HL2SDKCS2=${hl2sdk_SOURCE_DIR} ambuild
        INSTALL_COMMAND ""
        BUILD_BYPRODUCTS ${metamod_METAMOD_BINARY}
)

add_library(metamod SHARED IMPORTED)

target_include_directories(metamod INTERFACE
        ${metamod_SOURCE_DIR}/core
        ${metamod_SOURCE_DIR}/core/sourcehook
)
add_dependencies(metamod metamod_build)

if(LINUX)
    set_target_properties(metamod PROPERTIES IMPORTED_LOCATION ${metamod_METAMOD_BINARY})

elseif(WIN32)
    set_target_properties(metamod PROPERTIES IMPORTED_IMPLIB ${metamod_METAMOD_BINARY})
    set_target_properties(metamod PROPERTIES IMPORTED_LOCATION ${metamod_METAMOD_BINARY})

else()
    message(SEND_ERROR "metamod has not been configured for this platform!")
endif()

target_compile_definitions(metamod INTERFACE META_IS_SOURCE2)
