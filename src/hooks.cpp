#include "hooks.h"

// TODO: figure out a cleaner way to declare this
#ifdef WIN32
const uint8_t UTIL_CLIENT_PRINT_ALL_PATTERN[] = {0x48, 0x89, 0x5c, 0x24, 0x8, 0x48, 0x89, 0x6c, 0x24, 0x10, 0x48, 0x89, 0x74, 0x24, 0x18, 0x57, 0x48, 0x81, 0xec, 0x70, 0x1, 0x2a, 0x2a, 0x8b, 0xe9};
#else
const uint8_t UTIL_CLIENT_PRINT_ALL_PATTERN[] = {0x55, 0x48, 0x89, 0xe5, 0x41, 0x57, 0x49, 0x89, 0xd7, 0x41, 0x56, 0x49, 0x89, 0xf6, 0x41, 0x55, 0x41, 0x89, 0xfd};
#endif

// Declare hook for requested DLL function
cs2s::plugin::service::Pattern<decltype(UTIL_ClientPrintAll)> UTIL_ClientPrintAllPattern{
    UTIL_CLIENT_PRINT_ALL_PATTERN,
    sizeof(UTIL_CLIENT_PRINT_ALL_PATTERN),
};

GameEventKeySymbol_t attacker_symbol("attacker");
GameEventKeySymbol_t userid_symbol("userid");
GameEventKeySymbol_t name_symbol("name");
GameEventKeySymbol_t steamid_symbol("steamid");
GameEventKeySymbol_t bot_symbol("bot");
