#include "event.h"

#include "cs2s/common/virtual.h"

namespace cs2s::plugin
{

bool PluginEventService::Load(PluginId id, ISmmAPI* ismm, char* error, size_t maxlen, bool late)
{
    this->metamod = ismm;

    GET_V_IFACE_ANY(GetServerFactory, this->source_2_server, ISource2Server, SOURCE2SERVER_INTERFACE_VERSION);

    // Derive the game event manager
    IToolGameEventAPI* tool_game_event_api = cs2s::common::vcall<91, IToolGameEventAPI*>(this->source_2_server);
    this->game_event_manager = static_cast<IGameEventManager2*>(tool_game_event_api);
    if (!this->game_event_manager)
    {
        Log_Error(this->log, "[CS2S:events] Could not resolve a IGameEventManager2!\n");
        return false;
    }
    Log_Msg(this->log, "[CS2S:events] Found IGameEventManager2 at %p\n", this->game_event_manager);

    return true;
}

bool PluginEventService::Unload(char* error, size_t maxlen)
{
    // Do this by hand; we can't call `Unsubscribe` because we can't erase while iterating
    size_t listener_count = this->listeners.size();
    for (IGameEventListener2* listener : this->listeners)
    {
        this->game_event_manager->RemoveListener(listener);
    }
    this->listeners.clear();

    Log_Msg(this->log, "[CS2S:events] Unsubscribed %zu listeners and unloaded event service\n", listener_count);
    return true;
}

void PluginEventService::Subscribe(const char* event_name, IGameEventListener2* listener)
{
    Log_Msg(this->log, "[CS2S:events] Added listener %p to %p\n", listener, event_name);
    this->game_event_manager->AddListener(listener, event_name, true);
    this->listeners.emplace(listener);
}

void PluginEventService::Unsubscribe(IGameEventListener2* listener)
{
    Log_Msg(this->log, "[CS2S:events] Removed listener %p\n", listener);
    this->game_event_manager->RemoveListener(listener);
    this->listeners.erase(listener);
}

}
