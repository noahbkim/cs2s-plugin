#include "library.h"

#include <platform.h>

#include "cs2s/common/shared.h"

namespace cs2s::plugin
{

void* Library::Match(const uint8_t* pattern_data, size_t pattern_size) const
{
    auto blob = reinterpret_cast<uint8_t*>(this->address);
    for (size_t blob_cursor{0}; blob_cursor + pattern_size < this->size; ++blob_cursor)
    {
        for (size_t pattern_cursor{0}; pattern_cursor < pattern_size; ++pattern_cursor)
        {
            if (
                blob[blob_cursor + pattern_cursor] != pattern_data[pattern_cursor]
                || pattern_data[pattern_cursor] == 0x2A  // Wildcard
            )
            {
                goto blob_next;
            }
        }

        // If we get through the entire loop, we matched, so return!
        return reinterpret_cast<void*>(blob + blob_cursor);

        // If we found unequal bytes, jump to here, effectively `continue`ing the outer loop
        blob_next:;
    }

    return nullptr;
}

bool PluginLibraryService::Load(PluginId id, ISmmAPI* ismm, char* error, size_t maxlen, bool late)
{
    this->metamod = ismm;
    this->game_directory = Plat_GetGameDirectory();
    Log_Msg(this->log, "[CS2S:libraries] Loaded library service\n");
    return true;
}

bool PluginLibraryService::Unload(char* error, size_t maxlen)
{
    Log_Msg(this->log, "[CS2S:libraries] Unloaded library service\n");
    return true;
}

bool PluginLibraryService::Resolve(const std::string& subpath, std::string name, Library* library)
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
        Log_Error(this->log, "[CS2S:libraries] Failed to resolve %s, tried %s\n", name.c_str(), path.c_str());
        return false;
    }

    // Determine the base point and size
    cs2s::common::dlinfo_t info;
    int error = cs2s::common::dlinfo(handle, &info);
    if (error != 0)
    {
        Log_Error(this->log, "[CS2S:libraries] Failed to inspect %s, received error code %d\n", path.c_str(), error);
        return false;
    }

    // Return as complete struct
    Log_Msg(this->log, "[CS2S:libraries] Resolved %s to %p\n", path.c_str(), handle);
    library->path = std::move(path);
    library->name = std::move(name);
    library->handle = handle;
    library->address = info.address;
    library->size = info.size;
    return true;
}

}
