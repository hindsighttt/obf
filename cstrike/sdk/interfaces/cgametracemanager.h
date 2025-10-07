#pragma once
// used: pad and findpattern
#include "../../utilities/memory.h"
// used: vector
#include "../../sdk/datatypes/vector.h"
// used: array
#include <array>

struct Ray_t
{
public:
	Vector_t m_vecStart;
	Vector_t m_vecEnd;
	Vector_t m_vecMins;
	Vector_t m_vecMaxs;
	MEM_PAD(0x4);
	std::uint8_t UnkType;
};
static_assert(sizeof(Ray_t) == 0x38);

struct SurfaceData_t
{
public:
	MEM_PAD(0x8)
	float m_flPenetrationModifier;
	float m_flDamageModifier;
	MEM_PAD(0x4)
	int m_iMaterial;
};

static_assert(sizeof(SurfaceData_t) == 0x18);

struct TraceHitboxData_t
{
public:
	MEM_PAD(0x38);
	int m_nHitGroup;
	MEM_PAD(0x4);
	int m_nHitboxId;
};
static_assert(sizeof(TraceHitboxData_t) == 0x44);

class C_CSPlayerPawn;
struct GameTrace_t
{
public:
	GameTrace_t() = default;

	SurfaceData_t* GetSurfaceData();
	int GetHitboxId();
	int GetHitgroup();
	bool IsVisible() const;

	void* m_pSurface;
	C_CSPlayerPawn* m_pHitEntity;
	TraceHitboxData_t* m_pHitboxData;
	MEM_PAD(0x38);
	std::uint32_t m_uContents;
	MEM_PAD(0x24);
	Vector_t m_vecStartPos;
	Vector_t m_vecEndPos;
	Vector_t m_vecNormal;
	Vector_t m_vecPosition;
	MEM_PAD(0x4);
	float m_flFraction;
	MEM_PAD(0x6);
	bool m_bAllSolid;
	MEM_PAD(0x4D)
}; // Size: 0x108

static_assert(sizeof(GameTrace_t) == 0x108);

struct TraceFilter_t
{
public:
	MEM_PAD(0x8);
	std::int64_t m_uTraceMask;
	std::array<std::int64_t, 2> m_v1;
	std::array<std::int32_t, 4> m_arrSkipHandles;
	std::array<std::int16_t, 2> m_arrCollisions;
	std::int16_t m_v2;
	std::uint8_t m_v3;
	std::uint8_t m_v4;
	std::uint8_t m_v5;

	TraceFilter_t() = default;
	TraceFilter_t(std::uint64_t uMask, C_CSPlayerPawn* pSkip1, C_CSPlayerPawn* pSkip2, int nLayer);
};
static_assert(sizeof(TraceFilter_t) == 0x40);

struct update_value_t {
	float m_previous_length_mod{ }, m_current_length_mod{ };
	char pad0[0x8];
	std::int16_t m_handle_idx{ };
	char pad1[0x6];
};

struct trace_arr_element_t {
	char pad[0x30];
};

struct trace_data_t {
	std::int32_t m_uk1{ };
	float m_uk2{ 52.0f };
	void* m_arr_pointer{ };
	std::int32_t m_uk3{ 128 };
	std::int32_t m_uk4{ static_cast<std::int32_t>(0x80000000) };
	std::array< trace_arr_element_t, 0x80 > m_arr = { };

	char pad1[0x8];
	std::int64_t m_num_update{ };
	void* m_pointer_update_value{ };
	char pad2[0xC8];
	Vector_t m_start{ }, m_end{ };
	char pad3[0x50];
};

class CGameTraceManager
{
public:
	bool TraceShape(Ray_t* pRay, Vector_t vecStart, Vector_t vecEnd, TraceFilter_t* pFilter, GameTrace_t* pGameTrace)
	{
		using fnTraceShape = bool(__fastcall*)(CGameTraceManager*, Ray_t*, Vector_t*, Vector_t*, TraceFilter_t*, GameTrace_t*);
		// Credit: https://www.unknowncheats.me/forum/4265752-post6333.html
		static fnTraceShape oTraceShape = reinterpret_cast<fnTraceShape>(MEM::FindPattern(CLIENT_DLL, CS_XOR("48 89 5C 24 ? 48 89 4C 24 ? 55 56 41 55")));

		#ifdef CS_PARANOID
		CS_ASSERT(oTraceShape != nullptr);
		#endif

		return oTraceShape(this, pRay, &vecStart, &vecEnd, pFilter, pGameTrace);
	}

