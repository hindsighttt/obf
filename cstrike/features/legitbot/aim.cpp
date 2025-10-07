#include "aim.h"

// used: sdk entity
#include "../../sdk/entity.h"
#include "../../sdk/interfaces/cgameentitysystem.h"
#include "../../sdk/interfaces/iengineclient.h"
// used: cusercmd
#include "../../sdk/datatypes/usercmd.h"

// used: activation button
#include "../../utilities/inputsystem.h"

// used: cheat variables
#include "../../core/variables.h"

#include "../../sdk/interfaces/cgametracemanager.h"
#include "../../utilities/draw.h"

void F::LEGITBOT::AIM::OnMove(CUserCmd* pCmd, CBaseUserCmdPB* pBaseCmd, CCSPlayerController* pLocalController, C_CSPlayerPawn* pLocalPawn)
{
	// Check if the legitbot is enabled
	if (!C_GET(bool, Vars.bLegitbot))
		return;

	if (!pLocalController->IsPawnAlive())
		return;

	AimAssist(pCmd, pBaseCmd, pLocalPawn, pLocalController);
}

QAngle_t GetRecoil(CBaseUserCmdPB* pCmd, C_CSPlayerPawn* pLocal)
{
	static QAngle_t OldPunch; // get last tick AimPunch angles

	if (pLocal->GetShotsFired() > 0) // only update aimpunch while shooting
	{
		QAngle_t aimPunch = pLocal->GetAimPunchAngle() * 2.0f;
		OldPunch = aimPunch; // update old punch
		return aimPunch; // return correct aimpunch delta
	}
	else
	{
		OldPunch = QAngle_t{ 0, 0, 0 }; // reset old punch when not firing
		return QAngle_t{ 0, 0, 0 }; // return 0 if not shooting
	}
}

QAngle_t GetStandaloneRCS(CBaseUserCmdPB* pCmd, C_CSPlayerPawn* pLocal)
{
	static auto m_prev_punch = QAngle_t(0.f, 0.f, 0.f);
	auto& m_cached_punch = pLocal->GetAimPunchCache();
	QAngle_t correct_punch;

	auto m_predicted_punch = m_cached_punch.Size() > 0 ? m_cached_punch.Element(m_cached_punch.Size() - 1) : QAngle_t(0, 0, 0);
	correct_punch = m_prev_punch - m_predicted_punch * 2.f;
	if (m_cached_punch.Size() > 0 && m_cached_punch.Size() <= 0xFFFF) {
		m_predicted_punch = m_cached_punch.Element(m_cached_punch.Size() - 1);
		m_prev_punch = m_predicted_punch;
	}
	return correct_punch;
}

QAngle_t GetAngularDifference(CBaseUserCmdPB* pCmd, Vector_t vecTarget, C_CSPlayerPawn* pLocal)
{
	// The current position
	Vector_t vecCurrent = pLocal->GetEyePosition();


	// The new angle
	QAngle_t vNewAngle = (vecTarget - vecCurrent).ToAngles();
	vNewAngle.Normalize(); // Normalise it so we don't jitter about

	// Store our current angles
	QAngle_t vCurAngle = pCmd->pViewAngles->angValue;
	//QAngle_t vCurAngle = pLocal->GetLocalPlayer()->GetGameSceneNode()->GetAbsAngleRotation();

	// Find the difference between the two angles (later useful when adding smoothing)
	vNewAngle -= vCurAngle;

	return vNewAngle;
}

float GetAngularDistance(CBaseUserCmdPB* pCmd, Vector_t vecTarget, C_CSPlayerPawn* pLocal)
{
	return GetAngularDifference(pCmd, vecTarget, pLocal).Length2D();
}

enum bones : int
{
	head = 6,
	neck = 5,
	spine = 4,
	spine_1 = 2,
	left_shoulder = 8,
	left_arm = 9,
	left_hand = 11,
	pelvis = 0,
	right_shoulder = 13,
	right_arm = 14,
	right_hand = 16,
	left_hip = 22,
	left_knee = 23,
	left_feet = 24,
	right_hip = 25,
	right_knee = 26,
	right_feet = 27
};

