#pragma once
#include "../../math/math.hpp"
#include "../../schema/schema.hpp"

#include "../types/cnetworkutlvectorbase.hpp"
#include "../types/chandle.hpp"

class CPlayer_MovementServices {
   public:
    SCHEMA_FIELD(m_vecOldViewAngles, "CPlayer_MovementServices", "m_vecOldViewAngles", QAngle_t);
    SCHEMA_FIELD(m_flMaxspeed, "CPlayer_MovementServices", "m_flMaxspeed",
                 float);
};
