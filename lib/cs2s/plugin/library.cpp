#include "library.h"

#include <platform.h>
#include <interface.h>

#include "shared.h"

namespace cs2s::plugin
{

LibraryService::LibraryService(LoggingChannelID_t log)
    : Service(log)
{

}

bool LibraryService::Load(PluginId id, ISmmAPI* ismm, char* error, size_t maxlen, bool late)
{
    this->metamod = ismm;
    this->game_directory = Plat_GetGameDirectory();
    return true;
}

bool LibraryService::Unload(char* error, size_t maxlen)
{
    return true;
}

Library* LibraryService::Resolve(const std::string& subpath, std::string name)
{
    // TODO: optimize
    std::string path = this->game_directory;
    path.append(subpath);
    path.append(LIBRARY_PREFIX);
    path.append(name);
    path.append(LIBRARY_SUFFIX);

    // Find the loaded DLL
    HINSTANCE handle = dlmount(path.c_str());
    if (!handle)
    {
        Log_Error(this->log, "Failed to load %s/%s, tried %s\n", subpath.c_str(), name.c_str(), path.c_str());
        return nullptr;
    }

    // Determine the base point and size
    dlinfo_t info;
    int error = dlinfo(handle, &info);
    if (error != 0)
    {
        Log_Error(this->log, "Failed to inspect %s, received error code %d\n", path.c_str(), error);
        return nullptr;
    }

    // Return as complete struct
    Log_Msg(this->log, "Resolved %s to %p\n", name.c_str(), handle);
    this->libraries.emplace(
        name,
        Library{
            .path = path,
            .name = std::move(name),
            .handle = handle,
            .address = info.address,
            .size = info.size
        }
    );
}

}
