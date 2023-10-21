#include "event.h"

#include "cs2s/common/macro.h"
#include "cs2s/common/virtual.h"

#define LOG_PREFIX "[" STR(CS2S_PLUGIN_NAME) ":events] "

namespace cs2s::plugin
{

bool PluginEventService::Load(PluginId id, ISmmAPI* ismm, bool late)
{
    this->metamod = ismm;

    // We need the ISource2Server to get the IGameEventManager2
    auto source_2_server = reinterpret_cast<ISource2Server*>(
        ismm->VInterfaceMatch(ismm->GetServerFactory(), SOURCE2SERVER_INTERFACE_VERSION)
    );
    if (!source_2_server)
    {
        Log_Error(this->log, LOG_PREFIX "Could not resolve interface ISource2Server!\n");
        return false;
    }

    // Derive the game event manager
    IToolGameEventAPI* tool_game_event_api = cs2s::common::vcall<91, IToolGameEventAPI*>(source_2_server);
    this->game_event_manager = static_cast<IGameEventManager2*>(tool_game_event_api);
    if (!this->game_event_manager)
    {
        Log_Error(this->log, LOG_PREFIX "Could not resolve the IGameEventManager2!\n");
        return false;
    }
    Log_Msg(this->log, LOG_PREFIX "Found IGameEventManager2 at %p\n", this->game_event_manager);

    return true;
}

bool PluginEventService::Unload()
{
    if (this->metamod == nullptr)
    {
        return true;
    }

    // Do this by hand; we can't call `Unsubscribe` because we can't erase while iterating
    size_t listener_count = this->listeners.size();
    for (IGameEventListener2* listener : this->listeners)
    {
        this->game_event_manager->RemoveListener(listener);
    }
    this->listeners.clear();
    Log_Msg(this->log, LOG_PREFIX "Unsubscribed %zu remaining listeners\n", listener_count);

    // Reset members
    this->metamod = nullptr;
    this->game_event_manager = nullptr;

    Log_Msg(this->log, LOG_PREFIX "Unloaded event service\n", listener_count);
    return true;
}

void PluginEventService::Subscribe(const char* event_name, IGameEventListener2* listener)
{
    Log_Msg(this->log, LOG_PREFIX "Added listener %p to %p\n", listener, event_name);
    this->game_event_manager->AddListener(listener, event_name, true);
    this->listeners.emplace(listener);
}

void PluginEventService::Unsubscribe(IGameEventListener2* listener)
{
    Log_Msg(this->log, LOG_PREFIX "Removed listener %p\n", listener);
    this->game_event_manager->RemoveListener(listener);
    this->listeners.erase(listener);
}

}
