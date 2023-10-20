#pragma once

#include <tier0/logging.h>
#include <engine/igameeventsystem.h>
#include <igameevents.h>
#include <iserver.h>
#include <networksystem/inetworkserializer.h>

#include <ISmmPlugin.h>

#include <cs2s/plugin/library.h>
#include <cs2s/plugin/detour.h>
#include <cs2s/plugin/event.h>

void UTIL_ClientPrintAll(int msg_dest, const char *msg_name, const char *param1 = NULL, const char *param2 = NULL, const char *param3 = NULL, const char *param4 = NULL);

class Plugin final : public ISmmPlugin, public IMetamodListener, public IGameEventListener2
{
private:
    LoggingChannelID_t log;
    cs2s::plugin::PluginLibraryService libraries;
    cs2s::plugin::PluginDetourService detours;
    cs2s::plugin::PluginEventService events;

    // Derived
    ISmmAPI* metamod{nullptr};
    ICvar* con_vars{nullptr};
    IVEngineServer2* engine_server_2{nullptr};
    ISource2Server* source_2_server{nullptr};
    ISource2GameEntities* source_2_game_entities{nullptr};
    IServerGameClients* server_game_clients{nullptr};
    INetworkServerService* network_server_service{nullptr};
    IGameEventSystem* game_event_system{nullptr};
    IGameEventManager2* game_event_manager{nullptr};

    // Reversed
    decltype(UTIL_ClientPrintAll)* client_print_all{nullptr};

public:
    explicit Plugin(LoggingChannelID_t log);

public:
    bool Load(PluginId id, ISmmAPI* ismm, char* error, size_t maxlen, bool late) override;
    bool Unload(char* error, size_t maxlen) override;

    const char* GetAuthor() override { return "Noah Kim"; }
    const char* GetName() override { return "Plugin Template"; }
    const char* GetDescription() override { return "A CS2 server plugin template."; }
    const char* GetURL() override { return "https://github.com/noahbkim/cs2s-plugin"; }
    const char* GetLicense() override { return "MIT"; };
    const char* GetVersion() override { return "0.1"; };
    const char* GetDate() override { return "2023-10-18"; };
    const char* GetLogTag() override { return "cs2s-plugin"; };

public:
    void FireGameEvent(IGameEvent *event) override;
};
