include(ExternalProject)
set(cs2metamod_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/metamod)
set(cs2metamod_BINARY_DIR ${cs2metamod_BUILD_DIR}/core/metamod.2.cs2/linux-x86_64)
set(cs2metamod_PACKAGE_DIR ${cs2metamod_BUILD_DIR}/package)

ExternalProject_Add(
        cs2metamod-build
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/vendor/metamod-source
        BINARY_DIR ${cs2metamod_BUILD_DIR}
        CONFIGURE_COMMAND
            ${CMAKE_COMMAND} -E env HL2SDKCS2=${CMAKE_SOURCE_DIR}/vendor/hl2sdk
                python3 ${CMAKE_SOURCE_DIR}/vendor/metamod-source/configure.py
                    --sdks cs2 --disable-auto-versioning
        BUILD_COMMAND
            ambuild
            && ${CMAKE_COMMAND} -E copy_directory_if_different ${cs2metamod_PACKAGE_DIR}/addons ${cs2s_PACKAGE_DIR}/addons
            # TODO: may be able to shorten this to ${cs2metamod_PACKAGE_DIR} ${cs2s_PACKAGE_DIR}
        INSTALL_COMMAND ""
        BUILD_BYPRODUCTS
            ${cs2metamod_BINARY_DIR}/metamod.2.cs2.so
            ${cs2metamod_BINARY_DIR}/libtier0.so
)

add_library(cs2metamod SHARED IMPORTED)
add_library(cs2libtier0 SHARED IMPORTED)
add_library(cs2interfaces STATIC IMPORTED)
add_library(cs2tier1 STATIC IMPORTED)

# https://stackoverflow.com/questions/9160335/os-specific-instructions-in-cmake-how-to
if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

if(LINUX)
    set_target_properties(cs2metamod PROPERTIES IMPORTED_LOCATION ${cs2metamod_BINARY_DIR}/metamod.2.cs2.so)
    set_target_properties(cs2libtier0 PROPERTIES IMPORTED_LOCATION ${cs2metamod_BINARY_DIR}/libtier0.so)
    set_target_properties(cs2interfaces PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/vendor/hl2sdk/lib/linux64/interfaces.a)
    set_target_properties(cs2tier1 PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/vendor/hl2sdk/lib/linux64/tier1.a)
endif()

if(WIN32)
    set_target_properties(cs2metamod PROPERTIES IMPORTED_IMPLIB ${cs2metamod_BINARY_DIR}/metamod.2.cs2.dll)
    set_target_properties(cs2metamod PROPERTIES IMPORTED_LOCATION ${cs2metamod_BINARY_DIR}/metamod.2.cs2.dll)
    set_target_properties(cs2libtier0 PROPERTIES IMPORTED_IMPLIB ${CMAKE_SOURCE_DIR}/vendor/hl2sdk/lib/public/win64/tier0.lib)
    set_target_properties(cs2libtier0 PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/vendor/hl2sdk/lib/public/win64/tier0.lib)
    set_target_properties(cs2interfaces PROPERTIES IMPORTED_IMPLIB ${CMAKE_SOURCE_DIR}/vendor/hl2sdk/lib/public/win64/interfaces.lib)
    set_target_properties(cs2interfaces PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/vendor/hl2sdk/lib/public/win64/interfaces.lib)
    set_target_properties(cs2tier1 PROPERTIES IMPORTED_IMPLIB ${CMAKE_SOURCE_DIR}/vendor/hl2sdk/lib/public/win64/tier1.lib)
    set_target_properties(cs2tier1 PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/vendor/hl2sdk/lib/public/win64/tier1.lib)
endif()
