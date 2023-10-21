#include "plugin.h"

#include <string>

#include <Color.h>

#include <cs2s/common/macro.h>

#define LOG_PREFIX "[" STR(PLUGIN_NAME) "] "

// Set up Source 2 logging. Provides macros like `Log_Msg`, `Log_Warning`, and
// `Log_Error`. See tier0/logging.h for full documentation. You can change the
// color of this channel by adding your own RGB values to the `Color()`.
DEFINE_LOGGING_CHANNEL_NO_TAGS(LOG_CS2S, STR(CS2S_PLUGIN_NAME), 0, LV_MAX, Color(0, 0, 255));

// A static instance of the plugin loaded by Metamod.
Plugin g_Plugin(LOG_CS2S);
PL_EXPOSURE_FUNC(Plugin, g_Plugin);

Plugin::Plugin(LoggingChannelID_t log)
    : log(log)
    , libraries(log)
    , events(log)
    , players(ABSOLUTE_PLAYER_LIMIT)
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

    this->client_print_all = server_library.Match(UTIL_ClientPrintAllPattern);
    if (!this->client_print_all)
    {
        ismm->Format(error, maxlen, "failed to locate UTIL_ClientPrintAllPattern");
        return false;
    }

    this->events.Subscribe("player_connect", this);
    this->events.Subscribe("player_disconnect", this);
    this->events.Subscribe("player_info", this);
    this->events.Subscribe("player_death", this);

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

void Plugin::FireGameEvent(IGameEvent *event)
{
    switch (event->GetID())
    {
    case 8:
        this->PlayerConnect(event);
        break;
    case 9:
        this->PlayerDisconnect(event);
        break;
    case 10:  // TODO: verify
        this->PlayerInfo(event);
        break;
    case 53:  // player_death
        this->PlayerDeath(event);
        break;
    }
}

void Plugin::PlayerConnect(IGameEvent* event)
{
    int player_slot = event->GetInt(userid_symbol) % ABSOLUTE_PLAYER_LIMIT;
    if (this->players[player_slot].first)
    {
        // TODO
        Log_Warning(this->log, LOG_PREFIX "player_connect: slot %d still connected, removing\n", player_slot);
    }

    // Set this slot as connected
    this->players[player_slot].first = true;

    // Overwrite player information
    Player& player = this->players[player_slot].second;
    player.name = event->GetString(name_symbol);
    player.steam_id = event->GetUint64(steamid_symbol);
    player.bot = event->GetBool(bot_symbol);

    Log_Msg(this->log, LOG_PREFIX "Connected player %d as %s\n", player_slot, player.name.c_str());
}

void Plugin::PlayerInfo(IGameEvent* event)
{
    int player_slot = event->GetInt(userid_symbol) % ABSOLUTE_PLAYER_LIMIT;
    if (!this->players[player_slot].first)
    {
        Log_Warning(this->log, LOG_PREFIX "player_info: slot %d disconnected, updating anyway\n", player_slot);
    }

    // Update with new data
    Player& player = this->players[player_slot].second;
    player.name = event->GetString(name_symbol);
    player.steam_id = event->GetUint64(steamid_symbol);
    player.bot = event->GetBool(bot_symbol);

    Log_Msg(this->log, LOG_PREFIX "Updated player %d as %s\n", player_slot, player.name.c_str());
}

void Plugin::PlayerDisconnect(IGameEvent* event)
{
    int player_slot = event->GetInt(userid_symbol) % ABSOLUTE_PLAYER_LIMIT;
    if (!this->players[player_slot].first)
    {
        Log_Warning(this->log, LOG_PREFIX "player_disconnect: slot %d already disconnected, skipping\n", player_slot);
        return;
    }

    // Reset values for debugging and to keep memory profile low (probably unnecessary)
    Player& player = this->players[player_slot].second;
    player.name.clear();
    player.steam_id = 0;
    player.bot = false;
    player.rating = 0;
}

void Plugin::PlayerDeath(IGameEvent* event)
{
    int player_slot = event->GetInt(userid_symbol) % ABSOLUTE_PLAYER_LIMIT;
    if (!this->players[player_slot].first)
    {
        Log_Warning(this->log, LOG_PREFIX "player_death: slot %d disconnected, continuing\n", player_slot);
    }

    int attacker_player_slot = event->GetInt(attacker_symbol) % ABSOLUTE_PLAYER_LIMIT;
    if (!this->players[attacker_player_slot].first)
    {
        Log_Warning(this->log, LOG_PREFIX "player_death: attacker slot %d disconnected, continuing\n", attacker_player_slot);
    }

    const Player& player = this->players[player_slot].second;
    const Player& attacker_player = this->players[attacker_player_slot].second;

    std::string buffer = LOG_PREFIX " " + attacker_player.name + " killed " + player.name + "\n";
    this->client_print_all(HUD_PRINTTALK, buffer.c_str(), nullptr, nullptr, nullptr, nullptr);
}
