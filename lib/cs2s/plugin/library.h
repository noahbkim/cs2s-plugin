#pragma once

#include <string>
#include <utility>

#include <absl/container/flat_hash_map.h>

#include <ISmmPlugin.h>
#include <metamod_oslink.h>

#include "service.h"

#ifdef WIN32
#define ROOT_BIN_DIRECTORY "/bin/win64"
#define GAME_BIN_DIRECTORY "/csgo/bin/win64"
#define LIBRARY_PREFIX ""
#define LIBRARY_SUFFIX ".dll"
#else
#define ROOT_BIN_DIRECTORY "/bin/linuxsteamrt64/"
#define GAME_BIN_DIRECTORY "/csgo/bin/linuxsteamrt64/"
#define LIBRARY_PREFIX "lib"
#define LIBRARY_SUFFIX ".so"
#endif

// Adapted heavily from https://github.com/Source2ZE/CS2Fixes/blob/main/src/gameconfig.h
namespace cs2s::plugin
{

// Wraps introspection for shared libraries
struct Library
{
    std::string path;
    std::string name;
    HINSTANCE handle;
    void* address;
    size_t size;
};

class LibraryService : public Service
{
protected:
    ISmmAPI* metamod{nullptr};
    IVEngineServer2* server{nullptr};

    // Cached
    std::string game_directory;
    absl::flat_hash_map<std::string, Library> libraries;

public:
    explicit LibraryService(LoggingChannelID_t log);

public:
    bool Load(PluginId id, ISmmAPI* ismm, char* error, size_t maxlen, bool late) override;
    bool Unload(char* error, size_t maxlen) override;

public:
    virtual Library* Resolve(const std::string& subpath, std::string name);
};

}
