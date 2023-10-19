#pragma once

#ifdef WIN32
#include <windef.h>
#else
#include <metamod_oslink.h>
#endif

struct dlinfo_t
{
    void* address{nullptr};
    size_t size{0};
};

int dlinfo(HINSTANCE module, dlinfo_t* info);
