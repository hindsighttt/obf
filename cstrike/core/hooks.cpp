#include "hooks.h"

// used: variables
#include "variables.h"

// used: game's sdk
#include "../sdk/interfaces/iswapchaindx11.h"
#include "../sdk/interfaces/iviewrender.h"
#include "../sdk/interfaces/cgameentitysystem.h"
#include "../sdk/interfaces/cgametracemanager.h"
#include "../sdk/interfaces/ccsgoinput.h"
#include "../sdk/interfaces/iinputsystem.h"
#include "../sdk/interfaces/iengineclient.h"
#include "../sdk/interfaces/inetworkclientservice.h"
#include "../sdk/interfaces/iglobalvars.h"
#include "../sdk/interfaces/imaterialsystem.h"
#include "../sdk/interfaces/ipvs.h"

// used: viewsetup
#include "../sdk/datatypes/viewsetup.h"

// used: entity
#include "../sdk/entity.h"

// used: get virtual function, find pattern, ...
#include "../utilities/memory.h"
// used: inputsystem
#include "../utilities/inputsystem.h"
// used: draw
#include "../utilities/draw.h"

// used: features callbacks
#include "../features.h"
// used: CRC rebuild
#include "../features/CRC.h"
#include "../antiaim.h"

// used: game's interfaces
#include "interfaces.h"
#include "sdk.h"

// used: menu
#include "menu.h"

