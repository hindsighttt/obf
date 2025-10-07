#pragma once
class CUserCmd;
class CBaseUserCmdPB;
class CCSGOInputHistoryEntryPB;

class CCSPlayerController;
class C_CSPlayerPawn;

struct QAngle_t;

struct Enemy_t
{
	C_CSPlayerPawn* pPawn;
	CCSPlayerController* pController;
};

namespace F::RAGEBOT::RAGE
{
		void OnMove(CUserCmd* pCmd, CBaseUserCmdPB* pBaseCmd, CCSPlayerController* pLocalController, C_CSPlayerPawn* pLocalPawn);
}