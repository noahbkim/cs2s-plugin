#include "detour.h"

namespace cs2s::plugin
{

bool PluginDetourService::Load(PluginId id, ISmmAPI* ismm, char* error, size_t maxlen, bool late)
{
    Log_Msg(this->log, "[CS2S:detours] Loaded detour service\n");
    return true;
}

bool PluginDetourService::Unload(char* error, size_t maxlen)
{
    Log_Msg(this->log, "[CS2S:detours] Unloaded detour service\n");
    return true;
}

}
