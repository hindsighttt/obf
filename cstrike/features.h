#pragma once

#include "common.h"

class CUserCmd;
class CBaseUserCmdPB;
class CCSPlayerController;
class CMeshData;
class CCSPlayer_CameraServices;
class CViewSetup;

namespace F
{
	bool Setup();
	void Destroy();

	void OnPresent();
	void OnFrameStageNotify(int nStage);
	void OnCreateMove(CUserCmd* pCmd, CBaseUserCmdPB* pBaseCmd, CCSPlayerController* pLocalController);
	void OnOverrideFov(CCSPlayer_CameraServices* pCamereService);
	bool OnDrawObject(void* pAnimatableSceneObjectDesc, void* pDx11, CMeshData* arrMeshDraw, int nDataCount, void* pSceneView, void* pSceneLayer, void* pUnk, void* pUnk2);
	void OnOverrideView(CViewSetup* pSetup);
}
