#include "detour.h"

namespace cs2s::plugin
{

bool PluginDetourService::Load(PluginId id, ISmmAPI* ismm, bool late)
{
    Log_Msg(this->log, "[CS2S:detours] Loaded detour service\n");
    return true;
}

bool PluginDetourService::Unload()
{
    Log_Msg(this->log, "[CS2S:detours] Unloaded detour service\n");
    return true;
}

}
