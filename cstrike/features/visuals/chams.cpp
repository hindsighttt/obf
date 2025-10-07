#include "chams.h"

// used: game's interfaces
#include "../../core/interfaces.h"
#include "../../sdk/interfaces/imaterialsystem.h"
#include "../../sdk/interfaces/igameresourceservice.h"
#include "../../sdk/interfaces/cgameentitysystem.h"
#include "../../sdk/interfaces/iresourcesystem.h"
#include "../../sdk/interfaces/ienginecvar.h"
#include "../../core/sdk.h"
#include "../../sdk/entity.h"
#include "../../utilities/inputsystem.h"
#include "../../others.h"

// used: original call in hooked function
#include "../../core/hooks.h"

// used: cheat variables
#include "../../core/variables.h"

CStrongHandle<CMaterial2> F::VISUALS::CHAMS::CreateMaterial(const char* szMaterialName, const char szVmatBuffer[])
{
	CKeyValues3* pKeyValues3 = CKeyValues3::CreateMaterialResource();
	pKeyValues3->LoadFromBuffer(szVmatBuffer);

	CStrongHandle<CMaterial2> pCustomMaterial = {};
	MEM::fnCreateMaterial(nullptr, &pCustomMaterial, szMaterialName, pKeyValues3, 0, 1);

	return pCustomMaterial;
}

struct CustomMaterial_t
{
	CStrongHandle<CMaterial2> pMaterial;
	CStrongHandle<CMaterial2> pMaterialInvisible;
};

static CustomMaterial_t arrMaterials[VISUAL_MATERIAL_MAX];

bool F::VISUALS::CHAMS::Initialize()
{
	// check if we already initialized materials
	if (bInitialized)
		return bInitialized;

	arrMaterials[VISUAL_MATERIAL_FLAT] = CustomMaterial_t{
		.pMaterial = CreateMaterial(CS_XOR("materials/dev/flat.vmat"), szVMatBufferFlatVisible),
		.pMaterialInvisible = CreateMaterial(CS_XOR("materials/dev/flat_i.vmat"), szVMatBufferFlatInvisible)
	};

	arrMaterials[VISUAL_MATERIAL_ILLUMINATE] = CustomMaterial_t{
		.pMaterial = CreateMaterial(CS_XOR("materials/dev/il.vmat"), szVMatBufferIlluminationVisible),
		.pMaterialInvisible = CreateMaterial(CS_XOR("materials/dev/ili.vmat"), szVMatBufferIlluminationInvisible)
	};

	arrMaterials[VISUAL_MATERIAL_DEFAULT] = CustomMaterial_t{
		.pMaterial = CreateMaterial(CS_XOR("materials/dev/def.vmat"), szVMatBufferDefaultVisible),
		.pMaterialInvisible = CreateMaterial(CS_XOR("materials/dev/defi.vmat"), szVMatBufferDefaultInvisible)
	};

	arrMaterials[VISUAL_MATERIAL_GLOW] = CustomMaterial_t{
		.pMaterial = CreateMaterial(CS_XOR("materials/dev/glow.vmat"), szVMatBufferGlowVisible),
		.pMaterialInvisible = CreateMaterial(CS_XOR("materials/dev/glowi.vmat"), szVMatBufferGlowInvisible)
	};

	arrMaterials[VISUAL_MATERIAL_GLASS] = CustomMaterial_t{
	.pMaterial = CreateMaterial(CS_XOR("materials/dev/glass.vmat"), szVMatBufferGlassVisible),
	.pMaterialInvisible = CreateMaterial(CS_XOR("materials/dev/glassi.vmat"), szVMatBufferGlassInvisible)
	};

	arrMaterials[VISUAL_MATERIAL_METAL] = CustomMaterial_t{
		.pMaterial = CreateMaterial(CS_XOR("materials/dev/metal.vmat"), szVMatBufferMetalVisible),
		.pMaterialInvisible = CreateMaterial(CS_XOR("materials/dev/metali.vmat"), szVMatBufferMetalInvisible)
	};

	bInitialized = true;
	for (auto& [pMaterial, pMaterialInvisible] : arrMaterials)
	{
		if (pMaterial == nullptr || pMaterialInvisible == nullptr)
			bInitialized = false;
	}

	return bInitialized;
}

void F::VISUALS::CHAMS::Destroy()
{
	for (auto& [pVisible, pInvisible] : arrMaterials)
	{
		if (pVisible)
			I::ResourceHandleUtils->DeleteResource(pVisible.pBinding);

		if (pInvisible)
			I::ResourceHandleUtils->DeleteResource(pInvisible.pBinding);
	}
}

