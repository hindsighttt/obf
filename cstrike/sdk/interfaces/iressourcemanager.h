#pragma once
#include "../../utilities/memory.h"
#include "../../core/sdk.h"
#include "../datatypes/cbufferstring.h"
#include "../../core/schema.h"

class IRessourceManager {
public:
    void* PreCache(const char* name)
    {
        CBufferString names = CBufferString(name);
        return vmt::call_virtual<void*>(this, 48, &names, "");
    }
};