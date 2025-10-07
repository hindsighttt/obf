class CUserCmd;
class CBaseUserCmdPB;
class CCSGOInputHistoryEntryPB;

class CCSPlayerController;
class C_CSPlayerPawn;

struct QAngle_t;

namespace F::TriggerBot
{
	void OnMove(CUserCmd* pCmd, CBaseUserCmdPB* pBaseCmd, CCSPlayerController* pLocalController, C_CSPlayerPawn* pLocalPawn);
}