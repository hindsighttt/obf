#pragma once

#include "../../../core/schema.h"

#include "../econ/ceconitemdefinition.hpp"

class CEconItem;

class C_EconItemView {
   public:
    CEconItem* GetSOCData();

    auto GetCustomPaintKitIndex() { return CALL_VIRTUAL(int, 2, this); }
    auto GetStaticData() {
        return CALL_VIRTUAL(CEconItemDefinition*, 13, this);
    }

    SCHEMA_ADD_FIELD(uint16_t, GetItemDefinitionIndex, "C_EconItemView->m_iItemDefinitionIndex");
    SCHEMA_ADD_FIELD(uint64_t, GetItemID, "C_EconItemView->m_iItemID");
    SCHEMA_ADD_FIELD(uint32_t, GetItemIDLow, "C_EconItemView->m_iItemIDLow");
    SCHEMA_ADD_FIELD(uint32_t, GetItemIDHigh, "C_EconItemView->m_iItemIDHigh");
    SCHEMA_ADD_FIELD(uint32_t, GetAccountID, "C_EconItemView->m_iAccountID");
    SCHEMA_ADD_FIELD(bool, GetDisallowSOC, "C_EconItemView->m_bDisallowSOC");
};
