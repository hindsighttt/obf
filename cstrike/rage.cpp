#include "rage.h"
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
#include "utilities/SHA1.h"

#include <thread>
#include <mutex>
#include <vector>
#include <future>

void vector_angles(Vector_t vec_forward, Vector_t& vec_angles)
{
	if (vec_forward.x == 0.f && vec_forward.y == 0.f)
	{
		vec_angles.y = 0.f;
		vec_angles.x = vec_forward.z > 0.f ? 270.f : 90.f;
	}
	else
	{
		vec_angles.y = M_RAD2DEG(std::atan2(vec_forward.y, vec_forward.x));
		if (vec_angles.y < 0.f)
			vec_angles.y += 360.f;

		vec_angles.x = M_RAD2DEG(std::atan2(-vec_forward.z, vec_forward.Length2D()));
		if (vec_angles.x < 0.f)
			vec_angles.x += 360.f;
	}

	vec_angles.x = std::remainder(vec_angles.x, 360.f);
	vec_angles.y = std::remainder(vec_angles.y, 360.f);
	vec_angles.z = std::remainder(vec_angles.z, 360.f);
}

float get_max_player_speed()
{
	auto pWeaponServices = SDK::LocalPawn->GetWeaponServices();
	if (!pWeaponServices)
		return 260.0f;
	CBaseHandle hWeapon = pWeaponServices->GetActiveWeapon();
	auto pWeapon = I::GameResourceService->pGameEntitySystem->Get<C_CSWeaponBase>(hWeapon);
	if (!pWeapon)
		return 260.0f;
	auto pWeaponData = pWeapon->GetWeaponVData();
	if (!pWeaponData)
		return 260.0f;

	return  SDK::LocalPawn->IsScoped() ? pWeaponData->GetMaxSpeed()[1] : pWeaponData->GetMaxSpeed()[0];
}

void autoStop(CUserCmd* pCmd, C_CSPlayerPawn* pLocal)
{
	if (!(pLocal->GetFlags() & FL_ONGROUND))
		return;
	if (pLocal->GetAbsVelocity().Length2D() <= 0.f)
		return;

	auto pWeaponServices = SDK::LocalPawn->GetWeaponServices();
	if (!pWeaponServices)
		return;
	CBaseHandle hWeapon = pWeaponServices->GetActiveWeapon();
	auto pWeapon = I::GameResourceService->pGameEntitySystem->Get<C_CSWeaponBase>(hWeapon);
	if (!pWeapon)
		return;


	float max_accurate_speed = get_max_player_speed() * 0.25f;
	float wish_speed = pWeapon->GetMaxSpeed() * 0.25f;
	if (pLocal->GetAbsVelocity().Length2D() <= max_accurate_speed)
	{
		Vector_t velocity = pLocal->GetAbsVelocity();
		float speed_2d = velocity.Length2D();

		if (speed_2d <= wish_speed)
		{
			pCmd->csgoUserCmd.pBaseCmd->flForwardMove = 0.f;
			pCmd->csgoUserCmd.pBaseCmd->flSideMove = 0.f;
			return;
		}

		Vector_t stopDir = -velocity.NormalizeInPlace();

		QAngle_t viewAngles = I::Input->GetViewAngles();
		Vector_t vecViewAngles = { 0, viewAngles.y, viewAngles.z };

		float sy, cy;
		sy = std::sinf((MATH::_PI / 180.f) * vecViewAngles.y);
		cy = std::cosf((MATH::_PI / 180.f) * vecViewAngles.y);

		Vector_t local_stop_dir;
		local_stop_dir.x = stopDir.x * cy + stopDir.y * sy;
		local_stop_dir.y = -stopDir.x * sy + stopDir.y * cy;

		pCmd->csgoUserCmd.pBaseCmd->flForwardMove = local_stop_dir.x;
		pCmd->csgoUserCmd.pBaseCmd->flSideMove = local_stop_dir.y;

		if (speed_2d > 50.f)
		{
			const float scale = wish_speed / speed_2d;
			pCmd->csgoUserCmd.pBaseCmd->flForwardMove = (pCmd->csgoUserCmd.pBaseCmd->flForwardMove * scale);
			pCmd->csgoUserCmd.pBaseCmd->flSideMove = (pCmd->csgoUserCmd.pBaseCmd->flSideMove * scale);
		}

		return;
	}

	// fullstop
	{
		Vector_t velocity = pLocal->GetAbsVelocity();
		float speed = velocity.Length2D();
		if (speed < 1.0f)
		{
			pCmd->csgoUserCmd.pBaseCmd->flForwardMove = 0.0f;
			pCmd->csgoUserCmd.pBaseCmd->flSideMove = 0.0f;
			return;
		}
		Vector_t velocity_angle;
		vector_angles(velocity * -1.0f, velocity_angle);
		velocity_angle.y = I::Input->GetViewAngles().y - velocity_angle.y;

		Vector_t forward;
		float			flangle;
		static float	sr, sp, sy, cr, cp, cy;

		flangle = velocity_angle.y * (MATH::_2PI / 360);
		sy = sin(flangle);
		cy = cos(flangle);

		flangle = velocity_angle.x * (MATH::_2PI / 360);
		sp = sin(flangle);
		cp = cos(flangle);

		flangle = velocity_angle.z * (MATH::_2PI / 360);
		sr = sin(flangle);
		cr = cos(flangle);

		forward.x = cp * cy;
		forward.y = cp * sy;
		forward.z = -sp;
		float stop_power = std::min<float>(speed * 2.0f, get_max_player_speed());
		pCmd->csgoUserCmd.pBaseCmd->flForwardMove = forward.x * stop_power;
		pCmd->csgoUserCmd.pBaseCmd->flSideMove = forward.y * -stop_power;

	}
}

