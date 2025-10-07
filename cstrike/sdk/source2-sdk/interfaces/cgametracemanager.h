#pragma once
#include <array>

#include "../../memory/memory.hpp"
#include "../../virtual.hpp"
#include "../../math/math.hpp"

struct Ray_t
{
public:
	Vector m_vecStart;
	Vector m_vecEnd;
	Vector m_vecMins;
	Vector m_vecMaxs;
	char pad[0x4];
	std::uint8_t UnkType;
};
static_assert(sizeof(Ray_t) == 0x38);

struct SurfaceData_t
{
public:
	char pad[0x8];
	float m_flPenetrationModifier;
	float m_flDamageModifier;
	char pad2[0x4];
	int m_iMaterial;
};

static_assert(sizeof(SurfaceData_t) == 0x18);

struct TraceHitboxData_t
{
public:
	char pad[0x38];
	int m_nHitGroup;
	char pad2[0x4];
	int m_nHitboxId;
};
static_assert(sizeof(TraceHitboxData_t) == 0x44);

class C_CSPlayerPawn;
struct GameTrace_t
{
public:
	GameTrace_t( ) = default;

	SurfaceData_t* GetSurfaceData( );
	int GetHitboxId( );
	int GetHitgroup( );
	bool IsVisible( ) const;

	void* m_pSurface;
	C_CSPlayerPawn* m_pHitEntity;
	TraceHitboxData_t* m_pHitboxData;
	char pad2[0x38];
	std::uint32_t m_uContents;
	char pad3[0x24];
	Vector m_vecStartPos;
	Vector m_vecEndPos;
	Vector m_vecNormal;
	Vector m_vecPosition;
	char pad4[0x4];
	float m_flFraction;
	char pad5[0x6];
	bool m_bAllSolid;
	char pad6[0x4D];
}; // Size: 0x108

static_assert(sizeof(GameTrace_t) == 0x108);

struct TraceFilter_t
{
public:
	char _pad[0x8];
	std::int64_t m_uTraceMask;
	std::array<std::int64_t, 2> m_v1;
	std::array<std::int32_t, 4> m_arrSkipHandles;
	std::array<std::int16_t, 2> m_arrCollisions;
	std::int16_t m_v2;
	std::uint8_t m_v3;
	std::uint8_t m_v4;
	std::uint8_t m_v5;

	TraceFilter_t( ) = default;
	TraceFilter_t(std::uint64_t uMask, C_CSPlayerPawn* pSkip1, C_CSPlayerPawn* pSkip2, int nLayer);
};
static_assert(sizeof(TraceFilter_t) == 0x40);

class CGameTraceManager
{
public:
	bool TraceShape(Ray_t* pRay, Vector& vecStart, Vector& vecEnd, TraceFilter_t* pFilter, GameTrace_t* pGameTrace)
	{
		return memory::fnTraceShape(this, pRay, &vecStart, &vecEnd, pFilter, pGameTrace);
	}

	bool TraceShape2(Ray_t* pRay, Vector vecStart, Vector vecEnd, TraceFilter_t* pFilter, GameTrace_t* pGameTrace)
	{
		return memory::fnTraceShape(this, pRay, &vecStart, &vecEnd, pFilter, pGameTrace);
	}

	bool ClipRayToEntity(Ray_t* pRay, Vector& vecStart, Vector& vecEnd, C_CSPlayerPawn* pPawn, TraceFilter_t* pFilter, GameTrace_t* pGameTrace)
	{
		return memory::fnClipRayToEntity(this, pRay, &vecStart, &vecEnd, pPawn, pFilter, pGameTrace);
	}
};
