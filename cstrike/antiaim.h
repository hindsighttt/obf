class CBaseUserCmdPB;
class CUserCmd;
class CCSPlayerController;
class C_CSPlayerPawn;
struct QAngle_t;

namespace F::RAGEBOT::ANTIAIM
{
	extern QAngle_t OriginalAngles;
	void OnMove(CUserCmd* pCmd, CBaseUserCmdPB* pBaseCmd, CCSPlayerController* pLocalController, C_CSPlayerPawn* pLocalPawn);
}