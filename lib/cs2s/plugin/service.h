#pragma once

#include <exception>

#include <tier0/logging.h>

#include <ISmmPlugin.h>

namespace cs2s::plugin
{

class PluginService
{
protected:
    LoggingChannelID_t log;

public:
    explicit PluginService(LoggingChannelID_t log)
        : log(log)
    {
    }

    virtual ~PluginService() = default;

public:
    virtual bool Load(PluginId id, ISmmAPI* ismm, bool late)
    {
        return true;
    }

    virtual bool Unload()
    {
        return true;
    }
};

}
