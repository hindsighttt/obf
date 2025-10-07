#include "features.h"

// used: draw callbacks
#include "utilities/draw.h"
// used: notify
#include "utilities/notify.h"

// used: cheat variables
#include "core/variables.h"
// used: menu
#include "core/menu.h"
#include "core/sdk.h"

// used: features callbacks
#include "features/visuals.h"
#include "features/misc.h"
#include "features/legitbot.h"

// used: interfaces
#include "core/interfaces.h"
#include "sdk/interfaces/iengineclient.h"
#include "sdk/interfaces/cgameentitysystem.h"
#include "sdk/datatypes/usercmd.h"
#include "sdk/entity.h"
#include "triggerbot.h"
#include "ragebot.h"
#include "Inventory.h"

bool F::Setup()
{
	if (!VISUALS::Setup())
	{
		L_PRINT(LOG_ERROR) << CS_XOR("failed to setup visuals");
		return false;
	}

	return true;
}

void F::Destroy()
{
	VISUALS::OnDestroy();
}

void F::OnPresent()
{
	if (!D::bInitialized)
		return;

	static int current_style = 0; // 0 = light, 1 = dark

	D::NewFrame();
	{
		
		if (current_style != C_GET(bool, Vars.bMenuDarkMode))
		{
			if (C_GET(bool, Vars.bMenuDarkMode))
				MENU::UpdateStyle2(&ImGui::GetStyle());
			else
				MENU::UpdateStyle(&ImGui::GetStyle());
			current_style = C_GET(bool, Vars.bMenuDarkMode);
		}
		// render watermark
		MENU::RenderWatermark();

		// main window
		ImGui::PushFont(FONT::pMenu[C_GET(int, Vars.nDpiScale)]);
		// @note: here you can draw your stuff
		MENU::RenderMainWindow();
		// render notifications
		NOTIFY::Render();
		ImGui::PopFont();
	}
	D::Render();
}

void F::OnFrameStageNotify(int nStage)
{
	F::VISUALS::OnFrame(nStage);
	INVENTORY::OnFrameStageNotify(nStage);
}

void F::OnCreateMove(CUserCmd* pCmd, CBaseUserCmdPB* pBaseCmd, CCSPlayerController* pLocalController)
{
	C_CSPlayerPawn* pLocalPawn = I::GameResourceService->pGameEntitySystem->Get<C_CSPlayerPawn>(pLocalController->GetPawnHandle());
	if (pLocalPawn == nullptr)
		return;
	if (pCmd->csgoUserCmd.pBaseCmd == NULL)
		return;


	F::TriggerBot::OnMove(pCmd, pBaseCmd, pLocalController, pLocalPawn);
	F::LEGITBOT::OnMove(pCmd, pBaseCmd, pLocalController, pLocalPawn);
	F::RAGEBOT::OnMove(pCmd, pBaseCmd, pLocalController, pLocalPawn);
	F::MISC::OnMove(pCmd, pBaseCmd, pLocalController, pLocalPawn);

}

void F::OnOverrideFov(CCSPlayer_CameraServices* pCamereService)
{
	if (!I::Engine->IsConnected() || ! I::Engine->IsInGame())
		return;
	auto pLocalPawn = SDK::LocalPawn;
	if (!pLocalPawn)
		return;
	auto pWeaponServices = pLocalPawn->GetWeaponServices();
	if (!pWeaponServices)
		return;
	CBaseHandle hWeapon = pLocalPawn->GetWeaponServices()->GetActiveWeapon();
	auto pWeapon = I::GameResourceService->pGameEntitySystem->Get<C_CSWeaponBase>(hWeapon);
	if (!pWeapon)
		return;
	CCSWeaponBaseVData* pWeaponVData = reinterpret_cast<CCSWeaponBaseVData*>(pWeapon->GetVData());
	if (!pWeaponVData)
		return;
	if (!C_GET(bool, Vars.bFovChange))
		return;
	float flNewFov = C_GET(float, Vars.flFov);
	if (flNewFov < 0)
		return;
	
	// check for zoom
	if (!C_GET(bool, Vars.bOverrideZoom))
	{
		if (pWeaponVData->GetWeaponType() == WEAPONTYPE_SNIPER_RIFLE)
		{
			auto pActiveWeaponBaseGun = I::GameResourceService->pGameEntitySystem->Get<C_CSWeaponBaseGun>(pWeapon->GetRefEHandle());
			if (!pActiveWeaponBaseGun)
				return;
			if (pActiveWeaponBaseGun->GetZoomLevel() != 0)
				return;
		}
	}

	pCamereService->GetFOV() = flNewFov;
}

bool F::OnDrawObject(void* pAnimatableSceneObjectDesc, void* pDx11, CMeshData* arrMeshDraw, int nDataCount, void* pSceneView, void* pSceneLayer, void* pUnk, void* pUnk2)
{
	return VISUALS::OnDrawObject(pAnimatableSceneObjectDesc, pDx11, arrMeshDraw, nDataCount, pSceneView, pSceneLayer, pUnk, pUnk2);
}

void F::OnOverrideView(CViewSetup* pSetup)
{
	F::VISUALS::OnOverrideView(pSetup);
}