static QAngle_t GetStandaloneRCS(CBaseUserCmdPB* pCmd, C_CSPlayerPawn* pLocal)
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

static QAngle_t GetAngularDifference(CBaseUserCmdPB* pCmd, Vector_t vecTarget, C_CSPlayerPawn* pLocal)
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

static float GetAngularDistance(CBaseUserCmdPB* pCmd, Vector_t vecTarget, C_CSPlayerPawn* pLocal)
{
	return GetAngularDifference(pCmd, vecTarget, pLocal).Length2D();
}

static std::vector<unsigned int> GetMenuHitboxes()
{
	//"Head",
	//"Chest",
	//"Stomach",
	//"Pelvis",
	//"Arms",
	//"Legs",
	//"Feet"
	std::vector<unsigned int> BoneVectors;
	unsigned int iBoneArray = C_GET(unsigned int, Vars.nRageBotHitboxes);
	for (int i = 0; i < 32; ++i) // 32 bits in unsigned int
	{
		if (iBoneArray & (1u << i))
		{
			if (i == 0) // Head
				BoneVectors.push_back((unsigned int)HEAD);
			if (i == 1) // Chest
				BoneVectors.push_back((unsigned int)CHEST);
			if (i == 2) // Stomach
				BoneVectors.push_back((unsigned int)STOMACH);
			if (i == 3) // Pelivs
				BoneVectors.push_back((unsigned int)PELVIS);
			if (i == 4) // Arms
			{
				BoneVectors.push_back((unsigned int)L_ARM);
				BoneVectors.push_back((unsigned int)R_ARM);
			}
			if (i == 5) // Legs
			{
				BoneVectors.push_back((unsigned int)R_LEG);
				BoneVectors.push_back((unsigned int)L_LEG);
			}
			if (i == 6) // Feets
			{
				BoneVectors.push_back((unsigned int)R_FEET);
				BoneVectors.push_back((unsigned int)L_FEET);
			}
		}
	}
	return BoneVectors;
}

__forceinline Vector_t CalculateSpread(C_CSWeaponBase* weapon, int seed, float inaccuracy, float spread, bool revolver2 = false)
{
	const char* item_def_index;
	float      recoil_index, r1, r2, r3, r4, s1, c1, s2, c2;

	if (!weapon)
		return { };
	// if we have no bullets, we have no spread.
	auto wep_info = weapon->GetWeaponVData();
	if (!wep_info)
		return { };

	// get some data for later.
	item_def_index = wep_info->GetName();
	recoil_index = weapon->GetRecoilIndex();

	MATH::fnRandomSeed((seed & 0xff) + 1);

	// generate needed floats.
	r1 = MATH::fnRandomFloat(0.f, 1.f);
	r2 = MATH::fnRandomFloat(0.f, 3.14159265358979323846264338327950288f * 2);
	r3 = MATH::fnRandomFloat(0.f, 1.f);
	r4 = MATH::fnRandomFloat(0.f, 3.14159265358979323846264338327950288f * 2);

	// revolver secondary spread.
	if (item_def_index == ("weapon_revoler") && revolver2) {
		r1 = 1.f - (r1 * r1);
		r3 = 1.f - (r3 * r3);
	}

	// negev spread.
	else if (item_def_index == ("weapon_negev") && recoil_index < 3.f) {
		for (int i{ 3 }; i > recoil_index; --i) {
			r1 *= r1;
			r3 *= r3;
		}

		r1 = 1.f - r1;
		r3 = 1.f - r3;
	}

	// get needed sine / cosine values.
	c1 = std::cos(r2);
	c2 = std::cos(r4);
	s1 = std::sin(r2);
	s2 = std::sin(r4);

	// calculate spread vector.
	return {
		(c1 * (r1 * inaccuracy)) + (c2 * (r3 * spread)),
		(s1 * (r1 * inaccuracy)) + (s2 * (r3 * spread)),
		0.f
	};
}

