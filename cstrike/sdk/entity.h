#pragma once

// @test: using interfaces in the header | not critical but could blow up someday with thousands of errors or affect to compilation time etc
// used: cgameentitysystem, ischemasystem
#include "../core/interfaces.h"
#include "interfaces/igameresourceservice.h"
#include "interfaces/ischemasystem.h"
#include "source2-sdk/types/chandle.hpp"

// used: schema field
#include "../core/schema.h"

// used: l_print
#include "../utilities/log.h"
// used: vector_t
#include "datatypes/vector.h"
// used: qangle_t
#include "datatypes/qangle.h"
// used: ctransform
#include "datatypes/transform.h"

// used: cbasehandle
#include "entity_handle.h"
// used: game's definitions
#include "const.h"
// used: entity vdata
#include "vdata.h"
#include "../core/hooks.h"

using GameTime_t = std::float_t;
using GameTick_t = std::int32_t;

struct SOID_t {
	uint64_t m_id;
	uint32_t m_type;
	uint32_t m_padding;
};

enum ESOCacheEvent {
	/// Dummy sentinel value
	eSOCacheEvent_None = 0,

	/// We received a our first update from the GC and are subscribed
	eSOCacheEvent_Subscribed = 1,

	/// We lost connection to GC or GC notified us that we are no longer
	/// subscribed. Objects stay in the cache, but we no longer receive updates
	eSOCacheEvent_Unsubscribed = 2,

	/// We received a full update from the GC on a cache for which we were
	/// already subscribed. This can happen if connectivity is lost, and then
	/// restored before we realized it was lost.
	eSOCacheEvent_Resubscribed = 3,

	/// We received an incremental update from the GC about specific object(s)
	/// being added, updated, or removed from the cache
	eSOCacheEvent_Incremental = 4,

	/// A lister was added to the cache
	/// @see CGCClientSharedObjectCache::AddListener
	eSOCacheEvent_ListenerAdded = 5,

	/// A lister was removed from the cache
	/// @see CGCClientSharedObjectCache::RemoveListener
	eSOCacheEvent_ListenerRemoved = 6,
};

enum HitGroup_t : std::uint32_t {
	HITGROUP_INVALID = -1,
	HITGROUP_GENERIC = 0,
	HITGROUP_HEAD = 1,
	HITGROUP_CHEST = 2,
	HITGROUP_STOMACH = 3,
	HITGROUP_LEFTARM = 4,
	HITGROUP_RIGHTARM = 5,
	HITGROUP_LEFTLEG = 6,
	HITGROUP_RIGHTLEG = 7,
	HITGROUP_NECK = 8,
	HITGROUP_UNUSED = 9,
	HITGROUP_GEAR = 10,
	HITGROUP_SPECIAL = 11,
	HITGROUP_COUNT = 12,
};

enum HitBoxes : uint32_t {
	HEAD = 6,
	NECK = 5,
	CHEST = 4,
	RIGHT_CHEST = 8,
	L_ARM = 9,
	LEFT_CHEST = 13,
	R_ARM = 14,
	STOMACH = 3,
	PELVIS = 2,
	CENTER = 1,
	L_LEG = 23,
	L_FEET = 24,
	R_LEG = 26,
	R_FEET = 27,
};

class CEconItem {
public:
	void set_sticker(int index, int id);
	void SetDynamicAttributeValue(int index, void* value);
	void SetDynamicAttributeValueString(int index, const char* value);

public:
	static CEconItem* CreateInstance();

	void Destruct() {
		return MEM::CallVFunc<void, 1U>(this);
	}

	void SetPaintKit(float kit) { SetDynamicAttributeValue(6, &kit); }
	void SetPaintSeed(float seed) { SetDynamicAttributeValue(7, &seed); }
	void SetPaintWear(float wear) { SetDynamicAttributeValue(8, &wear); }
	void SetStatTrak(int count) { SetDynamicAttributeValue(80, &count); }
	void SetStatTrakType(int type) { SetDynamicAttributeValue(81, &type); }
	void SetCustomName(const char* pName) {
		SetDynamicAttributeValueString(111, pName);
	}

	char pad0[0x10];  // 2 vtables
	uint64_t m_ulID;
	uint64_t m_ulOriginalID;
	void* m_pCustomDataOptimizedObject;
	uint32_t m_unAccountID;
	uint32_t m_unInventory;
	uint16_t m_unDefIndex;
	uint16_t m_unOrigin : 5;
	uint16_t m_nQuality : 4;
	uint16_t m_unLevel : 2;
	uint16_t m_nRarity : 4;
	uint16_t m_dirtybitInUse : 1;
	int16_t m_iItemSet;
	int m_bSOUpdateFrame;
	uint8_t m_unFlags;
};