void F::LEGITBOT::AIM::AimAssist(CUserCmd* pCmd, CBaseUserCmdPB* pUserCmd, C_CSPlayerPawn* pLocalPawn, CCSPlayerController* pLocalController)
{
	// Check if the activation key is down
	if (!IPT::IsKeyDown(C_GET(unsigned int, Vars.nLegitbotActivationKey)) && !C_GET(bool, Vars.bLegitbotAlwaysOn))
		return;

	auto pWeaponService = pLocalPawn->GetWeaponServices();
	if (!pWeaponService)
		return;
	auto hWeapon = pWeaponService->GetActiveWeapon();
	auto pWeapon = I::GameResourceService->pGameEntitySystem->Get<C_CSWeaponBase>(hWeapon);
	if (!pWeapon)
		return;
	
    auto pWeaponVData = reinterpret_cast<CCSWeaponBaseVData*>(pWeapon->GetVData());
	if (!pWeaponVData)
		return;

	int iWeaponType = pWeaponVData->GetWeaponType();
	if (iWeaponType == WEAPONTYPE_C4 || iWeaponType == WEAPONTYPE_KNIFE || iWeaponType == WEAPONTYPE_EQUIPMENT || iWeaponType == WEAPONTYPE_GRENADE)
		return;

	// The current best distance
	float flDistance = INFINITY;
	// The target we have chosen
	CCSPlayerController* pTarget = nullptr;
	// Cache'd position
	Vector_t vecBestPosition = Vector_t();

	// Entity loop
	const int iHighestIndex = 126;
	//const int iHighestIndex = I::GameResourceService->pGameEntitySystem->GetHighestEntityIndex();

	for (int nIndex = 1; nIndex <= iHighestIndex; nIndex++)
	{
		// Get the entity
		C_BaseEntity* pEntity = I::GameResourceService->pGameEntitySystem->Get(nIndex);
		if (pEntity == nullptr)
			continue;

		// Get the class info
		SchemaClassInfoData_t* pClassInfo = nullptr;
		pEntity->GetSchemaClassInfo(&pClassInfo);
		if (pClassInfo == nullptr)
			continue;

		// Get the hashed name
		const FNV1A_t uHashedName = FNV1A::Hash(pClassInfo->szName);

		// Make sure they're a player controller
		if (uHashedName != FNV1A::HashConst("CCSPlayerController"))
			continue;

		// Cast to player controller
		CCSPlayerController* pPlayer = reinterpret_cast<CCSPlayerController*>(pEntity);
		if (pPlayer == nullptr)
			continue;

		// Check the entity is not us
		if (pPlayer == pLocalController)
			continue;

		// Get the player pawn
		C_CSPlayerPawn* pPawn = I::GameResourceService->pGameEntitySystem->Get<C_CSPlayerPawn>(pPlayer->GetPawnHandle());
		if (pPawn == nullptr)
			continue;

		// Make sure they're alive
		if (!pPlayer->IsPawnAlive())
			continue;

		// Check if they're an enemy
		if (!pLocalPawn->IsOtherEnemy(pPawn))
			continue;

		// Check if they're dormant
		CGameSceneNode* pCGameSceneNode = pPawn->GetGameSceneNode();
		if (pCGameSceneNode == nullptr || pCGameSceneNode->IsDormant())
			continue;

		// Get the position

		// Firstly, get the skeleton
		CSkeletonInstance* pSkeleton = pCGameSceneNode->GetSkeletonInstance();
		if (pSkeleton == nullptr)
			continue;
		// Now the bones
		Matrix2x4_t* pBoneCache = pSkeleton->pBoneCache;
		if (pBoneCache == nullptr)
			continue;

		int iBone;
		unsigned int iBoneArray = C_GET(unsigned int, Vars.nLegitHitbox);
		//	"Head",
		//	"Chest",
		//	"Stomach",
		for (int i = 0; i < 32; ++i) // 32 bits in unsigned int
		{
			if (iBoneArray & (1u << i))
			{
				if (i == 0) // Head
					iBone = head;
				if (i == 1) // Chest
					iBone = spine;
				if (i == 2)
					iBone = pelvis;
			}
			else
				continue;
		

			// Get the bone's position
			Vector_t vecPos = pBoneCache->GetOrigin(iBone);

			// @note: this is a simple example of how to check if the player is visible

			// initialize trace, construct filterr and initialize ray
			GameTrace_t trace = GameTrace_t();
			TraceFilter_t filter = TraceFilter_t(0x1C3003, pLocalPawn, nullptr, 4);
			Ray_t ray = Ray_t();

			// cast a ray from local player eye positon -> player head bone
			// @note: would recommend checking for nullptrs
			I::GameTraceManager->TraceShape(&ray, pLocalPawn->GetEyePosition(), pPawn->GetGameSceneNode()->GetSkeletonInstance()->pBoneCache->GetOrigin(6), &filter, &trace);
			// check if the hit entity is the one we wanted to check and if the trace end point is visible
			if ((trace.m_pHitEntity != pPawn || !trace.IsVisible()) && C_GET(bool, Vars.bLegitBotVisCheck))// if invisible, skip this entity
				continue;

			float flCurrentDistance = INFINITY;
			ImVec2 screenTarget, screenCenter;
			if (D::WorldToScreen(vecPos, &screenTarget))
			{
				ImVec2 displaySize = ImGui::GetIO().DisplaySize;
				screenCenter = ImVec2(displaySize.x / 2, displaySize.y / 2);

				flCurrentDistance = sqrtf(
					(screenTarget.x - screenCenter.x) * (screenTarget.x - screenCenter.x) +
					(screenTarget.y - screenCenter.y) * (screenTarget.y - screenCenter.y)
				);

				if (C_GET(int, Vars.nAimMode) == 1)
				{
					float flMaxFov = C_GET(float, Vars.flAimRange) * 6; // e.g. 100 pixels

					if (flCurrentDistance > flMaxFov)
						continue; // Skip this target, it's outside the FO
				}
			}

			// Get the distance/weight of the move
			if (C_GET(int, Vars.nAimMode) == 0 || C_GET(bool, Vars.bLegitSilent))
			{
				flCurrentDistance = GetAngularDistance(pUserCmd, vecPos, pLocalPawn);

				if (flCurrentDistance > C_GET(float, Vars.flAimRange))// Skip if this move out of aim range
					continue;
			}

			if (pTarget && flCurrentDistance > flDistance) // Override if this is the first move or if it is a better move
				continue;

			// Better move found, override.
			pTarget = pPlayer;
			flDistance = flCurrentDistance;
			vecBestPosition = vecPos;
		}
	}

	// Check if a target was found
	if (pTarget == nullptr)
	{
		return;
	}
		

	// Point at them
	QAngle_t* pViewAngles = &(pUserCmd->pViewAngles->angValue); // Just for readability sake!

	// Find the change in angles
	QAngle_t vNewAngles = GetAngularDifference(pUserCmd, vecBestPosition, pLocalPawn);

	// Get the smoothing
	const float flSmoothing = C_GET(float, Vars.flSmoothing);
	auto aimPunch =  GetStandaloneRCS(pUserCmd, pLocalPawn); //get AimPunch angles
	// Apply smoothing and set angles
	if (!C_GET(bool, Vars.bLegitSilent))
	{
		pViewAngles->x += (vNewAngles.x + aimPunch.x * 2.0f) / flSmoothing;// minus AimPunch angle to counteract recoil
		pViewAngles->y += (vNewAngles.y + aimPunch.y * 2.0f) / flSmoothing;
		pViewAngles->Normalize();
	}
	else
	{
		QAngle_t silentAngles = *pViewAngles;
		silentAngles.x += vNewAngles.x + aimPunch.x * 2.0f;
		silentAngles.y += vNewAngles.y + aimPunch.y * 2.0f;
		silentAngles.Normalize();

		// Set the silent aim angles
		pCmd->SetSubTickAngle(silentAngles);
	}
}