Vector_t calculate_spread_angles(Vector_t angle, int random_seed, float weapon_inaccuracy, float weapon_spread)
{
	MATH::fnRandomSeed(random_seed + 1);

	float r1 = MATH::fnRandomFloat(0.f, 1.f);
	float r2 = MATH::fnRandomFloat(0.f, MATH::_2PI);
	float r3 = MATH::fnRandomFloat(0.f, 1.f);
	float r4 = MATH::fnRandomFloat(0.f, MATH::_2PI);

	Vector_t spread = {
		(std::cos(r2) * (r1 * weapon_inaccuracy)) + (std::cos(r4) * (r3 * weapon_spread)),
		(std::sin(r2) * (r1 * weapon_inaccuracy)) + (std::sin(r4) * (r3 * weapon_spread))
	};

	Vector_t forward, right, up;

	float			flangle;
	static float	sr, sp, sy, cr, cp, cy;

	flangle = angle.y * (MATH::_2PI / 360);
	sy = sin(flangle);
	cy = cos(flangle);

	flangle = angle.x * (MATH::_2PI / 360);
	sp = sin(flangle);
	cp = cos(flangle);

	flangle = angle.z * (MATH::_2PI / 360);
	sr = sin(flangle);
	cr = cos(flangle);

	forward.x = cp * cy;
	forward.y = cp * sy;
	forward.z = -sp;

	right.x = (-1 * sr * sp * cy + -1 * cr * -sy);
	right.y = (-1 * sr * sp * sy + -1 * cr * cy);
	right.z = -1 * sr * cp;

	up.x = (cr * sp * cy + -sr * -sy);
	up.y = (cr * sp * sy + -sr * cy);
	up.z = cr * cp;
	
	return (forward + (right * spread.x) + (up * spread.y)).Normalized();
}

static int hitchance(C_CSPlayerPawn* pawn, Vector_t angles, C_CSWeaponBase* active_weapon, CCSWeaponBaseVData* weapon_data, bool no_spread)
{
	if (!SDK::LocalController->IsPawnAlive() || !pawn)
		return 0;
	if (no_spread)
		return 100;

	constexpr int seeds = 255;
	struct SpreadSeed {
		float r1, r2, r3, r4;
	};
	static std::vector<SpreadSeed> precomputedSeeds;
	static bool seedsInitialized = false;

	// Precompute seeds only once
	if (!seedsInitialized) {
		precomputedSeeds.resize(seeds);
		for (int seed = 0; seed < seeds; ++seed) {
			MATH::fnRandomSeed(seed + 1);
			precomputedSeeds[seed].r1 = MATH::fnRandomFloat(0.f, 1.f);
			precomputedSeeds[seed].r2 = MATH::fnRandomFloat(0.f, MATH::_2PI);
			precomputedSeeds[seed].r3 = MATH::fnRandomFloat(0.f, 1.f);
			precomputedSeeds[seed].r4 = MATH::fnRandomFloat(0.f, MATH::_2PI);
		}
		seedsInitialized = true;
	}

	Vector_t shoot_pos = SDK::LocalPawn->GetEyePosition();
	active_weapon->UpdateAccuracyPenalty();
	const float spread = active_weapon->GetSpread();
	const float inaccuracy = active_weapon->GetInaccuracy();
	int hits = 0;

	const int num_threads = std::thread::hardware_concurrency();
	int seeds_per_thread = seeds / num_threads;

	std::vector<std::future<int>> futures;

	for (int t = 0; t < num_threads; ++t) {
		int start = t * seeds_per_thread;
		int end = (t == num_threads - 1) ? seeds : start + seeds_per_thread;

		futures.push_back(std::async(std::launch::async, [=]() {
			int local_hits = 0;
			for (int seed = start; seed < end; ++seed) {
				// Use precomputed randoms
				const auto& s = precomputedSeeds[seed];
				Vector_t spreadVec = {
					(std::cos(s.r2) * (s.r1 * inaccuracy)) + (std::cos(s.r4) * (s.r3 * spread)),
					(std::sin(s.r2) * (s.r1 * inaccuracy)) + (std::sin(s.r4) * (s.r3 * spread)),
					0.f
				};

				// Calculate direction
				Vector_t forward, right, up;
				QAngle_t ang(angles.x, angles.y, angles.z);
				ang.ToDirections(&forward, &right, &up);
				Vector_t spread_angle = forward + (right * spreadVec.x) + (up * spreadVec.y);
				spread_angle.NormalizeInPlace();

				Vector_t result = spread_angle * weapon_data->GetRange() + shoot_pos;

				Ray_t ray{};
				GameTrace_t trace{};
				TraceFilter_t filter{};

				I::GameTraceManager->InitTraceFilter(filter, SDK::LocalPawn, MASK_SHOT, 0x3, 0x7);
				I::GameTraceManager->TraceShape(&ray, shoot_pos, result, &filter, &trace);
				I::GameTraceManager->ClipRayToEntity(&ray, shoot_pos, result, pawn, &filter, &trace);

				if (trace.m_pHitEntity && trace.m_pHitEntity->IsBasePlayerPawn() && trace.m_pHitEntity->GetRefEHandle().GetEntryIndex() == pawn->GetRefEHandle().GetEntryIndex())
					local_hits++;
			}
			return local_hits;
			}));
	}

	hits = 0;
	for (auto& f : futures)
		hits += f.get();

	/*
	for (int seed = 0; seed < seeds; seed++)
	{
		// Use precomputed randoms
		const auto& s = precomputedSeeds[seed];
		Vector_t spreadVec = {
			(std::cos(s.r2) * (s.r1 * inaccuracy)) + (std::cos(s.r4) * (s.r3 * spread)),
			(std::sin(s.r2) * (s.r1 * inaccuracy)) + (std::sin(s.r4) * (s.r3 * spread)),
			0.f
		};

		// Calculate direction
		Vector_t forward, right, up;
		QAngle_t ang(angles.x, angles.y, angles.z);
		ang.ToDirections(&forward, &right, &up);
		Vector_t spread_angle = forward + (right * spreadVec.x) + (up * spreadVec.y);
		spread_angle.NormalizeInPlace();

		Vector_t result = spread_angle * weapon_data->GetRange() + shoot_pos;

		Ray_t ray{};
		GameTrace_t trace{};
		TraceFilter_t filter{};

		I::GameTraceManager->InitTraceFilter(filter, SDK::LocalPawn, MASK_SHOT, 0x3, 0x7);
		I::GameTraceManager->TraceShape(&ray, shoot_pos, result, &filter, &trace);
		I::GameTraceManager->ClipRayToEntity(&ray, shoot_pos, result, pawn, &filter, &trace);

		if (trace.m_pHitEntity && trace.m_pHitEntity->IsBasePlayerPawn() && trace.m_pHitEntity->GetRefEHandle().GetEntryIndex() == pawn->GetRefEHandle().GetEntryIndex())
			hits++;
	}
	*/

	return static_cast<int>((static_cast<float>(hits) / seeds) * 100.f);
}