class CSharedObject;

class CEntityInstance;

class CEntityIdentity
{
public:
	CS_CLASS_NO_INITIALIZER(CEntityIdentity);

	// @note: handle index is not entity index
	SCHEMA_ADD_OFFSET(std::uint32_t, GetIndex, 0x10);
	SCHEMA_ADD_FIELD(const char*, GetDesignerName, "CEntityIdentity->m_designerName");
	SCHEMA_ADD_FIELD(std::uint32_t, GetFlags, "CEntityIdentity->m_flags");

	[[nodiscard]] bool IsValid()
	{
		return GetIndex() != INVALID_EHANDLE_INDEX;
	}

	[[nodiscard]] int GetEntryIndex()
	{
		if (!IsValid())
			return ENT_ENTRY_MASK;
		
		return GetIndex() & ENT_ENTRY_MASK;
	}

	[[nodiscard]] int GetSerialNumber()
	{
		return GetIndex() >> NUM_SERIAL_NUM_SHIFT_BITS;
	}

	CEntityInstance* pInstance; // 0x00
};

class CEntityInstance
{
public:
	CS_CLASS_NO_INITIALIZER(CEntityInstance);

	void GetSchemaClassInfo(SchemaClassInfoData_t** pReturn)
	{
		return MEM::CallVFunc<void, 38U>(this, pReturn);
	}

	[[nodiscard]] CBaseHandle GetRefEHandle()
	{
		CEntityIdentity* pIdentity = GetIdentity();
		if (pIdentity == nullptr)
			return CBaseHandle();

		return CBaseHandle(pIdentity->GetEntryIndex(), pIdentity->GetSerialNumber() - (pIdentity->GetFlags() & 1));
	}

	SCHEMA_ADD_FIELD(CEntityIdentity*, GetIdentity, "CEntityInstance->m_pEntity");
};

class CCollisionProperty
{
public:
	std::uint16_t CollisionMask()
	{
		return *reinterpret_cast<std::uint16_t*>(reinterpret_cast<std::uintptr_t>(this) + 0x38);
	}

	CS_CLASS_NO_INITIALIZER(CCollisionProperty);

	SCHEMA_ADD_FIELD(Vector_t, GetMins, "CCollisionProperty->m_vecMins");
	SCHEMA_ADD_FIELD(Vector_t, GetMaxs, "CCollisionProperty->m_vecMaxs");

	SCHEMA_ADD_FIELD(std::uint8_t, GetSolidFlags, "CCollisionProperty->m_usSolidFlags");
	SCHEMA_ADD_FIELD(std::uint8_t, GetCollisionGroup, "CCollisionProperty->m_CollisionGroup");
};

class CSkeletonInstance;
class CGameSceneNode
{
public:
	CS_CLASS_NO_INITIALIZER(CGameSceneNode);

	SCHEMA_ADD_FIELD(CTransform, GetNodeToWorld, "CGameSceneNode->m_nodeToWorld");
	SCHEMA_ADD_FIELD(CEntityInstance*, GetOwner, "CGameSceneNode->m_pOwner");

	SCHEMA_ADD_FIELD(Vector_t, GetAbsOrigin, "CGameSceneNode->m_vecAbsOrigin");
	SCHEMA_ADD_FIELD(Vector_t, GetRenderOrigin, "CGameSceneNode->m_vRenderOrigin");

	SCHEMA_ADD_FIELD(QAngle_t, GetAngleRotation, "CGameSceneNode->m_angRotation");
	SCHEMA_ADD_FIELD(QAngle_t, GetAbsAngleRotation, "CGameSceneNode->m_angAbsRotation");

	SCHEMA_ADD_FIELD(bool, IsDormant, "CGameSceneNode->m_bDormant");

	CSkeletonInstance* GetSkeletonInstance()
	{
		return MEM::CallVFunc<CSkeletonInstance*, 8U>(this);
	}

	void SetMeshGroupMask(uint64_t meshGroupMask)
	{
		return H::SetMeshGroupMask(this, meshGroupMask);
	}
};

class C_BaseEntity : public CEntityInstance
{
public:
	CS_CLASS_NO_INITIALIZER(C_BaseEntity);

	[[nodiscard]] bool IsBasePlayerController()
	{
		SchemaClassInfoData_t* pClassInfo;
		GetSchemaClassInfo(&pClassInfo);
		if (pClassInfo == nullptr)
			return false;

		return FNV1A::Hash(pClassInfo->szName) == FNV1A::HashConst("C_CSPlayerController");
	}

	[[nodiscard]] bool IsBasePlayerPawn()
	{
		SchemaClassInfoData_t* pClassInfo;
		GetSchemaClassInfo(&pClassInfo);
		if (pClassInfo == nullptr)
			return false;

		return FNV1A::Hash(pClassInfo->szName) == FNV1A::HashConst("C_CSPlayerPawn");
	}
	
