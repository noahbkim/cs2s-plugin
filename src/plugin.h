#pragma once

#include <utility>
#include <vector>

#include <tier0/logging.h>
#include <const.h>

#include <ISmmPlugin.h>

#include <cs2s/plugin/service/library.h>
#include <cs2s/plugin/service/detour.h>
#include <cs2s/plugin/service/event.h>

#include "hooks.h"

struct Player
{
    // Intrinsic
    std::string name;
    uint64_t steam_id{0};
    bool bot{false};

    // Plugin
    int32_t rating{0};
};

class Plugin final : public ISmmPlugin, public IMetamodListener, public IGameEventListener2
{
private:
    LoggingChannelID_t log;
    cs2s::plugin::service::PluginLibraryService libraries;
    cs2s::plugin::service::PluginEventService events;

    // Derived
    ISmmAPI* metamod{nullptr};

    // Reversed
    decltype(UTIL_ClientPrintAll)* client_print_all{nullptr};

    // Tracking (connected, Player)
    std::vector<std::pair<bool, Player>> players;

public:
    explicit Plugin(LoggingChannelID_t log);

    // We frequently pass references to `self`; ensure we're never copied or moved
    Plugin(Plugin&&) = delete;
    Plugin(const Plugin&) = delete;

public:
    bool Load(PluginId id, ISmmAPI* ismm, char* error, size_t maxlen, bool late) override;
    bool Unload(char* error, size_t maxlen) override;

    const char* GetAuthor() override { return "Noah Kim"; }
    const char* GetName() override { return "CS2S Plugin"; }
    const char* GetDescription() override { return "A CS2 server plugin template."; }
    const char* GetURL() override { return "https://github.com/noahbkim/cs2s-plugin"; }
    const char* GetLicense() override { return "MIT"; };
    const char* GetVersion() override { return "0.1"; };
    const char* GetDate() override { return "2023-10-18"; };
    const char* GetLogTag() override { return "cs2s-plugin"; };

public:
    void FireGameEvent(IGameEvent *event) override;

private:
    void PlayerConnect(IGameEvent* event);
    void PlayerInfo(IGameEvent* event);
    void PlayerDisconnect(IGameEvent* event);
    void PlayerDeath(IGameEvent* event);
};