Vector_t aim_direction(Vector_t src, Vector_t dst)
{
	Vector_t result;

	Vector_t delta = dst - src;

	vector_angles(delta, result);

	return result;
}

bool CanShoot(C_CSPlayerPawn* pawn, C_CSWeaponBase* weapon, CUserCmd* pCmd)
{
	if (!weapon || weapon->GetClip1() <= 0)
		return false;
	auto weapon_data = weapon->GetWeaponVData();
	if (!weapon_data)
		return false;
	if (weapon_data->IsFullAuto())
		return true;

	if (pawn->IsWaitForNoAttack())
		return false;
	auto tick = pCmd->csgoUserCmd.pBaseCmd->nClientTick;
	if (weapon->GetNextPrimaryAttackTick() > tick)
		return false;
	if (weapon->GetAttributeManager()->GetItem()->GetItemDefinitionIndex() == WEAPON_DESERT_EAGLE)
	{
		if (weapon->PostPoneFireTicks() > tick)
			return false;
	}

	if (weapon->GetAttributeManager()->GetItem()->GetItemDefinitionIndex() == WEAPON_GLOCK_18)
	{
		if (weapon->PostPoneFireTicks() > tick)
			return false;
	}

	if (weapon->GetAttributeManager()->GetItem()->GetItemDefinitionIndex() == WEAPON_R8_REVOLVER)
	{
		if (weapon->PostPoneFireTicks() > tick)
			return false;
	}


	return true;
}

std::vector<Enemy_t> GetAllValidTargets(C_CSPlayerPawn* pLocalPawn, CCSPlayerController* pLocalController)
{
	std::vector<Enemy_t> vecEntityList;

	const int iHighestIndex = 126;
	for (int nIndex = 1; nIndex < iHighestIndex; nIndex++)
	{
		// Get the entity
		C_BaseEntity* pEntity = I::GameResourceService->pGameEntitySystem->Get(nIndex);
		if (pEntity == nullptr)
			continue;

		// check if it's an actual player
		SchemaClassInfoData_t* pClassInfo = nullptr;
		pEntity->GetSchemaClassInfo(&pClassInfo);
		if (pClassInfo == nullptr)
			continue;
		const FNV1A_t uHashedName = FNV1A::Hash(pClassInfo->szName);
		if (uHashedName != FNV1A::HashConst("CCSPlayerController"))
			continue;
		// Cast to player controller
		CCSPlayerController* pPlayer = reinterpret_cast<CCSPlayerController*>(pEntity);
		if (pPlayer == nullptr)
			continue;

		// Filter out the LocalPlayer
		if (pPlayer == pLocalController)
			continue;

		// Get the player pawn
		C_CSPlayerPawn* pPawn = I::GameResourceService->pGameEntitySystem->Get<C_CSPlayerPawn>(pPlayer->GetPawnHandle());
		if (pPawn == nullptr)
			continue;

		// Make sure it's a valid target
		if (!pPlayer->IsPawnAlive())
			continue;
		if (!pLocalPawn->IsOtherEnemy(pPawn))
			continue;
		if (pPawn->IsImmune())
			continue;
		Enemy_t currentEnemy = { pPawn, pPlayer };
		vecEntityList.push_back(currentEnemy);
	}
	return vecEntityList;
}