	[[nodiscard]] bool IsFuncMoveLinear()
	{
		SchemaClassInfoData_t* pClassInfo;
		GetSchemaClassInfo(&pClassInfo);
		if (pClassInfo == nullptr)
			return false;

		return FNV1A::Hash(pClassInfo->szName) == FNV1A::HashConst("C_FuncMoveLinear");
	}

	[[nodiscard]] bool IsWeapon()
	{
		static SchemaClassInfoData_t* pWeaponBaseClass = nullptr;
		if (pWeaponBaseClass == nullptr)
		I::SchemaSystem->FindTypeScopeForModule(CS_XOR("client.dll"))->FindDeclaredClass(&pWeaponBaseClass, CS_XOR("C_CSWeaponBase"));


		SchemaClassInfoData_t* pClassInfo;
		GetSchemaClassInfo(&pClassInfo);
		if (pClassInfo == nullptr)
			return false;

		return (pClassInfo->InheritsFrom(pWeaponBaseClass));
	}

	static C_BaseEntity* GetLocalPlayer();

	// get entity origin on scene
	[[nodiscard]] const Vector_t& GetSceneOrigin();

	SCHEMA_ADD_FIELD(CGameSceneNode*, GetGameSceneNode, "C_BaseEntity->m_pGameSceneNode");
	SCHEMA_ADD_FIELD(CCollisionProperty*, GetCollision, "C_BaseEntity->m_pCollision");
	SCHEMA_ADD_FIELD(std::uint8_t, GetTeam, "C_BaseEntity->m_iTeamNum");
	SCHEMA_ADD_FIELD(CBaseHandle, GetOwnerHandle, "C_BaseEntity->m_hOwnerEntity");
	SCHEMA_ADD_FIELD(Vector_t, GetBaseVelocity, "C_BaseEntity->m_vecBaseVelocity");
	SCHEMA_ADD_FIELD(Vector_t, GetAbsVelocity, "C_BaseEntity->m_vecAbsVelocity");
	SCHEMA_ADD_FIELD(bool, IsTakingDamage, "C_BaseEntity->m_bTakesDamage");
	SCHEMA_ADD_FIELD(std::uint32_t, GetFlags, "C_BaseEntity->m_fFlags");
	SCHEMA_ADD_FIELD(std::int32_t, GetEflags, "C_BaseEntity->m_iEFlags");
	SCHEMA_ADD_FIELD(std::uint8_t, GetMoveType, "C_BaseEntity->m_nActualMoveType"); // m_nActualMoveType returns CSGO style movetype, m_nMoveType returns bitwise shifted move type
	SCHEMA_ADD_FIELD(std::uint8_t, GetLifeState, "C_BaseEntity->m_lifeState");
	SCHEMA_ADD_FIELD(std::int32_t, GetHealth, "C_BaseEntity->m_iHealth");
	SCHEMA_ADD_FIELD(std::int32_t, GetMaxHealth, "C_BaseEntity->m_iMaxHealth");
	SCHEMA_ADD_FIELD(float, GetWaterLevel, "C_BaseEntity->m_flWaterLevel");
	SCHEMA_ADD_FIELD(std::uint64_t, GetSubclassID, "C_BaseEntity->m_nSubclassID");
	SCHEMA_ADD_FIELD_OFFSET(void*, GetVData, "C_BaseEntity->m_nSubclassID", 0x8);

	
	void UpdateSubClass()
	{
		H::GetPlayerPawn(this);
	}
	
	void UpdateVData()
	{
		CALL_VIRTUAL(void*, 182, this);
	}

	void SetBodyGroup() {
		static auto fn = reinterpret_cast<void(__fastcall*)(void*, int, unsigned int)>(MEM::FindPattern(CLIENT_DLL, "85 D2 0F 88 ? ? ? ? 55 56 57"));
		fn(this, 0, 1);
	}
};

class CGlowProperty;

class C_BaseModelEntity : public C_BaseEntity
{
public:
	CS_CLASS_NO_INITIALIZER(C_BaseModelEntity);

