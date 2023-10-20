#pragma once

#include <tier0/logging.h>

#include <ISmmPlugin.h>

class PluginService
{
protected:
    LoggingChannelID_t log;

public:
    explicit PluginService(LoggingChannelID_t log) : log(log) {}
    virtual ~PluginService() = default;

public:
    virtual bool Load(PluginId id, ISmmAPI* ismm, char* error, size_t maxlen, bool late)
    {
        return true;
    }

    virtual bool Unload(char* error, size_t maxlen)
    {
        return true;
    }

// Convenience methods.
public:
    template<typename T>
    static bool LoadAll(PluginId id, ISmmAPI* ismm, char* error, size_t maxlen, bool late, T& plugin)
    {
        return plugin.Load(id, ismm, error, maxlen, late);
    }

    template<typename T, typename ...U>
    static bool LoadAll(PluginId id, ISmmAPI* ismm, char* error, size_t maxlen, bool late, T& plugin, U& ...rest)
    {
        if (!plugin.Load(id, ismm, error, maxlen, late))
        {
            return false;
        }

        if (!PluginService::LoadAll(id, ismm, error, maxlen, late, rest...))
        {
            plugin.Unload(error, maxlen);
            return false;
        }

        return true;
    }

    template<typename T>
    static bool UnloadAll(char* error, size_t maxlen, T& plugin)
    {
        return plugin.Unload(error, maxlen);
    }

    template<typename T, typename ...U>
    static bool UnloadAll(char* error, size_t maxlen, T& plugin, U& ...rest)
    {
        bool p = plugin.Unload(error, maxlen);
        bool r = PluginService::UnloadAll(error, maxlen, rest...);
        return p && r;
    }
};