void Ragebot(CUserCmd* pCmd, CBaseUserCmdPB* pBaseCmd, CCSPlayerController* pLocalController, C_CSPlayerPawn* pLocalPawn)
{
	// Early out for all basic checks
	if (!C_GET(bool, Vars.bRageBot) ||
		(!C_GET(bool, Vars.bRageBotAlwaysOn) && !IPT::IsKeyDown(C_GET(unsigned int, Vars.nRageBotActivationKey))))
		return;
	bool bAutoStopped = false;
	bool bScoped = false;
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
	if (iWeaponType == WEAPONTYPE_C4 || iWeaponType == WEAPONTYPE_KNIFE ||
		iWeaponType == WEAPONTYPE_EQUIPMENT || iWeaponType == WEAPONTYPE_GRENADE)
		return;
	if (pWeapon->GetNextPrimaryAttackTickRatio() < 0)
		return;

	float minDamage = C_GET(float, Vars.flMinDmg);
	float minDamageOverride = C_GET(float, Vars.flMinDmgOverride);
	float hitchanceThreshold = C_GET(float, Vars.flHitchance);

	if (IPT::IsKeyDown(C_GET(unsigned int, Vars.nRageBotMinDamageKey)) &&
		C_GET(unsigned int, Vars.nRageBotMinDamageKey) != NULL)
		minDamage = minDamageOverride;

	auto localPosition = pLocalPawn->GetEyePosition();
	auto aimPunch = GetStandaloneRCS(pBaseCmd, pLocalPawn);

	// Only check CanAttack once per tick
	if (!pLocalPawn->CanAttack(TICKS_TO_TIME(pCmd->csgoUserCmd.pBaseCmd->nClientTick)))
		return;

	// Get all valid targets once
	std::vector<Enemy_t> vecTargets = GetAllValidTargets(pLocalPawn, pLocalController);
	static std::vector<unsigned int> cachedHitboxes;
	static unsigned int lastHitboxMask = 0;
	unsigned int currentMask = C_GET(unsigned int, Vars.nRageBotHitboxes);
	if (currentMask != lastHitboxMask) {
		cachedHitboxes = GetMenuHitboxes();
		lastHitboxMask = currentMask;
	}
	const std::vector<unsigned int>& BoneArray = cachedHitboxes;

	CCSPlayerController* pTarget = nullptr;
	Vector_t vecBestPosition;
	float bestDmg = 0.0f;

	for (const auto& enemy : vecTargets)
	{
		auto pPawn = enemy.pPawn;
		CGameSceneNode* pCGameSceneNode = pPawn->GetGameSceneNode();
		if (!pCGameSceneNode || pCGameSceneNode->IsDormant())
			continue;

		CSkeletonInstance* pSkeleton = pCGameSceneNode->GetSkeletonInstance();
		if (!pSkeleton || !pSkeleton->pBoneCache)
			continue;
		if (pPawn->IsImmune())
			continue;

		Matrix2x4_t* pBoneCache = pSkeleton->pBoneCache;
		auto entity = I::GameResourceService->pGameEntitySystem->Get(pPawn->GetRefEHandle());
		auto localEnt = I::GameResourceService->pGameEntitySystem->Get(pLocalPawn->GetRefEHandle());
		if (!entity || !localEnt)
			continue;

		for (unsigned int Bone : BoneArray)
		{
			Vector_t vecPos = pBoneCache->GetOrigin(Bone);
			data_t data;

			float damage = 0.0f;
			bool canHit = false;
			WALL::Penetrate(data, localPosition, vecPos, entity, localEnt, pLocalPawn, pPawn, pWeaponVData, damage, canHit);

			auto vNewAng = aim_direction(localPosition, vecPos);
			vNewAng.x += aimPunch.x * 2.0f;
			vNewAng.y += aimPunch.y * 2.0f;

			if ((data.m_dmg < minDamage && data.m_dmg < pPawn->GetHealth()) || bestDmg > data.m_dmg)
				continue;

			if (hitchanceThreshold > 0)
			{
				if (hitchance(pPawn, vNewAng, pWeapon, pWeaponVData, false) < hitchanceThreshold)
				{
					if (!bAutoStopped)
					{
						if (CanShoot(pLocalPawn, pWeapon, pCmd) && C_GET(bool, Vars.bAutoStop))
							autoStop(pCmd, pLocalPawn);
						bAutoStopped = true;
					}
					if (!bScoped)
					{
						if (pWeaponVData->GetWeaponType() != WEAPONTYPE_SNIPER_RIFLE)
							return;
						auto pActiveWeaponBaseGun = I::GameResourceService->pGameEntitySystem->Get<C_CSWeaponBaseGun>(pWeapon->GetRefEHandle());
						if (!pActiveWeaponBaseGun)
							return;
						if (pActiveWeaponBaseGun->GetZoomLevel() == 0)
						{
							I::Engine->ClientCmd_Unrestricted("+attack2");
							I::Engine->ClientCmd_Unrestricted("-attack2");
						}
					}
					continue;
				}
			}
			bestDmg = data.m_dmg;
			pTarget = enemy.pController;
			vecBestPosition = vecPos;
			if (data.m_dmg >= pPawn->GetHealth()) // if we can already kill the target, don't look further
				break;
		}
		if (bestDmg >= pPawn->GetHealth())
			break;
	}
	if (!pTarget)
		return;

	QAngle_t ogAngles = pBaseCmd->pViewAngles->angValue;
	QAngle_t* pViewAngles = &(pBaseCmd->pViewAngles->angValue);

	QAngle_t vNewAngles = GetAngularDifference(pBaseCmd, vecBestPosition, pLocalPawn);
	QAngle_t subTickAngle = ogAngles + (vNewAngles + aimPunch * 2.0f);

	if (C_GET(bool, Vars.bRageSilentAim))
		pCmd->SetSubTickAngle(subTickAngle);
	else
	{
		pCmd->SetSubTickAngle(subTickAngle);
		pViewAngles->x += (vNewAngles.x + aimPunch.x * 2.0f);
		pViewAngles->y += (vNewAngles.y + aimPunch.y * 2.0f);
		pViewAngles->Normalize();
		pViewAngles->Clamp();
	}

	if (C_GET(bool, Vars.bAutoFire) && C_GET(bool, Vars.bRageSilentAim))
	{
		pCmd->nButtons.nValue |= IN_ATTACK;

		auto press = pCmd->csgoUserCmd.pBaseCmd->add_subtick_move();
		press->nCachedBits = 7;
		press->nButton = IN_ATTACK;
		press->bPressed = true;
		press->flWhen = 0.01f;
		pCmd->SetAttackHistory(pCmd->csgoUserCmd.inputHistoryField.nCurrentSize - 1);

		auto release = pCmd->csgoUserCmd.pBaseCmd->add_subtick_move();
		release->nCachedBits = 7;
		release->nButton = IN_ATTACK;
		release->bPressed = false;
		release->flWhen = 0.02f;
		pCmd->SetAttackHistory(pCmd->csgoUserCmd.inputHistoryField.nCurrentSize - 1);
	}
	else if (C_GET(bool, Vars.bAutoFire) && !C_GET(bool, Vars.bRageSilentAim))
	{
		I::Engine->ClientCmd_Unrestricted("+attack");
		I::Engine->ClientCmd_Unrestricted("-attack");
	}
}