	SCHEMA_ADD_FIELD(CCollisionProperty, GetCollisionInstance, "C_BaseModelEntity->m_Collision");
	SCHEMA_ADD_FIELD(CGlowProperty, GetGlowProperty, "C_BaseModelEntity->m_Glow");
	SCHEMA_ADD_FIELD(Vector_t, GetViewOffset, "C_BaseModelEntity->m_vecViewOffset");
	SCHEMA_ADD_FIELD(GameTime_t, GetCreationTime, "C_BaseModelEntity->m_flCreateTime");
	SCHEMA_ADD_FIELD(GameTick_t, GetCreationTick, "C_BaseModelEntity->m_nCreationTick");
	SCHEMA_ADD_FIELD(CBaseHandle, GetMoveParent, "C_BaseModelEntity->m_hOldMoveParent");
	SCHEMA_ADD_FIELD(std::float_t, GetAnimTime, "C_BaseModelEntity->m_flAnimTime");
	SCHEMA_ADD_FIELD(std::float_t, GetSimulationTime, "C_BaseModelEntity->m_flSimulationTime");
	void SetModel(const char* name);
};

class CPlayer_ItemServices;
class CPlayer_CameraServices;

class CPlayer_WeaponServices
{
public:
	SCHEMA_ADD_FIELD(CBaseHandle, GetActiveWeapon, "CPlayer_WeaponServices->m_hActiveWeapon");
};

class CCSPlayer_WeaponServices : public CPlayer_WeaponServices
{
public:
	SCHEMA_ADD_FIELD(GameTime_t, GetNextAttack, "CCSPlayer_WeaponServices->m_flNextAttack");
}; 

class C_BasePlayerPawn : public C_BaseModelEntity
{
public:
	CS_CLASS_NO_INITIALIZER(C_BasePlayerPawn);

	SCHEMA_ADD_FIELD(CBaseHandle, GetControllerHandle, "C_BasePlayerPawn->m_hController");
	SCHEMA_ADD_FIELD(CCSPlayer_WeaponServices*, GetWeaponServices, "C_BasePlayerPawn->m_pWeaponServices");
	SCHEMA_ADD_FIELD(CPlayer_ItemServices*, GetItemServices, "C_BasePlayerPawn->m_pItemServices");
	SCHEMA_ADD_FIELD(CPlayer_CameraServices*, GetCameraServices, "C_BasePlayerPawn->m_pCameraServices");

	[[nodiscard]] Vector_t GetEyePosition()
	{
		Vector_t vecEyePosition = Vector_t(0.0f, 0.0f, 0.0f);
		// Credit: https://www.unknowncheats.me/forum/4258133-post6228.html
		MEM::CallVFunc<void, 169U>(this, &vecEyePosition);
		return vecEyePosition;
	}
};

class CCSPlayer_ViewModelServices {
public:
	SCHEMA_ADD_PFIELD(CBaseHandle, GetViewModel, "CCSPlayer_ViewModelServices->m_hViewModel");
	SCHEMA_ADD_FIELD(CBaseHandle, GetViewModel2, "CCSPlayer_ViewModelServices->m_hViewModel");
};

class C_CSPlayerPawnBase : public C_BasePlayerPawn
{
public:
	CS_CLASS_NO_INITIALIZER(C_CSPlayerPawnBase);

	SCHEMA_ADD_FIELD(CCSPlayer_ViewModelServices*, GetViewModelServices, "C_CSPlayerPawnBase->m_pViewModelServices");
	SCHEMA_ADD_FIELD(float, GetLowerBodyYawTarget, "C_CSPlayerPawnBase->m_flLowerBodyYawTarget");
	SCHEMA_ADD_FIELD(float, GetFlashMaxAlpha, "C_CSPlayerPawnBase->m_flFlashMaxAlpha");
	SCHEMA_ADD_FIELD(float, GetFlashDuration, "C_CSPlayerPawnBase->m_flFlashDuration");
	SCHEMA_ADD_FIELD(Vector_t, GetLastSmokeOverlayColor, "C_CSPlayerPawnBase->m_vLastSmokeOverlayColor");
	SCHEMA_ADD_FIELD(float, GetSpawnTimeIndex,  "C_CSPlayerPawnBase->m_flLastSpawnTimeIndex");
	SCHEMA_ADD_FIELD(int, GetSurvivalTeam, "C_CSPlayerPawnBase->m_nSurvivalTeam"); // danger zone
	SCHEMA_ADD_FIELD(bool, IsImmune, "C_CSPlayerPawnBase->m_bGunGameImmunity"); // danger zone
};

class C_EconItemView;

class EntitySpottedState_t
{
public:
	char pad0[8];              // 0x0 - 0x7
	bool bSpotted;             // 0x8
	char pad1[3];              // 0x9 - 0xB
	uint32_t bSpottedByMask[2];// 0xC, 0x10
};

class CCSPlayerController_ActionTrackingServices;
class CCSPlayer_BulletServices;
class C_CSPlayerPawn : public C_CSPlayerPawnBase
{
public:
	CS_CLASS_NO_INITIALIZER(C_CSPlayerPawn);