	void ClipTraceToPlayer(Vector_t& start, Vector_t& end, TraceFilter_t* filter, GameTrace_t* trace, float min, int length, float max)
	{
		using fnClipTraceToPlayer = void(__fastcall*)(Vector_t&, Vector_t&, TraceFilter_t*, GameTrace_t*, float, int, float);
		static fnClipTraceToPlayer oClipTraceToPlayer = reinterpret_cast<fnClipTraceToPlayer>(MEM::FindPattern(CLIENT_DLL, CS_XOR("48 8B C4 55 56 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 48 89 58 ? 49 8B F0")));

		#ifdef CS_PARANOID
		CS_ASSERT(oClipTraceToPlayer != nullptr);
		#endif

		return oClipTraceToPlayer(start, end, filter, trace, min, max, length);
	}

	bool ClipRayToEntity(Ray_t* pRay, Vector_t vecStart, Vector_t vecEnd, C_CSPlayerPawn* pPawn, TraceFilter_t* pFilter, GameTrace_t* pGameTrace)
	{
		using fnClipRayToEntity = bool(__fastcall*)(CGameTraceManager*, Ray_t*, Vector_t*, Vector_t*, C_CSPlayerPawn*, TraceFilter_t*, GameTrace_t*);
		static fnClipRayToEntity oClipRayToEntity = reinterpret_cast<fnClipRayToEntity>(MEM::FindPattern(CLIENT_DLL, CS_XOR("48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 48 89 7C 24 20 41 54 41 56 41 57 48 81 EC C0 00 00 00 48 8B 9C")));

		#ifdef CS_PARANOID
		CS_ASSERT(oClipRayToEntity != nullptr);
		#endif

		return oClipRayToEntity(this, pRay, &vecStart, &vecEnd, pPawn, pFilter, pGameTrace);
	}

	bool HandleBulletPenetration(trace_data_t* const trace, void* stats,
		update_value_t* const mod_value,
		const bool draw_showimpacts = false);
	void GetTraceInfo(trace_data_t* const trace, GameTrace_t* const hit,
		const float unknown_float, void* unknown);
	void InitTraceInfo(GameTrace_t* const hit);

	void create_trace(trace_data_t* const trace, const Vector_t start,
		const Vector_t end, const TraceFilter_t& filler,
		const int penetration_count);
	void InitTraceFilter(TraceFilter_t& filter, C_CSPlayerPawn* skip, uint64_t mask, uint8_t layer, uint16_t idk);


};

enum Contents_t {
	CONTENTS_EMPTY = 0,
	CONTENTS_SOLID = 0x1,
	CONTENTS_WINDOW = 0x2,
	CONTENTS_AUX = 0x4,
	CONTENTS_GRATE = 0x8,
	CONTENTS_SLIME = 0x10,
	CONTENTS_WATER = 0x20,
	CONTENTS_BLOCKLOS = 0x40,
	CONTENTS_OPAQUE = 0x80,
	CONTENTS_TESTFOGVOLUME = 0x100,
	CONTENTS_UNUSED = 0x200,
	CONTENTS_BLOCKLIGHT = 0x400,
	CONTENTS_TEAM1 = 0x800,
	CONTENTS_TEAM2 = 0x1000,
	CONTENTS_IGNORE_NODRAW_OPAQUE = 0x2000,
	CONTENTS_MOVEABLE = 0x4000,
	CONTENTS_AREAPORTAL = 0x8000,
	CONTENTS_PLAYERCLIP = 0x10000,
	CONTENTS_MONSTERCLIP = 0x20000,
	CONTENTS_CURRENT_0 = 0x40000,
	CONTENTS_CURRENT_90 = 0x80000,
	CONTENTS_CURRENT_180 = 0x100000,
	CONTENTS_CURRENT_270 = 0x200000,
	CONTENTS_CURRENT_UP = 0x400000,
	CONTENTS_CURRENT_DOWN = 0x800000,
	CONTENTS_ORIGIN = 0x1000000,
	CONTENTS_MONSTER = 0x2000000,
	CONTENTS_DEBRIS = 0x4000000,
	CONTENTS_DETAIL = 0x8000000,
	CONTENTS_TRANSLUCENT = 0x10000000,
	CONTENTS_LADDER = 0x20000000,
	CONTENTS_HITBOX = 0x40000000,
};

