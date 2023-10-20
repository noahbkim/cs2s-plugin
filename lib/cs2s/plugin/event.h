#pragma once

#include <functional>

#include <absl/container/flat_hash_set.h>

#include <engine/igameeventsystem.h>
#include <igameevents.h>

#include <ISmmPlugin.h>

#include "service.h"

namespace cs2s::plugin
{

class PluginEventService : public PluginService
{
protected:
    ISmmAPI* metamod{nullptr};
    ISource2Server* source_2_server{nullptr};
    IGameEventManager2* game_event_manager{nullptr};

    absl::flat_hash_set<IGameEventListener2*> listeners;

public:
    using PluginService::PluginService;

public:
    bool Load(PluginId id, ISmmAPI* ismm, char* error, size_t maxlen, bool late) override;
    bool Unload(char* error, size_t maxlen) override;

public:
    void Subscribe(const char* event_name, IGameEventListener2* listener);
    void Unsubscribe(IGameEventListener2* listener);
};

}
