#pragma once
#include "cplayer_movementservices.hpp"
#include "cplayer_weaponservices.hpp"
#include "../interfaces/cplayer_cameraservices.hpp"
#include "c_basemodelentity.hpp"

class C_BasePlayerPawn : public C_BaseModelEntity
{
public:
	SCHEMA_FIELD(m_hController, "C_BasePlayerPawn", "m_hController", CHandle);
	SCHEMA_FIELD(m_pWeaponServices, "C_BasePlayerPawn", "m_pWeaponServices",
				 CPlayer_WeaponServices*);
	SCHEMA_FIELD(m_pMovementServices, "C_BasePlayerPawn", "m_pMovementServices",
				 CPlayer_MovementServices*);
	SCHEMA_FIELD(m_pCameraServices, "C_BasePlayerPawn", "m_pCameraServices",
				 CPlayer_CameraServices*);
};