	[[nodiscard]] bool IsOtherEnemy(C_CSPlayerPawn* pOther);
	[[nodiscard]] int GetAssociatedTeam();
	[[nodiscard]] bool CanAttack(const float flServerTime);
	[[nodiscard]] std::uint32_t GetOwnerHandleIndex();
	[[nodiscard]] std::uint16_t GetCollisionMask();

	SCHEMA_ADD_FIELD(bool, IsScoped, "C_CSPlayerPawn->m_bIsScoped");
	SCHEMA_ADD_FIELD(bool, IsDefusing, "C_CSPlayerPawn->m_bIsDefusing");
	SCHEMA_ADD_FIELD(bool, IsGrabbingHostage, "C_CSPlayerPawn->m_bIsGrabbingHostage");
	SCHEMA_ADD_FIELD(bool, IsWaitForNoAttack, "C_CSPlayerPawn->m_bWaitForNoAttack");
	SCHEMA_ADD_FIELD(int, GetShotsFired, "C_CSPlayerPawn->m_iShotsFired");
	SCHEMA_ADD_FIELD(std::int32_t, GetArmorValue, "C_CSPlayerPawn->m_ArmorValue");
	SCHEMA_ADD_FIELD(QAngle_t, GetAimPunchAngle, "C_CSPlayerPawn->m_aimPunchAngle");
	SCHEMA_ADD_FIELD(CUtlVector< QAngle_t >, GetAimPunchCache, "C_CSPlayerPawn->m_aimPunchCache");
	//SCHEMA_ADD_FIELD(C_EconItemView, GetEconGloves, "C_CSPlayerPawn->m_EconGloves");
	SCHEMA_ADD_FIELD(C_EconItemView, GetEconGloves, "C_CSPlayerPawn->m_EconGloves");
	SCHEMA_ADD_FIELD(EntitySpottedState_t, IsSpottedState, "C_CSPlayerPawn->m_entitySpottedState");
	SCHEMA_ADD_FIELD(CCSPlayerController_ActionTrackingServices*, GetActionTrackingServices, "C_CSPlayerPawn->m_pActionTrackingServices");
	SCHEMA_ADD_FIELD(CCSPlayer_BulletServices*, GetBulletServices, "C_CSPlayerPawn->m_pBulletServices");
	SCHEMA_ADD_FIELD(bool, NeedToReApplyGloves, "C_CSPlayerPawn->m_bNeedToReApplyGloves");
};

class CBasePlayerController : public C_BaseModelEntity
{
public:
	CS_CLASS_NO_INITIALIZER(CBasePlayerController);

	SCHEMA_ADD_FIELD(std::uint64_t, GetSteamId, "CBasePlayerController->m_steamID");
	SCHEMA_ADD_FIELD(std::uint32_t, GetTickBase, "CBasePlayerController->m_nTickBase");
	SCHEMA_ADD_FIELD(CBaseHandle, GetPawnHandle, "CBasePlayerController->m_hPawn");
	SCHEMA_ADD_FIELD(bool, IsLocalPlayerController, "CBasePlayerController->m_bIsLocalPlayerController");
};



// forward decleration
class C_CSWeaponBaseGun;
class C_BasePlayerWeapon;
class CCSPlayerController : public CBasePlayerController
{
public:
	CS_CLASS_NO_INITIALIZER(CCSPlayerController);

	[[nodiscard]] static CCSPlayerController* GetLocalPlayerController();

	// @note: always get origin from pawn not controller
	[[nodiscard]] const Vector_t& GetPawnOrigin();

	SCHEMA_ADD_FIELD(std::uint32_t, GetPing, "CCSPlayerController->m_iPing");
	SCHEMA_ADD_FIELD(const char*, GetPlayerName, "CCSPlayerController->m_sSanitizedPlayerName");
	SCHEMA_ADD_FIELD(std::int32_t, GetPawnHealth, "CCSPlayerController->m_iPawnHealth");
	SCHEMA_ADD_FIELD(std::int32_t, GetPawnArmor, "CCSPlayerController->m_iPawnArmor");
	SCHEMA_ADD_FIELD(bool, IsPawnHasDefuser, "CCSPlayerController->m_bPawnHasDefuser");
	SCHEMA_ADD_FIELD(bool, IsPawnHasHelmet, "CCSPlayerController->m_bPawnHasHelmet");
	SCHEMA_ADD_FIELD(bool, IsPawnAlive, "CCSPlayerController->m_bPawnIsAlive");
	SCHEMA_ADD_FIELD(CBaseHandle, GetPlayerPawnHandle, "CCSPlayerController->m_hPlayerPawn");
};

class CBaseAnimGraph : public C_BaseModelEntity
{
public:
	CS_CLASS_NO_INITIALIZER(CBaseAnimGraph);

	SCHEMA_ADD_FIELD(bool, IsClientRagdoll, "CBaseAnimGraph->m_bClientRagdoll");
};

