#include "plugin.h"

#include <cstdio>

#include <Color.h>
#include <tier0/logging.h>

#include <cs2s/plugin/detour.h>
#include <cs2s/plugin/library.h>

#define STR_LITERAL(D) #D
#define STR(D) STR_LITERAL(D)

// Set up Source 2 logging. Provides macros like `Log_Msg`, `Log_Warning`, and
// `Log_Error`. See tier0/logging.h for full documentation. You can change the
// color of this channel by adding your own RGB values to the `Color()`.
DEFINE_LOGGING_CHANNEL_NO_TAGS(LOG_CS2S, STR(CMAKE_PROJECT_NAME), 0, LV_MAX, Color());

// A static instance of the plugin loaded by Metamod.
Plugin g_Plugin(LOG_CS2S);
PLUGIN_EXPOSE(Plugin, g_Plugin);

Plugin::Plugin(LoggingChannelID_t logging_channel_id)
    : libraries(logging_channel_id)
{
}

// Called when the plugin is loaded by Metamod. You can test this by running
// `meta load addons/cs2s-plugin` (interpolating the actual generated plugin
// path and name) in your server console.
bool Plugin::Load(PluginId id, ISmmAPI* ismm, char* error, size_t maxlen, bool late)
{
    Log_Msg(LOG_CS2S, "Hello, world!\n");
    if (!this->libraries.Load(id, ismm, error, maxlen, late))
    {
        return false;
    }

    this->libraries.Resolve(ROOT_BIN_DIRECTORY, "engine2");
    this->libraries.Resolve(ROOT_BIN_DIRECTORY, "tier0");
    this->libraries.Resolve(GAME_BIN_DIRECTORY, "server");
    return true;
}

// Called when the plugin is unloaded by Metamod. You can manually unload a
// plugin with `meta unload addons/cs2s-plugin`.
bool Plugin::Unload(char* error, size_t maxlen)
{
    ConVar_Unregister();
    Log_Msg(LOG_CS2S, "Goodbye, world!\n");
    return this->libraries.Unload(error, maxlen);
}
