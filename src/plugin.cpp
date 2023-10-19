#include <cstdio>

#include "plugin.h"

Plugin g_Plugin;
PLUGIN_EXPOSE(Plugin, g_Plugin);

bool Plugin::Load(PluginId id, ISmmAPI* ismm, char* error, size_t maxlen, bool late)
{
    PLUGIN_SAVEVARS();
    printf("Hello, world!\n");

    return true;
}

bool Plugin::Unload(char *error, size_t maxlen)
{
    ConVar_Unregister();
    printf("Goodbye, world!\n");

    return true;
}