unsigned int get_hash_seed(C_CSPlayerPawn* player, Vector_t angle, int attack_tick)
{
	using funct = unsigned int(__fastcall*)(C_CSPlayerPawn*, Vector_t, int);
	static funct fns = reinterpret_cast<funct>(MEM::FindPattern(CLIENT_DLL, ("48 89 5C 24 ? 57 48 81 EC ? ? ? ? F3 0F 10 0A")));
	return fns(player, angle, attack_tick);
}

uint32_t GetSeed(const QAngle_t& angles, int tick) {
	struct HashBuffer {
		float pitch;
		float yaw;
		int tick;
	};

	HashBuffer buffer;

	float pitch = angles.x;
	pitch += pitch;
	buffer.pitch = std::floor(pitch) * 0.5f;

	float yaw = angles.y;
	yaw += yaw;
	buffer.yaw = std::floor(yaw) * 0.5f;

	buffer.tick = tick;

	CSHA1 sha1;
	sha1.Reset();
	sha1.Update(reinterpret_cast<uint8_t*>(&buffer), sizeof(buffer));
	sha1.Final();

	return *reinterpret_cast<uint32_t*>(sha1.GetDigest());
}

int get_player_tick(std::uintptr_t* container) {
	using funct = int(__fastcall*)(std::uintptr_t*, std::uintptr_t*, char);
	static funct fn = reinterpret_cast<funct>(MEM::FindPattern(CLIENT_DLL, "48 89 5C 24 08 57 48 83 EC 20 33 C9 41"));
	return fn(nullptr, container, 0);
}

Vector_t CalculateNoSpreadAngle(unsigned int seed, int shots_fired, float inaccuracy, float spread, unsigned short item_index, int recoil_index)
{
	MATH::fnRandomSeed(seed);

	float r1 = MATH::fnRandomFloat(0.0f, 1.0f);
	float r2 = MATH::fnRandomFloat(0.0f, MATH::_2PI);
	float r3 = MATH::fnRandomFloat(0.0f, 1.0f);
	float r4 = MATH::fnRandomFloat(0.0f, MATH::_2PI);

	// Apply weapon-specific spread modifications
	if (item_index == 64) // R8 Revolver
	{
		r1 = 1.f - (r1 * r1);
		r3 = 1.f - (r3 * r3);
	}
	else if (item_index == 28 && recoil_index < 3) // Negev
	{
		for (int i = 3; i > recoil_index; --i) {
			r1 *= r1;
			r3 *= r3;
		}
		r1 = 1.f - r1;
		r3 = 1.f - r3;
	}

	// Calculate sine/cosine values
	float c1 = std::cosf(r2);
	float c2 = std::cosf(r4);
	float s1 = std::sinf(r2);
	float s2 = std::sinf(r4);

	// Calculate final spread vector
	return Vector_t(
		(c1 * (r1 * inaccuracy)) + (c2 * (r3 * spread)),
		(s1 * (r1 * inaccuracy)) + (s2 * (r3 * spread)),
		0.0f
	);
}


