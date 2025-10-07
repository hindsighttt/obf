#include "antiaim.h"
#include "sdk/entity.h"
#include "sdk/interfaces/cgametracemanager.h"
#include "sdk/interfaces/cgameentitysystem.h"
#include "sdk/interfaces/iengineclient.h"
#include "core/interfaces.h"
#include "sdk/datatypes/usercmd.h"
#include "core/variables.h"
#include "wall.h"
#include <algorithm>
#include <math.h>
#include "core/sdk.h"
#include "utilities/math.h"
#include "features/visuals/overlay.h"
#include "sdk/interfaces/idebugoverlay.h"
#include "utilities/inputsystem.h"
#include "sdk/interfaces/ccsgoinput.h"

#include <thread>
#include <mutex>
#include <vector>
#include <future>

QAngle_t F::RAGEBOT::ANTIAIM::OriginalAngles = {};

void F::RAGEBOT::ANTIAIM::OnMove(CUserCmd* pCmd, CBaseUserCmdPB* pBaseCmd, CCSPlayerController* pLocalController, C_CSPlayerPawn* pLocalPawn)
{
	/*
	if (!I::Engine->IsConnected() || !I::Engine->IsInGame())
		return;
	if (!pLocalController->IsPawnAlive())
		return;
	F::RAGEBOT::ANTIAIM::OriginalAngles = pCmd->csgoUserCmd.pBaseCmd->pViewAngles->angValue;
	if (pCmd->nButtons.nValue & IN_USE)
		return;

	pCmd->csgoUserCmd.pBaseCmd->pViewAngles->angValue.x = 89.0f;
	//pCmd->csgoUserCmd.pBaseCmd->pViewAngles->angValue.y += 179.0f;
	//pCmd->csgoUserCmd.pBaseCmd->pViewAngles->angValue.z = 0;
	pCmd->csgoUserCmd.pBaseCmd->pViewAngles->angValue.Clamp();
	*/
}
