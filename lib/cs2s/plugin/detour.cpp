#include "detour.h"

#include "cs2s/common/macro.h"

#define LOG_PREFIX "[" STR(PLUGIN_NAME) ":events] "

namespace cs2s::plugin
{

bool PluginDetourService::Load(PluginId id, ISmmAPI* ismm, bool late)
{
    Log_Msg(this->log, LOG_PREFIX "Loaded detour service\n");
    return true;
}

bool PluginDetourService::Unload()
{
    Log_Msg(this->log, LOG_PREFIX "Unloaded detour service\n");
    return true;
}

}
