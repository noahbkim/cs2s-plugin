#include "plugin.h"

#include <string>

#include <Color.h>

#include <cs2s/macro.h>

#define LOG_PREFIX "[" STR(PLUGIN_NAME) "] "

// Set up Source 2 logging. Provides macros like `Log_Msg`, `Log_Warning`, and
// `Log_Error`. See tier0/logging.h for full documentation. You can change the
// color of this channel by adding your own RGB values to the `Color()`.
DEFINE_LOGGING_CHANNEL_NO_TAGS(LOG_CS2S, STR(PLUGIN_NAME), 0, LV_MAX, Color());

// A static instance of the plugin loaded by Metamod. Note the use of
// `PLUGIN_EXPOSE`, which also declares a bunch of global variables for
// tracking parts of the server interface.
Plugin g_Plugin(LOG_CS2S);
PLUGIN_EXPOSE(Plugin, g_Plugin);

Plugin::Plugin(LoggingChannelID_t log)
    : log(log)
    , events(log)
    , messages(log)
{
}

// Called when the plugin is loaded by Metamod. You can test this by running
// `meta load addons/cs2s-plugin` (interpolating the actual generated plugin
// path and name) in your server console.
bool Plugin::Load(PluginId id, ISmmAPI* ismm, char* error, size_t maxlen, bool late)
{
    // The global variables assigned here are declared by `PLUGIN_EXPOSE` and
    // used by a variety of metamod macros. I do not recommend removing it
    // unless you know what you're doing.
    PLUGIN_SAVEVARS();

    this->metamod = ismm;

    if (!this->events.Load(id, ismm, late))
    {
        ismm->Format(error, maxlen, "failed to load events service");
        return false;
    }

    if (!this->messages.Load(id, ismm, late))
    {
        ismm->Format(error, maxlen, "failed to load messages service");
        return false;
    }

    // Subscribe to the `weapon_fire` game event. Note that `Plugin` must
    // inherit from `IGameEventListener2` (and implement `FireGameEvent`) for
    // this to work.
    this->events.Subscribe("weapon_fire", this);

    return true;
}

// Called when the plugin is unloaded by Metamod. You can manually unload a
// plugin with `meta unload addons/cs2s-plugin`.
bool Plugin::Unload(char* error, size_t maxlen)
{
    // Automatically unsubscribes any event handlers we added.
    this->events.Unload();

    this->messages.Unload();

    return true;
}

// This function is called when a player fires a weapon on our server. Event
// subscription is handled by the call to `this->events.Subscribe` in the
// `Plugin::Load` method above.
void Plugin::FireGameEvent(IGameEvent* event)
{
    if (event->GetID() == 158)
    {
        int player_slot = event->GetPlayerSlot("userid").Get();
        this->messages.Print(HUD_PRINTTALK, "weapon_fire from player %d\n", player_slot);
    }
}
