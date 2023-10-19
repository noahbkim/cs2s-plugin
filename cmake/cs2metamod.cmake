include(ExternalProject)
set(cs2metamod_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/metamod)
set(cs2metamod_BINARY_DIR ${cs2metamod_BUILD_DIR}/core/metamod.2.cs2/linux-x86_64/)

ExternalProject_Add(
        cs2metamod-build
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/vendor/metamod-source
        BINARY_DIR ${cs2metamod_BUILD_DIR}
        CONFIGURE_COMMAND
            ${CMAKE_COMMAND} -E env HL2SDKCS2=${CMAKE_SOURCE_DIR}/vendor/hl2sdk
                python3 ${CMAKE_SOURCE_DIR}/vendor/metamod-source/configure.py
                    --sdks cs2 --disable-auto-versioning
        BUILD_COMMAND ambuild
        INSTALL_COMMAND ""
        BUILD_BYPRODUCTS
            ${cs2metamod_BINARY_DIR}/metamod.2.cs2.so
            ${cs2metamod_BINARY_DIR}/libtier0.so
)

add_library(cs2metamod SHARED IMPORTED)
set_target_properties(cs2metamod PROPERTIES IMPORTED_LOCATION ${cs2metamod_BINARY_DIR}/metamod.2.cs2.so)

add_library(cs2libtier0 SHARED IMPORTED)
set_target_properties(cs2libtier0 PROPERTIES IMPORTED_LOCATION ${cs2metamod_BINARY_DIR}/libtier0.so)

add_library(cs2interfaces STATIC IMPORTED)
set_target_properties(cs2interfaces PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/vendor/hl2sdk/lib/linux64/interfaces.a)

add_library(cs2tier1 STATIC IMPORTED)
set_target_properties(cs2tier1 PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/vendor/hl2sdk/lib/linux64/tier1.a)