class C_BaseFlex : public CBaseAnimGraph
{
public:
	CS_CLASS_NO_INITIALIZER(C_BaseFlex);
	/* not implemented */
};

class CEconItemDefinition {
public:
	bool IsWeapon();
	bool IsKnife(bool excludeDefault);
	bool IsGlove(bool excludeDefault);

	auto GetModelName() {
		return *reinterpret_cast<const char**>((uintptr_t)(this) + 0xD8);
	}

	auto GetStickersSupportedCount() {
		return *reinterpret_cast<int*>((uintptr_t)(this) + 0x100);
	}

	auto GetSimpleWeaponName() {
		return *reinterpret_cast<const char**>((uintptr_t)(this) + 0x210);
	}

	auto GetLoadoutSlot() {
		return *reinterpret_cast<int*>((uintptr_t)(this) + 0x2E8);
	}

	char pad0[0x8];  // vtable
	void* m_pKVItem;
	uint16_t m_nDefIndex;
	CUtlVector<uint16_t> m_nAssociatedItemsDefIndexes;
	bool m_bEnabled;
	const char* m_szPrefab;
	uint8_t m_unMinItemLevel;
	uint8_t m_unMaxItemLevel;
	uint8_t m_nItemRarity;
	uint8_t m_nItemQuality;
	uint8_t m_nForcedItemQuality;
	uint8_t m_nDefaultDropItemQuality;
	uint8_t m_nDefaultDropQuantity;
	CUtlVector<void*> m_vecStaticAttributes;
	uint8_t m_nPopularitySeed;
	void* m_pPortraitsKV;
	const char* m_pszItemBaseName;
	bool m_bProperName;
	const char* m_pszItemTypeName;
	uint32_t m_unItemTypeID;
	const char* m_pszItemDesc;
};

class C_EconItemView
{
public:
	CS_CLASS_NO_INITIALIZER(C_EconItemView);

	int GetCustomPaintKitIndex()
	{ 
		return CALL_VIRTUAL(int, 2, this);
	}
	CEconItemDefinition* GetStaticData()
	{
		return CALL_VIRTUAL(CEconItemDefinition*, 13, this);
	}
	SCHEMA_ADD_FIELD(std::uint16_t, GetItemDefinitionIndex, "C_EconItemView->m_iItemDefinitionIndex");
	SCHEMA_ADD_FIELD(std::uint64_t, GetItemID, "C_EconItemView->m_iItemID");
	SCHEMA_ADD_FIELD(std::uint32_t, GetItemIDHigh, "C_EconItemView->m_iItemIDHigh");
	SCHEMA_ADD_FIELD(std::uint32_t, GetItemIDLow, "C_EconItemView->m_iItemIDLow");
	SCHEMA_ADD_FIELD(std::uint32_t, GetAccountID, "C_EconItemView->m_iAccountID");
	SCHEMA_ADD_FIELD(char[161], GetCustomName, "C_EconItemView->m_szCustomName");
	SCHEMA_ADD_FIELD(char[161], GetCustomNameOverride, "C_EconItemView->m_szCustomNameOverride");
	SCHEMA_ADD_FIELD(bool, GetDisallowSOC, "C_EconItemView->m_bDisallowSOC");
	SCHEMA_ADD_FIELD(bool, GetInitialized, "C_EconItemView->m_bInitialized");
};

class CAttributeManager
{
public:
	CS_CLASS_NO_INITIALIZER(CAttributeManager);
	virtual ~CAttributeManager() = 0;
};
static_assert(sizeof(CAttributeManager) == 0x8);

class C_AttributeContainer : public CAttributeManager
{
public:
	CS_CLASS_NO_INITIALIZER(C_AttributeContainer);

	SCHEMA_ADD_PFIELD(C_EconItemView, GetItem, "C_AttributeContainer->m_Item");
};

class C_EconEntity : public C_BaseFlex
{
public:
	CS_CLASS_NO_INITIALIZER(C_EconEntity);

	SCHEMA_ADD_PFIELD(C_AttributeContainer, GetAttributeManager, "C_EconEntity->m_AttributeManager");
	SCHEMA_ADD_FIELD(std::uint32_t, GetOriginalOwnerXuidLow, "C_EconEntity->m_OriginalOwnerXuidLow");
	SCHEMA_ADD_FIELD(std::uint32_t, GetOriginalOwnerXuidHigh, "C_EconEntity->m_OriginalOwnerXuidHigh");
	SCHEMA_ADD_FIELD(std::int32_t, GetFallbackPaintKit, "C_EconEntity->m_nFallbackPaintKit");
	SCHEMA_ADD_FIELD(std::int32_t, GetFallbackSeed, "C_EconEntity->m_nFallbackSeed");
	SCHEMA_ADD_FIELD(std::int32_t, GetFallbackWear, "C_EconEntity->m_flFallbackWear");
	SCHEMA_ADD_FIELD(std::int32_t, GetFallbackStatTrak, "C_EconEntity->m_nFallbackStatTrak");
	SCHEMA_ADD_FIELD(CBaseHandle, GetViewModelAttachmentHandle, "C_EconEntity->m_hViewmodelAttachment");

