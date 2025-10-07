#pragma once

#include "../../schema/schema.hpp"
#include "../../math/math.hpp"

class CCollisionProperty
{
public:
	std::uint16_t CollisionMask( )
	{
		return *reinterpret_cast<std::uint16_t*>(reinterpret_cast<std::uintptr_t>(this) + 0x38);
	}

	SCHEMA_FIELD(GetMins, "CCollisionProperty", "m_vecMins", Vector);
	SCHEMA_FIELD(GetMaxs, "CCollisionProperty", "m_vecMaxs", Vector);
	SCHEMA_FIELD(GetSolidFlags, "CCollisionProperty", "m_usSolidFlags", uint8_t);
	SCHEMA_FIELD(GetCollisionGroup, "CCollisionProperty", "m_CollisionGroup", uint8_t);
};
