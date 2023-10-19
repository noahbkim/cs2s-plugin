#pragma once

#include <ISmmPlugin.h>

class Plugin final : public ISmmPlugin, public IMetamodListener
{
public:
    bool Load(PluginId id, ISmmAPI* ismm, char* error, size_t maxlen, bool late) override;
    bool Unload(char* error, size_t maxlen) override;

    const char *GetAuthor() override { return "Noah Kim"; }
    const char *GetName() override { return "Plugin Template"; }
    const char *GetDescription() override { return "A CS2 server plugin template."; }
    const char *GetURL() override { return "https://github.com/noahbkim/cs2s-plugin"; }
    const char *GetLicense() override { return "MIT"; };
    const char *GetVersion() override { return "0.1"; };
    const char *GetDate() override { return "2023-10-18"; };
    const char *GetLogTag() override { return "cs2s-plugin"; };
};
