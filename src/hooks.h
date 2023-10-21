#pragma once

#include <engine/igameeventsystem.h>
#include <igameevents.h>

#include <cs2s/plugin/library.h>

#define HUD_PRINTNOTIFY		1
#define HUD_PRINTCONSOLE	2
#define HUD_PRINTTALK		3
#define HUD_PRINTCENTER		4

void UTIL_ClientPrintAll(
    int msg_dest,
    const char *msg_name,
    const char *param1 = NULL,
    const char *param2 = NULL,
    const char *param3 = NULL,
    const char *param4 = NULL
);

extern cs2s::plugin::Pattern<decltype(UTIL_ClientPrintAll)> UTIL_ClientPrintAllPattern;

extern GameEventKeySymbol_t attacker_symbol;
extern GameEventKeySymbol_t userid_symbol;
