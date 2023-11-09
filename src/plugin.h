#pragma once

#include <utility>
#include <vector>

#include <tier0/logging.h>
#include <igameeventsystem.h>
#include <igameevents.h>

#include <ISmmPlugin.h>

#include <cs2s/service/message.h>
#include <cs2s/service/event.h>

class Plugin final : public ISmmPlugin, public IMetamodListener, public IGameEventListener2
{
private:
    LoggingChannelID_t log;
    cs2s::service::PluginEventService events;
    cs2s::service::PluginMessageService messages;

    // Derived
    ISmmAPI* metamod{nullptr};

public:
    explicit Plugin(LoggingChannelID_t log);

    // Ensure instances are never moved so pointers relative to `self` remain
    // valid during runtime. TODO: verify this is complete.
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
};