	uint64_t GetOriginalOwnerXuid() {
		return ((uint64_t)(GetOriginalOwnerXuidHigh()) << 32) |
			GetOriginalOwnerXuidLow();
	}
};

class C_EconWearable : public C_EconEntity
{
public:
	CS_CLASS_NO_INITIALIZER(C_EconWearable);

	SCHEMA_ADD_FIELD(std::int32_t, GetForceSkin, "C_EconWearable->m_nForceSkin");
	SCHEMA_ADD_FIELD(bool, IsAlwaysAllow, "C_EconWearable->m_bAlwaysAllow");
};

class C_BasePlayerWeapon : public C_EconEntity
{
public:
	CS_CLASS_NO_INITIALIZER(C_BasePlayerWeapon);

	SCHEMA_ADD_FIELD(GameTick_t, GetNextPrimaryAttackTick, "C_BasePlayerWeapon->m_nNextPrimaryAttackTick");
	SCHEMA_ADD_FIELD(float, GetNextPrimaryAttackTickRatio, "C_BasePlayerWeapon->m_flNextPrimaryAttackTickRatio");
	SCHEMA_ADD_FIELD(GameTick_t, GetNextSecondaryAttackTick, "C_BasePlayerWeapon->m_nNextSecondaryAttackTick");
	SCHEMA_ADD_FIELD(float, GetNextSecondaryAttackTickRatio, "C_BasePlayerWeapon->m_flNextSecondaryAttackTickRatio");
	SCHEMA_ADD_FIELD(std::int32_t, GetClip1, "C_BasePlayerWeapon->m_iClip1");
	SCHEMA_ADD_FIELD(std::int32_t, GetClip2, "C_BasePlayerWeapon->m_iClip2");
	SCHEMA_ADD_FIELD(std::int32_t[2], GetReserveAmmo, "C_BasePlayerWeapon->m_pReserveAmmo");
	SCHEMA_ADD_FIELD(float, GetRecoilIndex, "C_CSWeaponBase->m_flRecoilIndex");

	void UpdateAccuracyPenalty()
	{
		using fn_update_accuracy_penality_t = void(__fastcall*)(void*);
		static fn_update_accuracy_penality_t fn = reinterpret_cast<fn_update_accuracy_penality_t>(MEM::FindPattern(CLIENT_DLL, CS_XOR("48 89 5C 24 ? 57 48 83 EC ? 48 8B F9 E8 ? ? ? ? 48 8B D8 48 85 C0 0F 84 ? ? ? ? 44 0F 29 44 24")));

		fn(this);
	}

	float GetMaxSpeed()
	{
		using original_fn = float(__fastcall*)(void*);
		return (*(original_fn**)this)[340](this);
	}
};

class C_CSWeaponBase : public C_BasePlayerWeapon
{
public:
	CS_CLASS_NO_INITIALIZER(C_CSWeaponBase);

	SCHEMA_ADD_FIELD(bool, IsInReload, "C_CSWeaponBase->m_bInReload");
	SCHEMA_ADD_FIELD(float, PostPoneFireTicks, "C_CSWeaponBase->m_nPostponeFireReadyTicks");

	CCSWeaponBaseVData* GetWeaponVData()
	{
		return static_cast<CCSWeaponBaseVData*>(GetVData());
	}

	void* UpdateComposite() { return MEM::CallVFunc<void*, 7u>(this, 1); }
	void* UpdateCompositeSec() { return MEM::CallVFunc<void*, 100u>(this, 1); }

	// #STR: "clientside_reload_custom_econ"
	void UpdateModel(bool should)
	{
		if (!this)
			return;

		auto composite_material = reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(this) + 0x548);
		H::UpateWeaponModel(composite_material, should);
	}

	__forceinline float GetSpread() {
		using original_fn = float(__fastcall*)(void*);
		return (*(original_fn**)this)[364](this); // 352 // 357 // 364
	}
	float GetInaccuracy();
};

class C_CSWeaponBaseGun : public C_CSWeaponBase
{
public:
	CS_CLASS_NO_INITIALIZER(C_CSWeaponBaseGun);

