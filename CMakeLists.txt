cmake_minimum_required(VERSION 3.23)
set(CMAKE_CXX_STANDARD 17)
include(FetchContent)

project(cs2s-plugin)

add_subdirectory(vendor/funchook)
add_subdirectory(vendor/zlib)
add_subdirectory(vendor/absl)
add_subdirectory(vendor/protobuf)

add_library(cs2proto STATIC
        vendor/GameTracking-CS2/Protobufs/base_gcmessages_csgo.proto
        vendor/GameTracking-CS2/Protobufs/c_peer2peer_netmessages.proto
        vendor/GameTracking-CS2/Protobufs/clientmessages.proto
        vendor/GameTracking-CS2/Protobufs/connectionless_netmessages.proto
        vendor/GameTracking-CS2/Protobufs/cs_gameevents.proto
        vendor/GameTracking-CS2/Protobufs/cs_usercmd.proto
        vendor/GameTracking-CS2/Protobufs/cstrike15_gcmessages.proto
        vendor/GameTracking-CS2/Protobufs/cstrike15_usermessages.proto
        vendor/GameTracking-CS2/Protobufs/demo.proto
        vendor/GameTracking-CS2/Protobufs/econ_gcmessages.proto
        vendor/GameTracking-CS2/Protobufs/engine_gcmessages.proto
        vendor/GameTracking-CS2/Protobufs/enums_clientserver.proto
        vendor/GameTracking-CS2/Protobufs/fatdemo.proto
        vendor/GameTracking-CS2/Protobufs/gameevents.proto
        vendor/GameTracking-CS2/Protobufs/gcsdk_gcmessages.proto
        vendor/GameTracking-CS2/Protobufs/gcsystemmsgs.proto
        vendor/GameTracking-CS2/Protobufs/netmessages.proto
        vendor/GameTracking-CS2/Protobufs/network_connection.proto
        vendor/GameTracking-CS2/Protobufs/networkbasetypes.proto
        vendor/GameTracking-CS2/Protobufs/networksystem_protomessages.proto
        vendor/GameTracking-CS2/Protobufs/steamdatagram_messages_auth.proto
        vendor/GameTracking-CS2/Protobufs/steamdatagram_messages_sdr.proto
        vendor/GameTracking-CS2/Protobufs/steammessages.proto
        vendor/GameTracking-CS2/Protobufs/steammessages_base.proto
        vendor/GameTracking-CS2/Protobufs/steammessages_cloud.steamworkssdk.proto
        vendor/GameTracking-CS2/Protobufs/steammessages_gamenetworkingui.proto
        vendor/GameTracking-CS2/Protobufs/steammessages_helprequest.steamworkssdk.proto
        vendor/GameTracking-CS2/Protobufs/steammessages_oauth.steamworkssdk.proto
        vendor/GameTracking-CS2/Protobufs/steammessages_player.steamworkssdk.proto
        vendor/GameTracking-CS2/Protobufs/steammessages_publishedfile.steamworkssdk.proto
        vendor/GameTracking-CS2/Protobufs/steammessages_steamlearn.steamworkssdk.proto
        vendor/GameTracking-CS2/Protobufs/steammessages_unified_base.steamworkssdk.proto
        vendor/GameTracking-CS2/Protobufs/steamnetworkingsockets_messages.proto
        vendor/GameTracking-CS2/Protobufs/steamnetworkingsockets_messages_certs.proto
        vendor/GameTracking-CS2/Protobufs/steamnetworkingsockets_messages_udp.proto
        vendor/GameTracking-CS2/Protobufs/te.proto
        vendor/GameTracking-CS2/Protobufs/uifontfile_format.proto
        vendor/GameTracking-CS2/Protobufs/usercmd.proto
        vendor/GameTracking-CS2/Protobufs/usermessages.proto
        vendor/GameTracking-CS2/Protobufs/valveextensions.proto
)

include_directories(${Protobuf_INCLUDE_DIRS})
protobuf_generate(
        TARGET cs2proto
        LANGUAGE cpp
        APPEND_PATH
)

target_include_directories(cs2proto PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
