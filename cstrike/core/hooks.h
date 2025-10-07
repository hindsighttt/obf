#pragma once

// used: [d3d] api
#include <d3d11.h>
#include <dxgi1_2.h>

// used: chookobject
#include "../utilities/detourhook.h"

// used: viewmatrix_t
#include "../sdk/datatypes/matrix.h"

namespace VTABLE
{
	namespace D3D
	{
		enum
		{
			PRESENT = 8U,
			RESIZEBUFFERS = 13U,
			RESIZEBUFFERS_CSTYLE = 39U,
		};
	}

	namespace DXGI
	{
		enum
		{
			CREATESWAPCHAIN = 10U,
		};
	}

	namespace CLIENT
	{
		enum
		{
			CREATEMOVE = 21U, // Outdated (Using pattern for now)
			MOUSEINPUTENABLED = 19U,
			FRAMESTAGENOTIFY = 21U,
		};
	}

	namespace INPUT
	{
		enum
		{
			VALID_VIEWANGLE = 7U,
		};
	}

	namespace INPUTSYSTEM
	{
		enum
		{
			ISRELATIVEMOUSEMODE = 76U,
		};
	}
}

class CRenderGameSystem;
class IViewRender;
class CCSGOInput;
class CViewSetup;
class CUserCmd;
class CMeshData;
class CSubtickMoveStep;
class CCSPlayer_CameraServices;
struct trace_data_t;
struct update_value_t;
class GameTrace_t;
struct TraceFilter_t;
class C_CSWeaponBase;

namespace H
{
	bool Setup();
	void Destroy();

	/* @section: handlers */
	// d3d11 & wndproc
	HRESULT WINAPI Present(IDXGISwapChain* pSwapChain, UINT uSyncInterval, UINT uFlags);
	HRESULT CS_FASTCALL ResizeBuffers(IDXGISwapChain* pSwapChain, std::uint32_t nBufferCount, std::uint32_t nWidth, std::uint32_t nHeight, DXGI_FORMAT newFormat, std::uint32_t nFlags);
	HRESULT WINAPI CreateSwapChain(IDXGIFactory* pFactory, IUnknown* pDevice, DXGI_SWAP_CHAIN_DESC* pDesc, IDXGISwapChain** ppSwapChain);
	long CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	// game's functions
	ViewMatrix_t* CS_FASTCALL GetMatrixForView(CRenderGameSystem* pRenderGameSystem, IViewRender* pViewRender, ViewMatrix_t* pOutWorldToView, ViewMatrix_t* pOutViewToProjection, ViewMatrix_t* pOutWorldToProjection, ViewMatrix_t* pOutWorldToPixels);
	bool CS_FASTCALL CreateMove(CCSGOInput* pInput, int nSlot, CUserCmd* cmd);
	bool CS_FASTCALL MouseInputEnabled(void* pThisptr);
	void CS_FASTCALL FrameStageNotify(void* rcx, int nFrameStage);
	__int64* CS_FASTCALL LevelInit(void* pClientModeShared, const char* szNewMap);
	__int64 CS_FASTCALL LevelShutdown(void* pClientModeShared);
	void CS_FASTCALL OverrideView(void* pClientModeCSNormal, CViewSetup* pSetup);
	void CS_FASTCALL DrawObject(void* pAnimatableSceneObjectDesc, void* pDx11, CMeshData* arrMeshDraw, int nDataCount, void* pSceneView, void* pSceneLayer, void* pUnk, void* pUnk2);
	void* IsRelativeMouseMode(void* pThisptr, bool bActive);
	void CS_FASTCALL GetPlayerPawn(void* pUnk);
	void CS_FASTCALL UpateWeaponModel(void* pModel, bool bShouldUpdate);
	void* CS_FASTCALL SetModel(void* pModel, const char* sModelPath);
	void CS_FASTCALL SetMeshGroupMask(void* pSceneNode, uint64_t nMask);
	CSubtickMoveStep* CS_FASTCALL CreateSubTickStepMove(void* pArena);
	float CS_FASTCALL OverrideFOV(CCSPlayer_CameraServices* a1);
	void CS_FASTCALL ScopeOverlay(void* rcx, void* unk);
	void CS_FASTCALL Camera(CCSGOInput* pCSGOInput, int a2);
	void* CS_FASTCALL CVarGetValue(void* pRcx, const char* szCVar, int iDefaultValue);
	void CS_FASTCALL CreateMove2(CCSGOInput* ecx, int edx, CUserCmd* a2);

