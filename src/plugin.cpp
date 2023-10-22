#include "plugin.h"

#include <string>

#include <Color.h>

#include <cs2s/common/macro.h>

#define LOG_PREFIX "[" STR(PLUGIN_NAME) "] "

// Set up Source 2 logging. Provides macros like `Log_Msg`, `Log_Warning`, and
// `Log_Error`. See tier0/logging.h for full documentation. You can change the
// color of this channel by adding your own RGB values to the `Color()`.
DEFINE_LOGGING_CHANNEL_NO_TAGS(LOG_CS2S, STR(CS2S_PLUGIN_NAME), 0, LV_MAX, Color());

// A static instance of the plugin loaded by Metamod.
Plugin g_Plugin(LOG_CS2S);
PL_EXPOSURE_FUNC(Plugin, g_Plugin);

Plugin::Plugin(LoggingChannelID_t log)
    : log(log)
    , libraries(log)
    , events(log)
{
}

// Called when the plugin is loaded by Metamod. You can test this by running
// `meta load addons/cs2s-plugin` (interpolating the actual generated plugin
// path and name) in your server console.
bool Plugin::Load(PluginId id, ISmmAPI* ismm, char* error, size_t maxlen, bool late)
{
    this->metamod = ismm;

    if (!this->libraries.Load(id, ismm, late))
    {
        ismm->Format(error, maxlen, "failed to load library service");
        return false;
    }

    if (!this->events.Load(id, ismm, late))
    {
        ismm->Format(error, maxlen, "failed to load events service");
        return false;
    }

    cs2s::plugin::service::Library server_library;
    if (!this->libraries.Resolve(GAME_BIN_DIRECTORY, "server", &server_library))
    {
        ismm->Format(error, maxlen, "failed to resolve server library");
        return false;
    }

    return true;
}

// Called when the plugin is unloaded by Metamod. You can manually unload a
// plugin with `meta unload addons/cs2s-plugin`.
bool Plugin::Unload(char* error, size_t maxlen)
{
    this->libraries.Unload();
    this->events.Unload();

    return true;
}
