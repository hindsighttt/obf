#pragma once

#include "ccollisionproperty.hpp"
#include "centityinstance.hpp"
#include "cgamescenenode.hpp"
#include "../types/bitflags.hpp"
#include "../types/movetype.hpp"

#include "../../../utils/color.h"
#include "../../../sdk/memory/memory.hpp"

struct BBox_t
{
	float x, y, w, h;
};


class C_BaseEntity : public CEntityInstance
{
public:
	bool IsBasePlayerController( );
	bool IsBasePlayerWeapon( );
	bool IsChicken( );
	bool IsViewModel( );

	const Vector& GetOrigin( );
	bool GetBoundingBox(BBox_t& out, bool computeSurroundingBox = false);
	bool ComputeHitboxSurroundingBox(Vector& mins, Vector& maxs);
	float DistanceToSquared(C_BaseEntity* pEntity);

	void PostDataUpdate(int iUpdateType)
	{
		CALL_VIRTUAL(bool, 7, this, iUpdateType);
	}

	SCHEMA_FIELD(m_pGameSceneNode, "C_BaseEntity", "m_pGameSceneNode",
				 CGameSceneNode*);
	SCHEMA_FIELD(m_pCollision, "C_BaseEntity", "m_pCollision",
				 CCollisionProperty*);
	SCHEMA_FIELD(m_iTeamNum, "C_BaseEntity", "m_iTeamNum", uint8_t);
	SCHEMA_FIELD(m_hOwnerEntity, "C_BaseEntity", "m_hOwnerEntity", CHandle);
	SCHEMA_FIELD(m_iHealth, "C_BaseEntity", "m_iHealth", int);
	SCHEMA_FIELD(m_vecVelocity, "C_BaseEntity", "m_vecVelocity", Vector);
	SCHEMA_FIELD(GetCollision, "C_BaseEntity", "m_pCollision", CCollisionProperty);
	SCHEMA_FIELD(m_fFlags, "C_BaseEntity", "m_fFlags", BitFlag);
	SCHEMA_FIELD(m_MoveType, "C_BaseEntity", "m_MoveType", MoveType);
	bool OnGround( ) noexcept
	{
		return (m_fFlags( ).has_flag(1)) != 0;
	}

	SCHEMA_FIELD(m_nSubclassID, "C_BaseEntity", "m_nSubclassID", uint64_t);
	SCHEMA_FIELD_OFFSET(GetVData, "C_BaseEntity", "m_nSubclassID", 0x8, void*);

	void UpdateSubClass()
	{
		memory::fnGetPlayerPawn(this);
	}

	void UpdateVData()
	{
		CALL_VIRTUAL(void*, 182, this);
	}
};

class C_EnvSky : public C_BaseEntity
{
public:
	SCHEMA_FIELD(m_bStartDisabled, "C_EnvSky", "m_bStartDisabled", bool);
	SCHEMA_FIELD(m_vTintColor, "C_EnvSky", "m_vTintColor", Color_t);
	SCHEMA_FIELD(m_vTintColorLightingOnly, "C_EnvSky", "m_vTintColorLightingOnly", Color_t);
	SCHEMA_FIELD(m_flBrightnessScale, "C_EnvSky", "m_flBrightnessScale", float);
	SCHEMA_FIELD(m_nFogType, "C_EnvSky", "m_nFogType", int);
	SCHEMA_FIELD(m_flFogMinStart, "C_EnvSky", "m_flFogMinStart", float);
	SCHEMA_FIELD(m_flFogMinEnd, "C_EnvSky", "m_flFogMinEnd", float);
	SCHEMA_FIELD(m_flFogMaxStart, "C_EnvSky", "m_flFogMaxStart", float);
	SCHEMA_FIELD(m_flFogMaxEnd, "C_EnvSky", "m_flFogMaxEnd", float);
	SCHEMA_FIELD(m_bEnabled, "C_EnvSky", "m_bEnabled", bool);
};