bool F::VISUALS::CHAMS::OnDrawObject(void* pAnimatableSceneObjectDesc, void* pDx11, CMeshData* arrMeshDraw, int nDataCount, void* pSceneView, void* pSceneLayer, void* pUnk, void* pUnk2)
{
	if (!bInitialized)
		return false;

	if (!C_GET(bool, Vars.bVisualChams) && !C_GET(bool, Vars.bVisualChamsIgnoreZ) && !C_GET(bool, Vars.bFriendlyVisualChams) && !C_GET(bool, Vars.bFriendlyVisualChamsIgnoreZ) && !C_GET(bool, Vars.bLocalWeaponChams) && !C_GET(bool, Vars.bLocalHandsChams))
		return false;

	if (arrMeshDraw == nullptr)
		return false;

	if (arrMeshDraw->pSceneAnimatableObject == nullptr)
		return false;

	static bool occluded = true;

	if (occluded)
	{
		auto no_vis = I::Cvar->Find(FNV1A::HashConst("sc_no_vis"));
		if (no_vis)
		{
			no_vis->value.i1 = true;
			occluded = false;
		}

	}

	CBaseHandle hOwner = arrMeshDraw->pSceneAnimatableObject->hOwner;

	auto pEntity = I::GameResourceService->pGameEntitySystem->Get<C_BaseEntity>(hOwner);
	if (pEntity == nullptr)
		return false;

	SchemaClassInfoData_t* pClassInfo;
	pEntity->GetSchemaClassInfo(&pClassInfo);
	if (pClassInfo == nullptr)
		return false;

	if (CRT::StringCompare(pClassInfo->szName, CS_XOR("C_ViewmodelAttachmentModel")) == 0)
	{
		return OverrideMaterial(pAnimatableSceneObjectDesc, pDx11, arrMeshDraw, nDataCount, pSceneView, pSceneLayer, pUnk, pUnk2, false, 0, true, true);
	}

	if (CRT::StringCompareN(pClassInfo->szName, CS_XOR("C_Weapon"), 6) == 0 || CRT::StringCompareN(pClassInfo->szName, CS_XOR("C_Knife"), 5) == 0)
	{
		if (!F::VISUALS::OTHERS::bInThirdperson)
			return false;
		return OverrideMaterial(pAnimatableSceneObjectDesc, pDx11, arrMeshDraw, nDataCount, pSceneView, pSceneLayer, pUnk, pUnk2, false, 0, false, true);
	}

	if (CRT::StringCompare(pClassInfo->szName, CS_XOR("C_CSGOViewModel")) == 0)
	{
		return OverrideMaterial(pAnimatableSceneObjectDesc, pDx11, arrMeshDraw, nDataCount, pSceneView, pSceneLayer, pUnk, pUnk2, false, 0, false, true);
	}

	if (CRT::StringCompare(pClassInfo->szName, CS_XOR("C_CSPlayerPawn")) != 0)
		return false;

	
	auto pPlayerPawn = I::GameResourceService->pGameEntitySystem->Get<C_CSPlayerPawn>(hOwner);
	if (pPlayerPawn == nullptr)
		return false;

	bool bLocalPlayer = false;
	if (pPlayerPawn == SDK::LocalPawn)
		bLocalPlayer = true;

	if (!pPlayerPawn->IsOtherEnemy(SDK::LocalPawn) && !C_GET(bool, Vars.bFriendlyVisualChams) && !C_GET(bool, Vars.bFriendlyVisualChamsIgnoreZ))
		return false;

	// alive state
	if (!C_GET(bool, Vars.bRagdollChams) && pPlayerPawn->GetHealth() <= 0 && !C_GET(bool, Vars.bFriendlyRagdollChams))
		return false;
	bool bEnemy = pPlayerPawn->IsOtherEnemy(SDK::LocalPawn);
	return OverrideMaterial(pAnimatableSceneObjectDesc, pDx11, arrMeshDraw, nDataCount, pSceneView, pSceneLayer, pUnk, pUnk2, bEnemy, pPlayerPawn->GetHealth() >= 0, bLocalPlayer, false);
}

