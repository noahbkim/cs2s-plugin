#pragma once

#include <engine/igameeventsystem.h>
#include <igameevents.h>

#include <cs2s/plugin/service/library.h>

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

extern cs2s::plugin::service::Pattern<decltype(UTIL_ClientPrintAll)> UTIL_ClientPrintAllPattern;

extern GameEventKeySymbol_t attacker_symbol;
extern GameEventKeySymbol_t userid_symbol;
extern GameEventKeySymbol_t name_symbol;
extern GameEventKeySymbol_t steamid_symbol;
extern GameEventKeySymbol_t bot_symbol;
