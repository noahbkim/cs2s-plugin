#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

#include <funchook.h>

#include "cs2s/plugin/service.h"

namespace cs2s::plugin::service
{

// The virtual destructor of `Detour` subclasses should unload the detour. This
// allows trivial RAII management.
class Detour
{
public:
    virtual ~Detour() = default;
};

template<typename T>
class FunctionDetour : public Detour
{
protected:
    T* function;  // Local function pointer we're detouring to

    // Existing function we're hooking into
    T* detour;
    const char* detour_name{nullptr};  // Name of the function we're detouring from in the shared library
    const char* detour_symbol{nullptr};
    const uint8_t* detour_signature{nullptr};  //

    funchook_t* hook{nullptr};  // Instantiated funchook

    // State: whether this detour is installed.
    bool installed{false};

    // Constructor is private, so you have to go through a manager.
    FunctionDetour(T* function, const char* detour_name)
        : function(function)
        , detour_name(detour_name)
    {}
};

class PluginDetourService : public PluginService
{
private:
    std::vector<std::unique_ptr<Detour>> detours;

public:
    using PluginService::PluginService;

public:
    bool Load(PluginId id, ISmmAPI* ismm, bool late) override;
    bool Unload() override;
};

}
