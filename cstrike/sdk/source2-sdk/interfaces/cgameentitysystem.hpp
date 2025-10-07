#pragma once

#include "../../memory/memory.hpp"
#include "../../virtual.hpp"

#include "../entity/ccsplayercontroller.hpp"
#include "../entity/c_baseviewmodel.hpp"
#include "../entity/c_chicken.hpp"

class C_BaseEntity;

class CGameEntitySystem {
   public:
    static CGameEntitySystem* GetInstance();
    static CCSPlayerController* GetLocalPlayerController();

    template <typename T = C_BaseEntity>
    T* GetByIndex(CHandle nIndex)
    {
        return reinterpret_cast<T*>(this->GetEntityByIndex(nIndex.GetEntryIndex()));
    }

    static CGameEntitySystem& Get() {
        static CGameEntitySystem inst;
        return inst;
    }
    template <typename T = C_BaseEntity>
    T* GetBaseEntity(int index) {
        if (!memory::fnGetBaseEntity) return nullptr;
        return (T*)(memory::fnGetBaseEntity(this, index));
    }

    void* GetEntityByIndex(int nIndex)
    {
        //@ida: #STR: "(missing),", "(missing)", "Ent %3d: %s class %s name %s\n" | or find "cl_showents" cvar -> look for callback
        //	do { pEntity = GetBaseEntityByIndex(g_pGameEntitySystem, nCurrentIndex); ... }
        return memory::fnGetBaseEntity(this, nIndex);
    }

    int GetHighestEntityIndex() {
        int highestIdx = -1;
        if (!memory::fnGetHighestEntityIndex) return highestIdx;

        memory::fnGetHighestEntityIndex(this, &highestIdx);
        return highestIdx;
    }
    //template <typename T = C_BaseEntity>
    //T* Get(const CHandle hHandle) {
    //    if (!hHandle.IsValid()) return nullptr;

    //    return reinterpret_cast<T*>(
    //        this->GetBaseEntity(hHandle.GetEntryIndex()));
    //}
};