	bool CS_FASTCALL HandleBulletPenetration(trace_data_t* trace, void* stats, update_value_t* update_value, void* pUnk1, void* pUnk2, void* pUnk3, void* pUnk4, void* pUnk5, bool bDrawShowImpacts);
	void CS_FASTCALL GetTraceInfo(trace_data_t* trace, GameTrace_t* game_trace, float flUnk, void* Unk2);
	void CS_FASTCALL InitTraceInfo(GameTrace_t* hit);
	void CS_FASTCALL CreateTrace(trace_data_t* trace, Vector_t vecStart, Vector_t vecEnd, TraceFilter_t filter, int unk);
	TraceFilter_t* InitTraceFilter(TraceFilter_t& filter, void* pSkip, uint64_t nMask, uint8_t nLayer, uint16_t uUnk1);

    float CS_FASTCALL GetWeaponInaccuracy(C_CSWeaponBase* pWeapon, void* unk, void* unk2);

	bool CS_FASTCALL AntiTamperReport(int data, void* dataLength, void* retAddress, int unknownVar);
	bool CS_FASTCALL AntiTamper(int data, void* dataLength, void* retAddress, int unknownVar);

	/* @section: managers */
	inline CBaseHookObject<decltype(&Present)> hkPresent = {};
	inline CBaseHookObject<decltype(&ResizeBuffers)> hkResizeBuffers = {};
	inline CBaseHookObject<decltype(&CreateSwapChain)> hkCreateSwapChain = {};
	inline CBaseHookObject<decltype(&WndProc)> hkWndProc = {};

	inline CBaseHookObject<decltype(&GetMatrixForView)> hkGetMatrixForView = {};
	inline CBaseHookObject<decltype(&CreateMove)> hkCreateMove = {};
	inline CBaseHookObject<decltype(&MouseInputEnabled)> hkMouseInputEnabled = {};
	inline CBaseHookObject<decltype(&IsRelativeMouseMode)> hkIsRelativeMouseMode = {};
	inline CBaseHookObject<decltype(&FrameStageNotify)> hkFrameStageNotify = {};
	inline CBaseHookObject<decltype(&LevelInit)> hkLevelInit = {};
	inline CBaseHookObject<decltype(&LevelShutdown)> hkLevelShutdown = {};
	inline CBaseHookObject<decltype(&OverrideView)> hkOverrideView = {};

	inline CBaseHookObject<decltype(&DrawObject)> hkDrawObject = {};
	inline CBaseHookObject<decltype(&UpateWeaponModel)> hkUpateWeaponModel = {};
	inline CBaseHookObject<decltype(&GetPlayerPawn)>hkGetPlayerPawn = {};
	inline CBaseHookObject<decltype(&SetModel)>hkSetModel = {};
	inline CBaseHookObject<decltype(&SetMeshGroupMask)>hkSetMeshGroupMask = {};
	inline CBaseHookObject<decltype(&CreateSubTickStepMove)>hkCreateSubTickStepMove = {};
	inline CBaseHookObject<decltype(&OverrideFOV)>hkOverrideFOV = {};

	inline CBaseHookObject<decltype(&AntiTamperReport)>hkAntiTamperReport = {};
	inline CBaseHookObject<decltype(&AntiTamper)>hkAntiTamper = {};
	inline CBaseHookObject<decltype(&ScopeOverlay)>hkScopeOverlay = {};
	inline CBaseHookObject<decltype(&Camera)>hkCamera = {};
	inline CBaseHookObject<decltype(&CVarGetValue)>hkCVarGetValue = {};
	inline CBaseHookObject<decltype(&CreateMove2)>hkCreateMove2 = {};
	inline CBaseHookObject<decltype(&HandleBulletPenetration)>hkHandleBulletPenetration = {};
	inline CBaseHookObject<decltype(&GetTraceInfo)>hkGetTraceInfo = {};
	inline CBaseHookObject<decltype(&InitTraceInfo)>hkInitTraceInfo = {};
	inline CBaseHookObject<decltype(&CreateTrace)>hkCreateTrace = {};
	inline CBaseHookObject<decltype(&InitTraceFilter)>hkInitTraceFilter = {};
}