	SCHEMA_ADD_FIELD(std::int32_t, GetZoomLevel, "C_CSWeaponBaseGun->m_zoomLevel");
	SCHEMA_ADD_FIELD(std::int32_t, GetBurstShotsRemaining, "C_CSWeaponBaseGun->m_iBurstShotsRemaining");
	SCHEMA_ADD_FIELD(bool, IsBurstMode, "C_CSWeaponBase->m_bBurstMode");
	SCHEMA_ADD_FIELD(float, GetPostponeFireReadyFrac, "C_CSWeaponBase->m_flPostponeFireReadyFrac");

	[[nodiscard]] bool CanPrimaryAttack(const int nWeaponType, const float flServerTime);
	[[nodiscard]] bool CanSecondaryAttack(const int nWeaponType, const float flServerTime);
};

class C_BaseCSGrenade : public C_CSWeaponBase
{
public:
	SCHEMA_ADD_FIELD(bool, IsHeldByPlayer, "C_BaseCSGrenade->m_bIsHeldByPlayer");
	SCHEMA_ADD_FIELD(bool, IsPinPulled, "C_BaseCSGrenade->m_bPinPulled");
	SCHEMA_ADD_FIELD(GameTime_t, GetThrowTime, "C_BaseCSGrenade->m_fThrowTime");
	SCHEMA_ADD_FIELD(float, GetThrowStrength, "C_BaseCSGrenade->m_flThrowStrength");
};

class C_BaseGrenade : public C_BaseFlex
{
public:
	CS_CLASS_NO_INITIALIZER(C_BaseGrenade);
};

class CSkeletonInstance : public CGameSceneNode
{
public:
	MEM_PAD(0x1CC); //0x0000
	int nBoneCount; //0x01CC
	MEM_PAD(0x18); //0x01D0
	int nMask; //0x01E8
	MEM_PAD(0x4); //0x01EC
	Matrix2x4_t* pBoneCache; //0x01F0
};

class CAnimGraphNetworkedVariables;
class CAnimationGraphInstance2
{
public:
	char pad_0x0000[0x2E0]; //0x0000
	CAnimGraphNetworkedVariables* pAnimGraphNetworkedVariables; //0x02E0
};

class C_BaseViewModel : public C_BaseModelEntity {
public:
	SCHEMA_ADD_FIELD(CBaseHandle, GetWeapon, "C_BaseViewModel->m_hWeapon");
};


class C_CSGOViewModel2 : public C_BaseViewModel
{
public:
	char pad_0x0000[0xD60]; //0x0000
	CAnimationGraphInstance2* pAnimationGraphInstance; //0xD60
};

class CCSPlayer_CameraServices
{
public:
	SCHEMA_ADD_FIELD(int, GetFOV, "CCSPlayerBase_CameraServices->m_iFOV");
	SCHEMA_ADD_FIELD(int, GetFOVStart, "CCSPlayerBase_CameraServices->m_iFOVStart");
	SCHEMA_ADD_FIELD(float, GetFOVTime, "CCSPlayerBase_CameraServices->m_flFOVTime");
	SCHEMA_ADD_FIELD(float, GetFOVRate, "CCSPlayerBase_CameraServices->m_flFOVRate");
	SCHEMA_ADD_FIELD(CBaseHandle, GetZoomOwner, "CCSPlayerBase_CameraServices->m_hZoomOwner");
	SCHEMA_ADD_FIELD(float, GetLastShotFOV, "CCSPlayerBase_CameraServices->m_flLastShotFOV");
	SCHEMA_ADD_FIELD(float, GetDeathCameraTilt, "CCSPlayer_CameraServices->m_flDeathCameraTilt");
	SCHEMA_ADD_FIELD(C_NetworkUtlVectorBase<CBaseHandle>, GetPostProcessingVolumes, "CPlayer_CameraServices->m_PostProcessingVolumes");

};

class CCSPlayerController_ActionTrackingServices
{
public:
	CS_CLASS_NO_INITIALIZER(CCSPlayerController_ActionTrackingServices);

	SCHEMA_ADD_FIELD(std::int32_t, GetNumRoundKills, "CCSPlayerController_ActionTrackingServices->m_iNumRoundKills");
	SCHEMA_ADD_FIELD(std::int32_t, GetNumRoundHeadshotKills, "CCSPlayerController_ActionTrackingServices->m_iNumRoundKillsHeadshots");
	SCHEMA_ADD_FIELD(std::int32_t, GetTotalRoundDamageDealt, "CCSPlayerController_ActionTrackingServices->m_unTotalRoundDamageDealt");
};

class CCSPlayer_BulletServices
{
public:
	CS_CLASS_NO_INITIALIZER(CCSPlayer_BulletServices);

	SCHEMA_ADD_FIELD(int, GetTotalHitsOnServer, "CCSPlayer_BulletServices->m_totalHitsOnServer");
};