enum Masks_t {
	MASK_ALL = 0xFFFFFFFF,
	MASK_SOLID = CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_MONSTER | CONTENTS_GRATE,
	MASK_PLAYERSOLID = CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_PLAYERCLIP | CONTENTS_WINDOW | CONTENTS_MONSTER | CONTENTS_GRATE,
	MASK_NPCSOLID = CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_MONSTERCLIP | CONTENTS_WINDOW | CONTENTS_MONSTER | CONTENTS_GRATE,
	MASK_NPCFLUID = CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_MONSTERCLIP | CONTENTS_WINDOW | CONTENTS_MONSTER | CONTENTS_GRATE,
	MASK_WATER = CONTENTS_WATER | CONTENTS_MOVEABLE | CONTENTS_SLIME,
	MASK_OPAQUE = CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_OPAQUE,
	MASK_OPAQUE_AND_NPCS = MASK_OPAQUE | CONTENTS_MONSTER,
	MASK_BLOCKLOS = CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_BLOCKLOS,
	MASK_BLOCKLOS_AND_NPCS = MASK_BLOCKLOS | CONTENTS_MONSTER,
	MASK_VISIBLE = MASK_OPAQUE | CONTENTS_IGNORE_NODRAW_OPAQUE,
	MASK_VISIBLE_AND_NPCS = MASK_OPAQUE_AND_NPCS | CONTENTS_IGNORE_NODRAW_OPAQUE,
	MASK_SHOT = CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_MONSTER | CONTENTS_WINDOW | CONTENTS_DEBRIS | CONTENTS_GRATE | CONTENTS_HITBOX,
	MASK_SHOT_BRUSHONLY = CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_DEBRIS,
	MASK_SHOT_HULL = CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_MONSTER | CONTENTS_WINDOW | CONTENTS_DEBRIS | CONTENTS_GRATE,
	MASK_SHOT_PORTAL = CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_MONSTER,
	MASK_SOLID_BRUSHONLY = CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_GRATE,
	MASK_PLAYERSOLID_BRUSHONLY = CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_PLAYERCLIP | CONTENTS_GRATE,
	MASK_NPCSOLID_BRUSHONLY = CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_MONSTERCLIP | CONTENTS_GRATE,
	MASK_NPCWORLDSTATIC = CONTENTS_SOLID | CONTENTS_WINDOW | CONTENTS_MONSTERCLIP | CONTENTS_GRATE,
	MASK_NPCWORLDSTATIC_FLUID = CONTENTS_SOLID | CONTENTS_WINDOW | CONTENTS_MONSTERCLIP,
	MASK_SPLITAREPORTAL = CONTENTS_WATER | CONTENTS_SLIME,
	MASK_CURRENT = CONTENTS_CURRENT_0 | CONTENTS_CURRENT_90 | CONTENTS_CURRENT_180 | CONTENTS_CURRENT_270 | CONTENTS_CURRENT_UP | CONTENTS_CURRENT_DOWN,
	MASK_DEADSOLID = CONTENTS_SOLID | CONTENTS_PLAYERCLIP | CONTENTS_WINDOW | CONTENTS_GRATE,
};

enum {
	SURF_LIGHT = 0x0001,
	SURF_SKY2D = 0x0002,
	SURF_SKY = 0x0004,
	SURF_WARP = 0x0008,
	SURF_TRANS = 0x0010,
	SURF_NOPORTAL = 0x0020,
	SURF_TRIGGER = 0x0040,
	SURF_NODRAW = 0x0080,
	SURF_HINT = 0x0100,
	SURF_SKIP = 0x0200,
	SURF_NOLIGHT = 0x0400,
	SURF_BUMPLIGHT = 0x0800,
	SURF_NOSHADOWS = 0x1000,
	SURF_NODECALS = 0x2000,
	SURF_NOPAINT = SURF_NODECALS,
	SURF_NOCHOP = 0x4000,
	SURF_HITBOX = 0x8000
};

