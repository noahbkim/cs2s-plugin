#pragma once

#include <tier0/logging.h>

#include <ISmmPlugin.h>

#include <cs2s/plugin/library.h>
#include <cs2s/plugin/detour.h>
#include <cs2s/plugin/event.h>

#include "hooks.h"

struct Player
{
    // Intrinsic
    uint32_t id;
    std::string name;
    uint64_t steam_id;
    bool bot;

    // Plugin
    int32_t rating{0};
};

class Plugin final : public ISmmPlugin, public IMetamodListener, public IGameEventListener2
{
private:
    LoggingChannelID_t log;
    cs2s::plugin::PluginLibraryService libraries;
    cs2s::plugin::PluginDetourService detours;
    cs2s::plugin::PluginEventService events;

    // Derived
    ISmmAPI* metamod{nullptr};

    // Reversed
    decltype(UTIL_ClientPrintAll)* client_print_all{nullptr};

public:
    explicit Plugin(LoggingChannelID_t log);

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
};
