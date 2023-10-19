#pragma once

#include <cstdint>
#include <functional>
#include <memory>

#include <funchook.h>

namespace cs2s::detour
{

// The virtual destructor of `Detour` subclasses should unload the detour. This
// allows trivial RAII management.
class PluginDetour
{
public:
    virtual ~PluginDetour() = default;
};

template<typename T>
class PluginFunctionDetour : public PluginDetour
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

    // Constructor is private so you have to go through a manager.
    PluginFunctionDetour(T* function, const char* detour_name)
        : function(function)
        , detour_name(detour_name)
    {}
};

class DetourManager
{
private:

};

}