Vector_t CalculateNoSpreadAngles(const QAngle_t& aimAngles, C_CSPlayerPawn* pLocalPawn, C_CSWeaponBase* pWeapon, int currentTick)
{
	if (!pLocalPawn || !pWeapon)
		return Vector_t(aimAngles.x, aimAngles.y, aimAngles.z);

	auto pWeaponVData = pWeapon->GetWeaponVData();
	if (!pWeaponVData)
		return Vector_t(aimAngles.x, aimAngles.y, aimAngles.z);

	pWeapon->UpdateAccuracyPenalty();
	float inaccuracy = pWeapon->GetInaccuracy();
	float spread = pWeapon->GetSpread();
	auto nWeaponDefinitionIndex = pWeapon->GetAttributeManager()->GetItem()->GetItemDefinitionIndex();

	constexpr int k_max_seeds = 255;

	// Iterate all possible seeds to find the correct no-spread angle
	for (int i = 0; i < k_max_seeds; ++i)
	{
		// Create spread angles for seed calculation
		QAngle_t spreadAngles(static_cast<float>(i) / 2.0f, aimAngles.y, 0.0f);

		// Calculate the spread seed
		unsigned int spreadSeed = GetSeed(spreadAngles, currentTick);
		spreadSeed = get_hash_seed(pLocalPawn, Vector_t(spreadAngles.x, spreadAngles.y, spreadAngles.z), currentTick);

		// Calculate the spread using our existing function
		Vector_t calculatedSpread = CalculateNoSpreadAngle(
			spreadSeed + 1,
			pLocalPawn->GetShotsFired(),
			inaccuracy,
			spread,
			nWeaponDefinitionIndex,
			pWeapon->GetRecoilIndex()
		);

		// Calculate no-spread angles
		QAngle_t noSpreadAngles;
		noSpreadAngles.x = aimAngles.x + M_RAD2DEG(std::atan(std::sqrt(calculatedSpread.x * calculatedSpread.x + calculatedSpread.y * calculatedSpread.y)));
		noSpreadAngles.y = aimAngles.y;
		noSpreadAngles.z = -M_RAD2DEG(std::atan2(calculatedSpread.x, calculatedSpread.y));

		Vector_t vecFinalAngles = Vector_t(spreadAngles.x, spreadAngles.y, spreadAngles.z);
		vecFinalAngles += calculatedSpread;

		// Verify if this is the correct seed
		if (GetSeed(QAngle_t(vecFinalAngles.x, vecFinalAngles.y, vecFinalAngles.z), currentTick) == spreadSeed)
			return vecFinalAngles;
	}

	// Fallback to original angles if no valid seed found
	return Vector_t(aimAngles.x, aimAngles.y, aimAngles.z);
}