bool H::Setup()
{
	if (MH_Initialize() != MH_OK)
	{
		L_PRINT(LOG_ERROR) << CS_XOR("failed to initialize minhook");

		return false;
	}
	L_PRINT(LOG_INFO) << CS_XOR("minhook initialization completed");

	if (!hkPresent.Create(MEM::GetVFunc(I::SwapChain->pDXGISwapChain, VTABLE::D3D::PRESENT), reinterpret_cast<void*>(&Present)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"Present\" hook has been created");

	if (!hkResizeBuffers.Create(MEM::GetVFunc(I::SwapChain->pDXGISwapChain, VTABLE::D3D::RESIZEBUFFERS), reinterpret_cast<void*>(&ResizeBuffers)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"ResizeBuffers\" hook has been created");

	// creat swap chain hook
	IDXGIDevice* pDXGIDevice = NULL;
	I::Device->QueryInterface(IID_PPV_ARGS(&pDXGIDevice));

	IDXGIAdapter* pDXGIAdapter = NULL;
	pDXGIDevice->GetAdapter(&pDXGIAdapter);

	IDXGIFactory* pIDXGIFactory = NULL;
	pDXGIAdapter->GetParent(IID_PPV_ARGS(&pIDXGIFactory));

	if (!hkCreateSwapChain.Create(MEM::GetVFunc(pIDXGIFactory, VTABLE::DXGI::CREATESWAPCHAIN), reinterpret_cast<void*>(&CreateSwapChain)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"CreateSwapChain\" hook has been created");

	pDXGIDevice->Release();
	pDXGIDevice = nullptr;
	pDXGIAdapter->Release();
	pDXGIAdapter = nullptr;
	pIDXGIFactory->Release();
	pIDXGIFactory = nullptr;

	// @ida: class CViewRender->OnRenderStart call GetMatricesForView
	if (!hkGetMatrixForView.Create(MEM::FindPattern(CLIENT_DLL, CS_XOR("40 53 48 81 EC ? ? ? ? 49 8B C1")), reinterpret_cast<void*>(&GetMatrixForView)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"GetMatrixForView\" hook has been created");

	// @ida: #STR: cl: CreateMove clamped invalid attack history index %d in frame history to -1. Was %d, frame history size %d.\n
	// Consider updating I::Input, VTABLE::CLIENT::CREATEMOVE and using that instead.

	// For now, we'll use the pattern
	// Credit: https://www.unknowncheats.me/forum/4265695-post6331.html
	if (!hkCreateMove.Create(MEM::FindPattern(CLIENT_DLL, CS_XOR("48 8B C4 4C 89 40 ? 48 89 48 ? 55 53 57")), reinterpret_cast<void*>(&CreateMove)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"CreateMove\" hook has been created");

	if (!hkMouseInputEnabled.Create(MEM::GetVFunc(I::Input, VTABLE::CLIENT::MOUSEINPUTENABLED), reinterpret_cast<void*>(&MouseInputEnabled)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"MouseInputEnabled\" hook has been created");

	//48 89 5C 24 ? 56 48 83 EC 30 8B 05
	//if (!hkFrameStageNotify.Create(MEM::GetVFunc(I::Client, VTABLE::CLIENT::FRAMESTAGENOTIFY), reinterpret_cast<void*>(&FrameStageNotify)))
	if (!hkFrameStageNotify.Create(MEM::FindPattern(CLIENT_DLL, CS_XOR("48 89 5C 24 ? 56 48 83 EC 30 8B 05")), reinterpret_cast<void*>(&FrameStageNotify)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"FrameStageNotify\" hook has been created");

	// in ida it will go in order as
	// @ida: #STR: ; "game_newmap"
	// @ida: #STR: ; "mapname"
	// @ida: #STR: ; "transition"
	// and the pattern is in the first one "game_newmap"
	if (!hkLevelInit.Create(MEM::FindPattern(CLIENT_DLL, CS_XOR("48 89 5C 24 ? 56 48 83 EC ? 48 8B 0D ? ? ? ? 48 8B F2")), reinterpret_cast<void*>(&LevelInit)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"LevelInit\" hook has been created");

	// @ida: ClientModeShared -> #STR: "map_shutdown"
	if (!hkLevelShutdown.Create(MEM::FindPattern(CLIENT_DLL, CS_XOR("48 83 EC ? 48 8B 0D ? ? ? ? 48 8D 15 ? ? ? ? 45 33 C9 45 33 C0 48 8B 01 FF 50 ? 48 85 C0 74 ? 48 8B 0D ? ? ? ? 48 8B D0 4C 8B 01 41 FF 50 ? 48 83 C4")), reinterpret_cast<void*>(&LevelShutdown)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"LevelShutdown\" hook has been created");

	// @note: seems to do nothing for now...
	// @ida: ClientModeCSNormal->OverrideView idx 15
	//v21 = flSomeWidthSize * 0.5;
	//v22 = *flSomeHeightSize * 0.5;
	//*(float*)(pSetup + 0x49C) = v21; // m_OrthoRight
	//*(float*)(pSetup + 0x494) = -v21; // m_OrthoLeft
	//*(float*)(pSetup + 0x498) = -v22; // m_OrthoTop
	//*(float*)(pSetup + 0x4A0) = v22; // m_OrthoBottom
	if (!hkOverrideView.Create(MEM::FindPattern(CLIENT_DLL, CS_XOR("48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 56 41 57 48 83 EC ? 48 8B FA E8")), reinterpret_cast<void*>(&OverrideView)))
		return false;

	//L_PRINT(LOG_INFO) << CS_XOR("\"OverrideView\" hook has been created");

	// Credit: https://www.unknowncheats.me/forum/4253223-post6185.html
	if (!hkDrawObject.Create(MEM::FindPattern(SCENESYSTEM_DLL, CS_XOR("48 8B C4 48 89 50 ? 53")), reinterpret_cast<void*>(&DrawObject)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"DrawObject\" hook has been created");

	if (!hkIsRelativeMouseMode.Create(MEM::GetVFunc(I::InputSystem, VTABLE::INPUTSYSTEM::ISRELATIVEMOUSEMODE), reinterpret_cast<void*>(&IsRelativeMouseMode)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"IsRelativeMouseMode\" hook has been created");

	if (!hkGetPlayerPawn.Create(MEM::FindPattern(CLIENT_DLL, CS_XOR("40 53 48 83 EC 30 48 8B 41 10 48 8B D9 8B 50 30")), reinterpret_cast<void*>(&GetPlayerPawn)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"GetPlayerPawn\" hook has been created");

	if (!hkUpateWeaponModel.Create(MEM::FindPattern(CLIENT_DLL, CS_XOR("48 89 5C 24 10 48 89 6C 24 18 48 89 74 24 20 57 41 56 41 57 48 83 EC 20 44 0F B6 F2 48 8B F1 E8")), reinterpret_cast<void*>(&UpateWeaponModel)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"UpateWeaponModel\" hook has been created");

	if (!hkSetModel.Create(MEM::FindPattern(CLIENT_DLL, CS_XOR("40 53 48 83 EC 20 48 8B D9 4C 8B C2 48 8B 0D ? ? ? ? 48 8D 54 24")), reinterpret_cast<void*>(&SetModel)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"SetModel\" hook has been created");

	if (!hkSetMeshGroupMask.Create(MEM::GetAbsoluteAddress(MEM::FindPattern(CLIENT_DLL, CS_XOR("E8 ? ? ? ? 48 8B 5C 24 ? 4C 8B 7C 24 ? 4C 8B 74 24 ?")), 0x1, 0x0), reinterpret_cast<void*>(&SetMeshGroupMask)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"SetMeshGroupMask\" hook has been created");

	if (!hkCreateSubTickStepMove.Create(MEM::GetAbsoluteAddress(MEM::FindPattern(CLIENT_DLL, CS_XOR("E8 ?? ?? ?? ?? 48 8B D0 48 8D 4F 18 E8 ?? ?? ?? ?? 48 8B D0")), 0x1, 0x0), reinterpret_cast<void*>(&CreateSubTickStepMove)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"CreateSubTickStepMove\" hook has been created");	
	
	if (!hkOverrideFOV.Create(MEM::FindPattern(CLIENT_DLL, CS_XOR("40 53 48 83 EC ? 48 8B D9 E8 ? ? ? ? 48 85 C0 74 ? 48 8B C8 48 83 C4")), reinterpret_cast<void*>(&OverrideFOV)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"OverrideFOV\" hook has been created");

	if (!hkAntiTamper.Create(MEM::FindPattern(CLIENT_DLL, CS_XOR("48 89 5C 24 08 48 89 74 24 10 48 89 7C 24 18 4C 89 64 24 20 55 41 56 41 57 48 8D 6C 24 B9 48 81 EC F0")), reinterpret_cast<void*>(&AntiTamper)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"AntiTamper\" hook has been created");

	if (!hkScopeOverlay.Create(MEM::FindPattern(CLIENT_DLL, CS_XOR("4C 8B DC 53 56 57 48 83 EC")), reinterpret_cast<void*>(&ScopeOverlay)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"ScopeOverlay\" hook has been created");
	
	// x-ref "Pitch: %6.1f   Yaw: %6.1f   Dist: %6.1f %16s" or "camera offset".
	if (!hkCamera.Create(MEM::FindPattern(CLIENT_DLL, CS_XOR("40 55 53 56 57 41 55 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 63 DA")), reinterpret_cast<void*>(&Camera)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"Camera\" hook has been created");
	
	// STR: "sv_debug_overlays_broadcast"
	// then go into the first sub 
	// or STR: "RegisterConVar: Unknown error registering convar "%s"!" and one up
	if (!hkCVarGetValue.Create(MEM::FindPattern(CLIENT_DLL, CS_XOR("40 53 48 81 EC 80 00 00 00 0F 57 C0 48 C7")), reinterpret_cast<void*>(&CVarGetValue)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"CVarGetValue\" hook has been created");
	
	if (!hkCreateMove2.Create(MEM::FindPattern(CLIENT_DLL, CS_XOR("85 D2 0F 85 ? ? ? ? 48 8B C4 44 88 40")), reinterpret_cast<void*>(&CreateMove2)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"CreateMove2\" hook has been created");
	
	// Wrong one
	
	if (!hkHandleBulletPenetration.Create(MEM::FindPattern(CLIENT_DLL, CS_XOR("48 8B C4 44 89 48 20 48 89 50 10 48")), reinterpret_cast<void*>(&HandleBulletPenetration)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"HandleBulletPenetration\" hook has been created");
	
	/*
	if (!hkGetTraceInfo.Create(MEM::FindPattern(CLIENT_DLL, CS_XOR("48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 48 83 EC 60 48 8B E9 0F")), reinterpret_cast<void*>(&GetTraceInfo)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"GetTraceInfo\" hook has been created");
	*/
	
	/*
	if (!hkInitTraceInfo.Create(MEM::FindPattern(CLIENT_DLL, CS_XOR("48 89 5C 24 ? 57 48 83 EC ? 48 8B D9 33 FF 48 8B 0D ? ? ? ? 48 85 DB")), reinterpret_cast<void*>(&InitTraceInfo)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"InitTraceInfo\" hook has been created");
	*/
	
	
	if (!hkInitTraceFilter.Create(MEM::FindPattern(CLIENT_DLL, CS_XOR("48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 0F B6 41 37 33")), reinterpret_cast<void*>(&InitTraceFilter)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"InitTraceFilter\" hook has been created");
	

	if (!hkCreateTrace.Create(MEM::FindPattern(CLIENT_DLL, CS_XOR("48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 41 56 41 57 48 83 EC 40 F2")), reinterpret_cast<void*>(&CreateTrace)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"CreateTrace\" hook has been created");

	return true;
}

void H::Destroy()
{
	MH_DisableHook(MH_ALL_HOOKS);
	MH_RemoveHook(MH_ALL_HOOKS);

	MH_Uninitialize();
}

HRESULT __stdcall H::Present(IDXGISwapChain* pSwapChain, UINT uSyncInterval, UINT uFlags)
{
	const auto oPresent = hkPresent.GetOriginal();

	// recreate it if it's not valid
	if (I::RenderTargetView == nullptr)
		I::CreateRenderTarget();

	// set our render target
	if (I::RenderTargetView != nullptr)
		I::DeviceContext->OMSetRenderTargets(1, &I::RenderTargetView, nullptr);

	F::OnPresent();

	return oPresent(I::SwapChain->pDXGISwapChain, uSyncInterval, uFlags);
}

HRESULT CS_FASTCALL H::ResizeBuffers(IDXGISwapChain* pSwapChain, std::uint32_t nBufferCount, std::uint32_t nWidth, std::uint32_t nHeight, DXGI_FORMAT newFormat, std::uint32_t nFlags)
{
	const auto oResizeBuffer = hkResizeBuffers.GetOriginal();

	auto hResult = oResizeBuffer(pSwapChain, nBufferCount, nWidth, nHeight, newFormat, nFlags);
	if (SUCCEEDED(hResult))
		I::CreateRenderTarget();

	return hResult;
}

HRESULT __stdcall H::CreateSwapChain(IDXGIFactory* pFactory, IUnknown* pDevice, DXGI_SWAP_CHAIN_DESC* pDesc, IDXGISwapChain** ppSwapChain)
{
	const auto oCreateSwapChain = hkCreateSwapChain.GetOriginal();

	I::DestroyRenderTarget();
	L_PRINT(LOG_INFO) << CS_XOR("render target view has been destroyed");

	return oCreateSwapChain(pFactory, pDevice, pDesc, ppSwapChain);
}

long H::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (D::OnWndProc(hWnd, uMsg, wParam, lParam))
		return 1L;

	return ::CallWindowProcW(IPT::pOldWndProc, hWnd, uMsg, wParam, lParam);
}

ViewMatrix_t* CS_FASTCALL H::GetMatrixForView(CRenderGameSystem* pRenderGameSystem, IViewRender* pViewRender, ViewMatrix_t* pOutWorldToView, ViewMatrix_t* pOutViewToProjection, ViewMatrix_t* pOutWorldToProjection, ViewMatrix_t* pOutWorldToPixels)
{
	const auto oGetMatrixForView = hkGetMatrixForView.GetOriginal();
	ViewMatrix_t* matResult = oGetMatrixForView(pRenderGameSystem, pViewRender, pOutWorldToView, pOutViewToProjection, pOutWorldToProjection, pOutWorldToPixels);

	// get view matrix
	SDK::ViewMatrix = *pOutWorldToProjection;
	// get camera position
	// @note: ida @GetMatrixForView(global_pointer, pRenderGameSystem + 16, ...)
	SDK::CameraPosition = pViewRender->vecOrigin;

	return matResult;
}

bool CS_FASTCALL H::CreateMove(CCSGOInput* pInput, int nSlot, CUserCmd* cmd)
{
	const auto oCreateMove = hkCreateMove.GetOriginal();
	bool bResult = oCreateMove(pInput, nSlot, cmd);

	if (!I::Engine->IsConnected() || !I::Engine->IsInGame())
		return bResult;

	SDK::Cmd = cmd;
	if (SDK::Cmd == nullptr)
		return bResult;
	I::Input = pInput;

	CBaseUserCmdPB* pBaseCmd = SDK::Cmd->csgoUserCmd.pBaseCmd;
	if (pBaseCmd == nullptr)
		return bResult;

	SDK::LocalController = CCSPlayerController::GetLocalPlayerController();
	if (SDK::LocalController == nullptr)
		return bResult;

	SDK::LocalPawn = I::GameResourceService->pGameEntitySystem->Get<C_CSPlayerPawn>(SDK::LocalController->GetPawnHandle());
	if (SDK::LocalPawn == nullptr)
		return bResult;
	F::OnCreateMove(cmd, pBaseCmd, SDK::LocalController);

	// TODO : We need to fix CRC saving
	// 
	// There seems to be an issue within CBasePB and the classes that derive it.
	// So far, you may be unable to press specific keys such as crouch and automatic shooting.
	// A dodgy fix would be to comment it out but it still doesn't fix the bhop etc.

	/*
	CRC::Save(pBaseCmd);
	if (CRC::CalculateCRC(pBaseCmd) == true)
		CRC::Apply(SDK::Cmd);
		*/
	return bResult;
}

bool CS_FASTCALL H::MouseInputEnabled(void* pThisptr)
{
	const auto oMouseInputEnabled = hkMouseInputEnabled.GetOriginal();
	return MENU::bMainWindowOpened ? false : oMouseInputEnabled(pThisptr);
}

void CS_FASTCALL H::FrameStageNotify(void* rcx, int nFrameStage)
{
	const auto oFrameStageNotify = hkFrameStageNotify.GetOriginal();
	F::OnFrameStageNotify(nFrameStage);

	return oFrameStageNotify(rcx, nFrameStage);
}

__int64* CS_FASTCALL H::LevelInit(void* pClientModeShared, const char* szNewMap)
{
	const auto oLevelInit = hkLevelInit.GetOriginal();
	// if global variables are not captured during I::Setup or we join a new game, recapture it
	if ( I::GlobalVars == nullptr )
		I::GlobalVars = *reinterpret_cast< IGlobalVars** >( MEM::ResolveRelativeAddress( MEM::FindPattern( CLIENT_DLL, CS_XOR( "48 89 15 ? ? ? ? 48 89 42" ) ), 0x3, 0x7 ) );
	
	// disable model occlusion
	I::PVS->Set(false);

	return oLevelInit(pClientModeShared, szNewMap);
}

__int64 CS_FASTCALL H::LevelShutdown(void* pClientModeShared)
{
	const auto oLevelShutdown = hkLevelShutdown.GetOriginal();
	// reset global variables since it got discarded by the game
	I::GlobalVars = nullptr;

	return oLevelShutdown(pClientModeShared);
}

void CS_FASTCALL H::OverrideView(void* pClientModeCSNormal, CViewSetup* pSetup)
{
	const auto oOverrideView = hkOverrideView.GetOriginal();
	if (!I::Engine->IsConnected() || !I::Engine->IsInGame())
		return oOverrideView(pClientModeCSNormal, pSetup);
	F::OnOverrideView(pSetup);
	oOverrideView(pClientModeCSNormal, pSetup);

}

void CS_FASTCALL H::DrawObject(void* pAnimatableSceneObjectDesc, void* pDx11, CMeshData* arrMeshDraw, int nDataCount, void* pSceneView, void* pSceneLayer, void* pUnk, void* pUnk2)
{
	const auto oDrawObject = hkDrawObject.GetOriginal();
	if (!I::Engine->IsConnected() || !I::Engine->IsInGame())
		return oDrawObject(pAnimatableSceneObjectDesc, pDx11, arrMeshDraw, nDataCount, pSceneView, pSceneLayer, pUnk, pUnk2);

	if (SDK::LocalController == nullptr || SDK::LocalPawn == nullptr)
		return oDrawObject(pAnimatableSceneObjectDesc, pDx11, arrMeshDraw, nDataCount, pSceneView, pSceneLayer, pUnk, pUnk2);

	if (!F::OnDrawObject(pAnimatableSceneObjectDesc, pDx11, arrMeshDraw, nDataCount, pSceneView, pSceneLayer, pUnk, pUnk2))
		oDrawObject(pAnimatableSceneObjectDesc, pDx11, arrMeshDraw, nDataCount, pSceneView, pSceneLayer, pUnk, pUnk2);
}

void* H::IsRelativeMouseMode(void* pThisptr, bool bActive)
{
	const auto oIsRelativeMouseMode = hkIsRelativeMouseMode.GetOriginal();

	MENU::bMainActive = bActive;

	if (MENU::bMainWindowOpened)
		return oIsRelativeMouseMode(pThisptr, false);

	return oIsRelativeMouseMode(pThisptr, bActive);
}

void CS_FASTCALL H::GetPlayerPawn(void* pUnk)
{
	const auto oGetPlayerPawn = hkGetPlayerPawn.GetOriginal();
	return oGetPlayerPawn(pUnk);
}

void CS_FASTCALL H::UpateWeaponModel(void* pModel, bool bShouldUpdate)
{
	const auto oUpdateWeaponModel = hkUpateWeaponModel.GetOriginal();

	return oUpdateWeaponModel(pModel, bShouldUpdate);
}

void* CS_FASTCALL H::SetModel(void* pModel, const char* sModelPath)
{
	const auto oSetModel = hkSetModel.GetOriginal();

	return oSetModel(pModel, sModelPath);
}

void CS_FASTCALL H::SetMeshGroupMask(void* pSceneNode, uint64_t nMask)
{
	const auto oSetMeshGroupMask = hkSetMeshGroupMask.GetOriginal();

	return oSetMeshGroupMask(pSceneNode, nMask);
}

CSubtickMoveStep* CS_FASTCALL H::CreateSubTickStepMove(void* pArena)
{
	const auto oCreateSubTickStepMove = hkCreateSubTickStepMove.GetOriginal();
	return oCreateSubTickStepMove(pArena);
}

float CS_FASTCALL H::OverrideFOV(CCSPlayer_CameraServices* a1)
{
	const auto oOverrideFOV = hkOverrideFOV.GetOriginal();

	F::OnOverrideFov(a1);

	return oOverrideFOV(a1);
}

void CS_FASTCALL H::ScopeOverlay(void* rcx, void* unk)
{
	static auto oScopeOverlay = hkScopeOverlay.GetOriginal();
	if (I::Engine->IsInGame())
	{
		if (C_GET(bool, Vars.bRemoveScope))
			return;
	}
	return oScopeOverlay(rcx, unk);
}

void CS_FASTCALL H::Camera(CCSGOInput* pCSGOInput, int a2)
{
	const auto oCamera = hkCamera.GetOriginal();
	if (!I::Engine->IsInGame() || !I::Engine->IsConnected())
		return oCamera(pCSGOInput, a2);
	if (SDK::LocalPawn->GetHealth() <= 0)
		return;
	//auto m_backup_input_angle = F::RAGEBOT::ANTIAIM::OriginalAngles;
	//auto m_backup_input_angle = pCSGOInput->angThirdPersonAngles;
	oCamera(pCSGOInput, a2);
	auto NewAngles = QAngle_t(89.0f, 189.0f, 0);
	//pCSGOInput->SetViewAngle(m_backup_input_angle);

	//return oCamera(pCSGOInput, a2);
}

void* CS_FASTCALL H::CVarGetValue(void* pRcx, const char* szCVar, int iDefaultValue)
{
	const auto oCVarGetValue = hkCVarGetValue.GetOriginal();
	return oCVarGetValue(pRcx, szCVar, iDefaultValue);
}

void CS_FASTCALL H::CreateMove2(CCSGOInput* ecx, int edx, CUserCmd* a2)
{
	const auto oCreateMove2 = hkCreateMove2.GetOriginal();
	
	if (!I::Engine->IsConnected() || !I::Engine->IsInGame())
		return oCreateMove2(ecx, edx, a2);
	return oCreateMove2(ecx, edx, a2);
}

bool CS_FASTCALL H::HandleBulletPenetration(trace_data_t* trace, void* stats, update_value_t* update_value, void* pUnk1, void* pUnk2, void* pUnk3, void* pUnk4, void* pUnk5, bool bDrawShowImpacts)
{
	const auto oHandleBulletPenetration = hkHandleBulletPenetration.GetOriginal();

	return oHandleBulletPenetration(trace, stats, update_value, pUnk1, pUnk2, pUnk3, pUnk4, pUnk5, bDrawShowImpacts);
}

void CS_FASTCALL H::GetTraceInfo(trace_data_t* pTraceData, GameTrace_t* pGameTrace, float flUnk, void* Unk1)
{
	const auto oGetTraceInfo = hkGetTraceInfo.GetOriginal();

	return oGetTraceInfo(pTraceData, pGameTrace, flUnk, Unk1);
}

void CS_FASTCALL H::InitTraceInfo(GameTrace_t* hit)
{
	return hkInitTraceInfo.GetOriginal()(hit);
}

void CS_FASTCALL H::CreateTrace(trace_data_t* trace, Vector_t vecStart, Vector_t vecEnd, TraceFilter_t filter, int unk)
{
	return hkCreateTrace.GetOriginal()(trace, vecStart, vecEnd, filter, unk);
}


TraceFilter_t* H::InitTraceFilter(TraceFilter_t& filter, void* pSkip, uint64_t nMask, uint8_t nLayer, uint16_t uUnk1)
{
	return hkInitTraceFilter.GetOriginal()(filter, pSkip, nMask, nLayer, uUnk1);
}

float CS_FASTCALL H::GetWeaponInaccuracy(C_CSWeaponBase* pWeapon, void* unk, void* unk2)
{
	return 0.0f;
}

bool CS_FASTCALL H::AntiTamperReport(int data, void* dataLength, void* retAddress, int unknownVar)
{
	return true;
}

bool CS_FASTCALL H::AntiTamper(int data, void* dataLength, void* retAddress, int unknownVar)
{
	return true;
}
