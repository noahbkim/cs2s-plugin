#include "plugin.h"

#include <cstdio>

#include <Color.h>

//#include <cstrike15_usermessages.pb.h>
#include <usermessages.pb.h>
#include <cs_gameevents.pb.h>
//#include <gameevents.pb.h>
//#include <te.pb.h>

#include <cs2s/plugin/detour.h>
#include <cs2s/plugin/library.h>
#include <cs2s/sdk/server/recipientfilter.h>

#define STR_LITERAL(D) #D
#define STR(D) STR_LITERAL(D)

// TODO: figure out a cleaner way to declare this
#ifdef WIN32
const uint8_t UTIL_CLIENT_PRINT_ALL_PATTERN[] = {0x48, 0x89, 0x5c, 0x24, 0x8, 0x48, 0x89, 0x6c, 0x24, 0x10, 0x48, 0x89, 0x74, 0x24, 0x18, 0x57, 0x48, 0x81, 0xec, 0x70, 0x1, 0x2a, 0x2a, 0x8b, 0xe9};
#else
const uint8_t UTIL_CLIENT_PRINT_ALL_PATTERN[] = {0x55, 0x48, 0x89, 0xe5, 0x41, 0x57, 0x49, 0x89, 0xd7, 0x41, 0x56, 0x49, 0x89, 0xf6, 0x41, 0x55, 0x41, 0x89, 0xfd};
#endif

// Declare hook for requested DLL function
cs2s::plugin::Pattern<decltype(UTIL_ClientPrintAll)> UTIL_ClientPrintAllPattern{
    UTIL_CLIENT_PRINT_ALL_PATTERN,
    sizeof(UTIL_CLIENT_PRINT_ALL_PATTERN),
};

// Set up Source 2 logging. Provides macros like `Log_Msg`, `Log_Warning`, and
// `Log_Error`. See tier0/logging.h for full documentation. You can change the
// color of this channel by adding your own RGB values to the `Color()`.
DEFINE_LOGGING_CHANNEL_NO_TAGS(LOG_CS2S, STR(CMAKE_PROJECT_NAME), 0, LV_MAX, Color());

// A static instance of the plugin loaded by Metamod.
Plugin g_Plugin(LOG_CS2S);
PLUGIN_EXPOSE(Plugin, g_Plugin);

// Declare Metamod hooks; must come after PLUGIN_EXPOSE as it depends on globals.
SH_DECL_HOOK8_void(
    IGameEventSystem,
    PostEventAbstract,
    SH_NOATTRIB,
    0,
    CSplitScreenSlot,
    bool,
    int,
    const uint64*,
    INetworkSerializable*,
    const void*,
    unsigned long,
    NetChannelBufType_t
);

Plugin::Plugin(LoggingChannelID_t log)
    : log(log)
    , libraries(log)
    , detours(log)
{
}

// Called when the plugin is loaded by Metamod. You can test this by running
// `meta load addons/cs2s-plugin` (interpolating the actual generated plugin
// path and name) in your server console.
bool Plugin::Load(PluginId id, ISmmAPI* ismm, char* error, size_t maxlen, bool late)
{
    this->metamod = ismm;

    // Set globals defined by `PLUGIN_EXPOSE` and used by things like
    // `SH_DECL_HOOK8_void`.
    PLUGIN_SAVEVARS();

    Log_Msg(this->log, "[CS2S] Hello, world!\n");
    if (!PluginService::LoadAll(id, ismm, error, maxlen, late, this->libraries, this->detours))
    {
        return false;
    }

    cs2s::plugin::Library server_library;
    if (!this->libraries.Resolve(GAME_BIN_DIRECTORY, "server", &server_library))
    {
        return false;
    }

    this->client_print_all = server_library.Match(UTIL_ClientPrintAllPattern);
    if (!this->client_print_all)
    {
        Log_Error(this->log, "[CS2S] Did not find UTIL_ClientPrintAll!\n");
        return false;
    }
    Log_Msg(this->log, "[CS2S] Found UTIL_ClientPrintAll at %p\n", this->client_print_all);

    // Request interfaces
    GET_V_IFACE_CURRENT(GetEngineFactory, this->engine_server_2, IVEngineServer2, SOURCE2ENGINETOSERVER_INTERFACE_VERSION);
    GET_V_IFACE_CURRENT(GetEngineFactory, this->con_vars, ICvar, CVAR_INTERFACE_VERSION);
    GET_V_IFACE_ANY(GetServerFactory, this->source_2_server, ISource2Server, SOURCE2SERVER_INTERFACE_VERSION);
    GET_V_IFACE_ANY(GetServerFactory, this->source_2_game_entities, ISource2GameEntities, SOURCE2GAMEENTITIES_INTERFACE_VERSION);
    GET_V_IFACE_ANY(GetServerFactory, this->server_game_clients, IServerGameClients, SOURCE2GAMECLIENTS_INTERFACE_VERSION);
    GET_V_IFACE_ANY(GetEngineFactory, this->network_server_service, INetworkServerService, NETWORKSERVERSERVICE_INTERFACE_VERSION);
    GET_V_IFACE_ANY(GetEngineFactory, this->game_event_system, IGameEventSystem, GAMEEVENTSYSTEM_INTERFACE_VERSION);

    // Bind events
    SH_ADD_HOOK_MEMFUNC(IGameEventSystem, PostEventAbstract, this->game_event_system, this, &Plugin::PostEventAbstract, false);

    return true;
}

#define HUD_PRINTNOTIFY		1
#define HUD_PRINTCONSOLE	2
#define HUD_PRINTTALK		3
#define HUD_PRINTCENTER		4

void Plugin::PostEventAbstract(
    CSplitScreenSlot nSlot,
    bool bLocalOnly,
    int nClientCount,
    const uint64* clients,
    INetworkSerializable* pEvent,
    const void* pData,
    unsigned long nSize,
    NetChannelBufType_t bufType
)
{
    NetMessageInfo_t* info = pEvent->GetNetMessageInfo();
    if (info->m_MessageId == GE_FireBulletsId)
    {
        this->client_print_all(HUD_PRINTTALK, "[CS2S] Fire!\n", nullptr, nullptr, nullptr, nullptr);
//        Log_Msg(this->log, "[CS2S] Fire!\n");
    }
}

// Called when the plugin is unloaded by Metamod. You can manually unload a
// plugin with `meta unload addons/cs2s-plugin`.
bool Plugin::Unload(char* error, size_t maxlen)
{
    ConVar_Unregister();
    Log_Msg(this->log, "[CS2S] Goodbye, world!\n");

    // Remove hooks.
    SH_REMOVE_HOOK_MEMFUNC(IGameEventSystem, PostEventAbstract, this->game_event_system, this, &Plugin::PostEventAbstract, false);

    // Every service should get a chance to unload.
    return PluginService::UnloadAll(error, maxlen, this->libraries, this->detours);
}
