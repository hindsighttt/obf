#include "sdk/entity.h"
#include "sdk/interfaces/cgameentitysystem.h"
#include "sdk/interfaces/iengineclient.h"
// used: cusercmd
#include "sdk/datatypes/usercmd.h"

// used: activation button
#include "utilities/inputsystem.h"

// used: cheat variables
#include "core/variables.h"
#include "core/menu.h"

#include "sdk/interfaces/cgametracemanager.h"
#include "sdk/interfaces/ccsgoinput.h"
#include "sdk/interfaces/iinputsystem.h"
#include "sdk/interfaces/iengineclient.h"
#include "triggerbot.h"
#include "wall.h"
#include "core/sdk.h"
#include <thread>

void Attack(CUserCmd* pCmd, CBaseUserCmdPB* pUserCmd)
{
	I::Engine->ClientCmd_Unrestricted("+attack");
	I::Engine->ClientCmd_Unrestricted("-attack");
	/*
	if (MENU::bMainWindowOpened)
		return;
	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	*/
	/*
	if (!(pCmd->nButtons.nValue & IN_ATTACK))
	{
		pCmd->nButtons.nValue |= IN_ATTACK;
		pCmd->nButtons.nValueChanged |= IN_ATTACK;

		pCmd->csgoUserCmd.pBaseCmd->pInButtonState->nValue |= IN_ATTACK;
		pCmd->csgoUserCmd.pBaseCmd->pInButtonState->nValueScroll |= IN_ATTACK;
		auto pSubtickPress = pCmd->csgoUserCmd.pBaseCmd->create_subtick_step(pCmd->csgoUserCmd.pBaseCmd->subtickMovesField.pArena);
		pSubtickPress->nCachedBits = 7;
		pSubtickPress->nButton = IN_ATTACK;
		pSubtickPress->bPressed = true;
		//subtick->unk = 4;
		pSubtickPress->flWhen = 0.999f;
		auto pSubtickRelease = pCmd->csgoUserCmd.pBaseCmd->create_subtick_step(pCmd->csgoUserCmd.pBaseCmd->subtickMovesField.pArena);
		pSubtickPress->nCachedBits = 7;
		pSubtickPress->nButton = IN_ATTACK;
		pSubtickPress->bPressed = false;
		//subtick->unk = 4;
		pSubtickPress->flWhen = 0.999f;
	}

	pCmd->SetAttackHistory(0 - 1);
	*/

}

