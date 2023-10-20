#pragma once

#include <tier0/logging.h>
#include <engine/igameeventsystem.h>
#include <iserver.h>
#include <networksystem/inetworkserializer.h>

#include <ISmmPlugin.h>

#include <cs2s/plugin/library.h>
#include <cs2s/plugin/detour.h>

void UTIL_ClientPrintAll(int msg_dest, const char *msg_name, const char *param1 = NULL, const char *param2 = NULL, const char *param3 = NULL, const char *param4 = NULL);

class Plugin final : public ISmmPlugin, public IMetamodListener
{
private:
    LoggingChannelID_t log;
    cs2s::plugin::PluginLibraryService libraries;
    cs2s::plugin::PluginDetourService detours;

    // Derived
    ISmmAPI* metamod{nullptr};
    ICvar* con_vars{nullptr};
    IVEngineServer2* engine_server_2{nullptr};
    ISource2Server* source_2_server{nullptr};
    ISource2GameEntities* source_2_game_entities{nullptr};
    IServerGameClients* server_game_clients{nullptr};
    INetworkServerService* network_server_service{nullptr};
    IGameEventSystem* game_event_system{nullptr};

    // Reversed
    decltype(UTIL_ClientPrintAll)* client_print_all{nullptr};

public:
    explicit Plugin(LoggingChannelID_t log);

public:
    bool Load(PluginId id, ISmmAPI* ismm, char* error, size_t maxlen, bool late) override;
    bool Unload(char* error, size_t maxlen) override;

    const char *GetAuthor() override { return "Noah Kim"; }
    const char *GetName() override { return "Plugin Template"; }
    const char *GetDescription() override { return "A CS2 server plugin template."; }
    const char *GetURL() override { return "https://github.com/noahbkim/cs2s-plugin"; }
    const char *GetLicense() override { return "MIT"; };
    const char *GetVersion() override { return "0.1"; };
    const char *GetDate() override { return "2023-10-18"; };
    const char *GetLogTag() override { return "cs2s-plugin"; };

public:
    void PostEventAbstract(
        CSplitScreenSlot nSlot,
        bool bLocalOnly,
        int nClientCount,
        const uint64* clients,
        INetworkSerializable* pEvent,
        const void* pData,
        unsigned long nSize,
        NetChannelBufType_t bufType
    );
};


/*
 * error: no matches converting function 'PostEventAbstract' to type
 * 'void (class IGameEventSystem::*)(struct CSplitScreenSlot, bool, int, const uint64_t*, class INetworkSerializable*, const void*, long unsigned int, enum NetChannelBufType_t)' {aka
 * 'void (class IGameEventSystem::*)(struct CSplitScreenSlot, bool, int, const long unsigned int*, class INetworkSerializable*, const void*, long unsigned int, enum NetChannelBufType_t)'}
 *
 *                   {CSplitScreenSlot, bool, int, const long long unsigned int*, INetworkSerializable*, const void*, long unsigned int, NetChannelBufType_t}](&Plugin::PostEventAbstract)' from '
 * FastDelegate<[...],CSplitScreenSlot, bool, int, const long long unsigned int*, INetworkSerializable*, const void*, long unsigned int, NetChannelBufType_t>' to
 * FastDelegate<[...],CSplitScreenSlot, bool, int, const long unsigned int*, INetworkSerializable*, const void*, long unsigned int, NetChannelBufType_t>'
 */