#pragma once

// Adapted from https://github.com/Source2ZE/CS2Fixes/blob/29a699a79936f00b77b9a51fd0e988dc9f04a3d0/src/utils/virtual.h#L26

#include <utility>

namespace cs2s::common
{

template<size_t Index, typename T, typename ...Args>
constexpr T vcall(void* self, Args&&... args)
{
    void** vtable = *reinterpret_cast<void***>(self);
    return reinterpret_cast<T(*)(void*, Args...)>(vtable[Index])(self, std::forward(args)...);
}

}