void F::TriggerBot::OnMove(CUserCmd* pCmd, CBaseUserCmdPB* pBaseCmd, CCSPlayerController* pLocalController, C_CSPlayerPawn* pLocalPawn)
{
	if (!C_GET(bool, Vars.bTriggerbot))
		return;
	if (!IPT::IsKeyDown(C_GET(unsigned int, Vars.nTriggerbotActivationKey)) && !C_GET(bool, Vars.bTriggerbotAlwaysOn))
		return;

	if (!I::Engine->IsConnected() || !I::Engine->IsInGame())
		return;
	if (!pLocalController->IsPawnAlive())
		return;
	if (pLocalPawn->IsWaitForNoAttack())
		return;
	CCSPlayer_WeaponServices* pWeaponService = pLocalPawn->GetWeaponServices();
	if (!pWeaponService)
		return;
	CBaseHandle hActiveWeapon = pWeaponService->GetActiveWeapon();
	auto pWeapon = I::GameResourceService->pGameEntitySystem->Get<C_CSWeaponBase>(hActiveWeapon);
	if (!pWeapon)
		return;
    CCSWeaponBaseVData* pWeaponVData = reinterpret_cast<CCSWeaponBaseVData*>(pWeapon->GetVData());
	if (!pWeaponVData)
		return;
	int iWeaponType = pWeaponVData->GetWeaponType();
	if (iWeaponType == WEAPONTYPE_C4 || iWeaponType == WEAPONTYPE_KNIFE || iWeaponType == WEAPONTYPE_EQUIPMENT || iWeaponType == WEAPONTYPE_GRENADE)
		return;
	auto pActiveWeaponBaseGun = I::GameResourceService->pGameEntitySystem->Get<C_CSWeaponBaseGun>(pWeapon->GetRefEHandle());
	if (!pActiveWeaponBaseGun)
		return;
	if (!pActiveWeaponBaseGun->CanPrimaryAttack(pWeaponVData->GetWeaponType(), TICKS_TO_TIME(pLocalController->GetTickBase())))
		return;
	if (pWeapon->IsInReload())
		return;
	if (C_GET(bool, Vars.bTriggerBotScopedOnly))
	{
		if (pWeaponVData->GetWeaponType() == WEAPONTYPE_SNIPER_RIFLE)
			if (pActiveWeaponBaseGun->GetZoomLevel() == 0)
				return;
	}

	// We validated everthing, time to check if the enemy is visible
	if (!C_GET(bool, Vars.bTriggerWall))
	{
		GameTrace_t trace = GameTrace_t();
		TraceFilter_t filter = TraceFilter_t(0x1C3003, pLocalPawn, nullptr, 4);
		Ray_t pRay = Ray_t();

		//1174421515 awall mask?

		Vector_t vecForward = Vector_t();
		QAngle_t vecViewAngle = pCmd->csgoUserCmd.pBaseCmd->pViewAngles->angValue;
		vecViewAngle += pLocalPawn->GetAimPunchAngle();
		Vector_t vecEyePosition = pLocalPawn->GetEyePosition();
		vecViewAngle.ToDirections(&vecForward);
		vecForward *= pWeaponVData->GetRange();
		Vector_t vecAimLocation = vecEyePosition + vecForward;
		I::GameTraceManager->TraceShape(&pRay, vecEyePosition, vecAimLocation, &filter, &trace);
		if (!trace.m_pHitEntity || !trace.m_pHitEntity->IsBasePlayerPawn())
			return;
		if (trace.m_pHitEntity->GetHealth() <= 0 || !trace.m_pHitEntity->IsOtherEnemy(pLocalPawn) || trace.m_pHitEntity->IsImmune())
			return;
	}
	else
	{
		GameTrace_t trace = GameTrace_t();
		TraceFilter_t filter = TraceFilter_t(0x1C300Bu, pLocalPawn, nullptr, 4);
		Ray_t pRay = Ray_t();

		//1174421515 awall mask?

		Vector_t vecForward = Vector_t();
		QAngle_t vecViewAngle = pCmd->csgoUserCmd.pBaseCmd->pViewAngles->angValue;
		vecViewAngle += pLocalPawn->GetAimPunchAngle();
		Vector_t vecEyePosition = pLocalPawn->GetEyePosition();
		vecViewAngle.ToDirections(&vecForward);
		vecForward *= pWeaponVData->GetRange();
		Vector_t vecAimLocation = vecEyePosition + vecForward;
		I::GameTraceManager->TraceShape(&pRay, vecEyePosition, vecAimLocation, &filter, &trace);
		if (!trace.m_pHitEntity)
			return;
		GameTrace_t tracenew = GameTrace_t();
		TraceFilter_t filternew = TraceFilter_t(0x1C300Bu, pLocalPawn, trace.m_pHitEntity, 4);
		Ray_t pRayNew = Ray_t();
		bool bAutoWall = false;
		auto target = trace.m_pHitEntity;
		if (!trace.m_pHitEntity->IsBasePlayerPawn())
		{
			bAutoWall = true;
			I::GameTraceManager->TraceShape(&pRayNew, vecEyePosition, vecAimLocation, &filternew, &tracenew);
			if (!tracenew.m_pHitEntity || !tracenew.m_pHitEntity->IsBasePlayerPawn())
				return;
			target = tracenew.m_pHitEntity;
		}
		
		if (target->GetHealth() <= 0 /* || !target->IsOtherEnemy(pLocalPawn) */ || target->IsImmune())
			return;

		auto entity = I::GameResourceService->pGameEntitySystem->Get(target->GetRefEHandle());
		if (!entity)
			return;
		auto localEnt = I::GameResourceService->pGameEntitySystem->Get(pLocalPawn->GetRefEHandle());
		if (!localEnt)
			return;
		data_t data;
		float damage = 0.0f;
		bool canHit = false;
		WALL::Penetrate(data, vecEyePosition, target->GetEyePosition(), entity, localEnt, pLocalPawn, target, pWeaponVData, damage, canHit);
		L_PRINT(LOG_INFO) << data.m_dmg;
		if (data.m_dmg <= 0)
			return;
	}
	if (pCmd->csgoUserCmd.pBaseCmd->nClientTick >= pWeapon->GetNextPrimaryAttackTick())
	{
		//L_PRINT(LOG_INFO) << pWeapon->GetNextPrimaryAttackTick();
		//L_PRINT(LOG_INFO) << ("tried to shot at: " + std::to_string(pCmd->csgoUserCmd.pBaseCmd->nClientTick)).c_str();
		Attack(pCmd, pCmd->csgoUserCmd.pBaseCmd);
		/*
		pCmd->nButtons.nValue |= IN_ATTACK;
		//pCmd->nButtons.nValueChanged |= IN_ATTACK;
		//pCmd->nButtons.nValueScroll |= IN_ATTACK;

		auto press = pCmd->csgoUserCmd.pBaseCmd->add_subtick_move();
		press->nCachedBits = 7;
		press->nButton = IN_ATTACK;
		press->bPressed = true;
		press->flWhen = 0.01f;
		pCmd->SetAttackHistory(pCmd->csgoUserCmd.inputHistoryField.nCurrentSize - 1);
		//pCmd->SetAttackHistory(-1);


		auto release = pCmd->csgoUserCmd.pBaseCmd->add_subtick_move();
		release->nCachedBits = 7;
		release->nButton = IN_ATTACK;
		release->bPressed = false;
		release->flWhen = 0.02f;
		pCmd->SetAttackHistory(pCmd->csgoUserCmd.inputHistoryField.nCurrentSize - 1);
		*/
	}
}
