#pragma once

// https://github.com/abseil/abseil-cpp/issues/1554
// #include <absl/container/flat_hash_set.h>
#include <unordered_set>

#include <engine/igameeventsystem.h>
#include <igameevents.h>

#include <ISmmPlugin.h>

#include "cs2s/plugin/service.h"

namespace cs2s::plugin::service
{

class PluginEventService : public PluginService
{
protected:
    ISmmAPI* metamod{nullptr};
    IGameEventManager2* game_event_manager{nullptr};

    // Used to unregister remaining listeners in `Unload`
    std::unordered_set<IGameEventListener2*> listeners;

public:
    using PluginService::PluginService;

public:
    bool Load(PluginId id, ISmmAPI* ismm, bool late) override;
    bool Unload() override;

public:
    bool Subscribe(const char* event_name, IGameEventListener2* listener);
    void Unsubscribe(IGameEventListener2* listener);
};

}
