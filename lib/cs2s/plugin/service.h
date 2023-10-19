#pragma once

#include <tier0/logging.h>

#include <ISmmPlugin.h>

class Service
{
protected:
    LoggingChannelID_t log;

public:
    explicit Service(LoggingChannelID_t log) : log(log) {}
    virtual ~Service() = default;

public:
    virtual bool Load(PluginId id, ISmmAPI* ismm, char* error, size_t maxlen, bool late)
    {
        return true;
    }

    virtual bool Unload(char* error, size_t maxlen)
    {
        return true;
    }
};