bool F::VISUALS::CHAMS::OverrideMaterial(void* pAnimatableSceneObjectDesc, void* pDx11, CMeshData* arrMeshDraw, int nDataCount, void* pSceneView, void* pSceneLayer, void* pUnk, void* pUnk2, bool bEnemy, bool bLifeState, bool bLocalPlayer, bool bWeapon)
{
	const auto oDrawObject = H::hkDrawObject.GetOriginal();

	if (bLocalPlayer && !bWeapon)
	{
		oDrawObject(pAnimatableSceneObjectDesc, pDx11, arrMeshDraw, nDataCount, pSceneView, pSceneLayer, pUnk, pUnk2);
		return true;
	}

	// Enemy chams
	const CustomMaterial_t customMaterialEnemy = arrMaterials[C_GET(int, Vars.nVisualChamMaterial)];
	const CustomMaterial_t customMaterialEnemyIgnoreZ = arrMaterials[C_GET(int, Vars.nVisualChamMaterialIgnoreZ)];

	bool bRendered = false;
	if (C_GET(bool, Vars.bVisualChamsIgnoreZ) && bEnemy && (bLifeState || C_GET(bool, Vars.bRagdollChams))) // Ignore Invisible
	{
		arrMeshDraw->pMaterial = customMaterialEnemyIgnoreZ.pMaterialInvisible;
		arrMeshDraw->colValue = C_GET(Color_t, Vars.colVisualChamsIgnoreZ);
		oDrawObject(pAnimatableSceneObjectDesc, pDx11, arrMeshDraw, nDataCount, pSceneView, pSceneLayer, pUnk, pUnk2);
		bRendered = true;
	}
	if (C_GET(bool, Vars.bVisualChams) && bEnemy && (bLifeState || C_GET(bool, Vars.bRagdollChams)))
	{
		arrMeshDraw->pMaterial = customMaterialEnemy.pMaterial;
		arrMeshDraw->colValue = C_GET(Color_t, Vars.colVisualChams);
		oDrawObject(pAnimatableSceneObjectDesc, pDx11, arrMeshDraw, nDataCount, pSceneView, pSceneLayer, pUnk, pUnk2);
		bRendered = true;
	}

	// Friendly chams
	const CustomMaterial_t customMaterialFriendly = arrMaterials[C_GET(int, Vars.nFriendlyVisualChamMaterial)];
	const CustomMaterial_t customMaterialFriendlyIgnoreZ = arrMaterials[C_GET(int, Vars.nFriendlyVisualChamMaterialIgnoreZ)];
	if (C_GET(bool, Vars.bFriendlyVisualChamsIgnoreZ) && !bEnemy && (bLifeState || C_GET(bool, Vars.bFriendlyRagdollChams))) // Ignore Invisible
	{
		arrMeshDraw->pMaterial = customMaterialFriendlyIgnoreZ.pMaterialInvisible;
		arrMeshDraw->colValue = C_GET(Color_t, Vars.colFriendlyVisualChamsIgnoreZ);
		oDrawObject(pAnimatableSceneObjectDesc, pDx11, arrMeshDraw, nDataCount, pSceneView, pSceneLayer, pUnk, pUnk2);
		bRendered = true;
	}
	if (C_GET(bool, Vars.bFriendlyVisualChams) && !bEnemy && (bLifeState || C_GET(bool, Vars.bFriendlyRagdollChams)))
	{
		arrMeshDraw->pMaterial = customMaterialFriendly.pMaterial;
		arrMeshDraw->colValue = C_GET(Color_t, Vars.colFriendlyVisualChams);
		oDrawObject(pAnimatableSceneObjectDesc, pDx11, arrMeshDraw, nDataCount, pSceneView, pSceneLayer, pUnk, pUnk2);
		bRendered = true;
	}

	// Local Weapon
	const CustomMaterial_t customMaterialLocalWeapon = arrMaterials[C_GET(int, Vars.nLocalWeaponChamsMaterial)];
	if (C_GET(bool, Vars.bLocalWeaponChams) && bWeapon && !bLocalPlayer)
	{
		arrMeshDraw->pMaterial = customMaterialLocalWeapon.pMaterial;
		arrMeshDraw->colValue = C_GET(Color_t, Vars.colLocalWeaponChamsColor);
		oDrawObject(pAnimatableSceneObjectDesc, pDx11, arrMeshDraw, nDataCount, pSceneView, pSceneLayer, pUnk, pUnk2);
		bRendered = true;
	}

	// Local Hands
	const CustomMaterial_t customMaterialLocalHands = arrMaterials[C_GET(int, Vars.nLocalHandsChamsMaterial)];
	if (C_GET(bool, Vars.bLocalHandsChams) && bWeapon && bLocalPlayer)
	{
		arrMeshDraw->pMaterial = customMaterialLocalHands.pMaterial;
		arrMeshDraw->colValue = C_GET(Color_t, Vars.colLocalHandsChamsColor);
		oDrawObject(pAnimatableSceneObjectDesc, pDx11, arrMeshDraw, nDataCount, pSceneView, pSceneLayer, pUnk, pUnk2);
		bRendered = true;
	}

	if (!bRendered)
		oDrawObject(pAnimatableSceneObjectDesc, pDx11, arrMeshDraw, nDataCount, pSceneView, pSceneLayer, pUnk, pUnk2);
	return true;
}