void NoSpreadRagebot(CUserCmd* pCmd, CBaseUserCmdPB* pBaseCmd, CCSPlayerController* pLocalController, C_CSPlayerPawn* pLocalPawn)
{
	// Early validation checks
	if (!C_GET(bool, Vars.bRageBot) || (!C_GET(bool, Vars.bRageBotAlwaysOn) && !IPT::IsKeyDown(C_GET(unsigned int, Vars.nRageBotActivationKey))))
		return;

	// Weapon validation
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

	// Validate weapon type
	int iWeaponType = pWeaponVData->GetWeaponType();
	if (iWeaponType == WEAPONTYPE_C4 || iWeaponType == WEAPONTYPE_KNIFE || iWeaponType == WEAPONTYPE_EQUIPMENT || iWeaponType == WEAPONTYPE_GRENADE)
		return;
	if (pWeapon->GetNextPrimaryAttackTickRatio() < 0)
		return;

	// Damage settings
	float minDamage = C_GET(float, Vars.flMinDmg);
	if (IPT::IsKeyDown(C_GET(unsigned int, Vars.nRageBotMinDamageKey)) && C_GET(unsigned int, Vars.nRageBotMinDamageKey) != NULL)
		minDamage = C_GET(float, Vars.flMinDmgOverride);

	// Player and weapon state
	auto localPosition = pLocalPawn->GetEyePosition();
	auto aimPunch = GetStandaloneRCS(pBaseCmd, pLocalPawn);

	// Target finding variables
	CCSPlayerController* pTarget = nullptr;
	Vector_t vecBestPosition;
	float bestDmg = 0;

	// Get all valid targets
	std::vector<Enemy_t> vecTargets = GetAllValidTargets(pLocalPawn, pLocalController);
	static std::vector<unsigned int> cachedHitboxes;
	static unsigned int lastHitboxMask = 0;
	unsigned int currentMask = C_GET(unsigned int, Vars.nRageBotHitboxes);
	if (currentMask != lastHitboxMask) {
		cachedHitboxes = GetMenuHitboxes();
		lastHitboxMask = currentMask;
	}
	const std::vector<unsigned int>& BoneArray = cachedHitboxes;

	// Find best target using existing logic
	for (const auto& enemy : vecTargets)
	{
		auto pPawn = enemy.pPawn;
		CGameSceneNode* pCGameSceneNode = pPawn->GetGameSceneNode();
		if (!pCGameSceneNode || pCGameSceneNode->IsDormant())
			continue;

		CSkeletonInstance* pSkeleton = pCGameSceneNode->GetSkeletonInstance();
		if (!pSkeleton || !pSkeleton->pBoneCache)
			continue;
		if (pPawn->IsImmune())
			continue;

		Matrix2x4_t* pBoneCache = pSkeleton->pBoneCache;
		auto entity = I::GameResourceService->pGameEntitySystem->Get(pPawn->GetRefEHandle());
		auto localEnt = I::GameResourceService->pGameEntitySystem->Get(pLocalPawn->GetRefEHandle());
		if (!entity || !localEnt)
			continue;

		for (unsigned int Bone : BoneArray)
		{
			Vector_t vecPos = pBoneCache->GetOrigin(Bone);

			// Penetration check
			data_t data;
			float damage = 0.0f;
			bool canHit = false;
			WALL::Penetrate(data, localPosition, vecPos, entity, localEnt, pLocalPawn, pPawn, pWeaponVData, damage, canHit);

			if (data.m_dmg < minDamage && data.m_dmg < pPawn->GetHealth())
				continue;

			if (bestDmg > data.m_dmg)
				continue;

			// Found better target
			bestDmg = data.m_dmg;
			pTarget = enemy.pController;
			vecBestPosition = vecPos;
			if (data.m_dmg >= pPawn->GetHealth()) // Early out for lethal shots
				goto target_found;
		}
		if (bestDmg >= pPawn->GetHealth())
			break;
	}

target_found:
	// No target found
	if (pTarget == nullptr)
		return;

	// Calculate aim angles to target
	Vector_t targetDir = vecBestPosition - localPosition;
	Vector_t targetAngleVec;
	vector_angles(targetDir, targetAngleVec);

	// Apply recoil compensation
	QAngle_t aimAngles(targetAngleVec.x + aimPunch.x * 2.0f,
		targetAngleVec.y + aimPunch.y * 2.0f,
		targetAngleVec.z);

	// Get current tick for seed calculation
	int currentTick = pLocalController->GetTickBase();

	// Calculate no-spread angles using the new algorithm
	Vector_t noSpreadAngles = CalculateNoSpreadAngles(aimAngles, pLocalPawn, pWeapon, currentTick);
	QAngle_t finalAngle(noSpreadAngles.x, noSpreadAngles.y, noSpreadAngles.z);

	// Apply angles
	if (C_GET(bool, Vars.bRageSilentAim))
	{
		pCmd->SetSubTickAngle(finalAngle);
	}
	else
	{
		pCmd->SetSubTickAngle(finalAngle);
		pBaseCmd->pViewAngles->angValue = finalAngle;
		pBaseCmd->pViewAngles->angValue.Normalize();
	}

	// Handle auto-fire
	if (C_GET(bool, Vars.bAutoFire))
	{
		if (C_GET(bool, Vars.bRageSilentAim))
		{
			pCmd->nButtons.nValue |= IN_ATTACK;

			auto press = pCmd->csgoUserCmd.pBaseCmd->add_subtick_move();
			press->nCachedBits = 7;
			press->nButton = IN_ATTACK;
			press->bPressed = true;
			press->flWhen = 0.01f;
			pCmd->SetAttackHistory(pCmd->csgoUserCmd.inputHistoryField.nCurrentSize - 1);

			auto release = pCmd->csgoUserCmd.pBaseCmd->add_subtick_move();
			release->nCachedBits = 7;
			release->nButton = IN_ATTACK;
			release->bPressed = false;
			release->flWhen = 0.02f;
			pCmd->SetAttackHistory(pCmd->csgoUserCmd.inputHistoryField.nCurrentSize - 1);
		}
		else
		{
			I::Engine->ClientCmd_Unrestricted("+attack");
			I::Engine->ClientCmd_Unrestricted("-attack");
		}
	}
}



void F::RAGEBOT::RAGE::OnMove(CUserCmd* pCmd, CBaseUserCmdPB* pBaseCmd, CCSPlayerController* pLocalController, C_CSPlayerPawn* pLocalPawn)
{
	if (!I::Engine->IsConnected() || !I::Engine->IsInGame())
		return;
	if (!pLocalController->IsPawnAlive())
		return;
	if (C_GET(bool, Vars.bRageBot))
	{
		if (C_GET(bool, Vars.bNoSpread))
			NoSpreadRagebot(pCmd, pBaseCmd, pLocalController, pLocalPawn);
		else
			Ragebot(pCmd, pBaseCmd, pLocalController, pLocalPawn);
	}
}
