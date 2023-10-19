#include <cstdio>

#include <Color.h>
#include <tier0/logging.h>

#include "plugin.h"

#define STR_LITERAL(D) #D
#define STR(D) STR_LITERAL(D)

// A static instance of the plugin loaded by Metamod.
Plugin g_Plugin;
PLUGIN_EXPOSE(Plugin, g_Plugin);

// Set up logging.
DEFINE_LOGGING_CHANNEL_NO_TAGS(LOG_CS2S, STR(CMAKE_PROJECT_NAME), 0, LV_MAX, Color());

// Called when the plugin is loaded by Metamod. You can test this by running
// `meta load addons/cs2s-plugin` (interpolating the actual generated plugin
// path and name) in your server console.
bool Plugin::Load(PluginId id, ISmmAPI* ismm, char* error, size_t maxlen, bool late)
{
    PLUGIN_SAVEVARS();
    Log_Msg(LOG_CS2S, "Hello, world!\n");

    return true;
}

// Called when the plugin is unloaded by Metamod. You can manually unload a
// plugin with `meta unload addons/cs2s-plugin`.
bool Plugin::Unload(char *error, size_t maxlen)
{
    ConVar_Unregister();
    Log_Msg(LOG_CS2S, "Goodbye, world!\n");

    return true;
}
