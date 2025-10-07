#pragma once

class CMeshData;
class CViewSetup;

namespace F::VISUALS
{
	bool Setup();
	void OnDestroy();

	void OnFrame(const int nStage);
	bool OnDrawObject(void* pAnimatableSceneObjectDesc, void* pDx11, CMeshData* arrMeshDraw, int nDataCount, void* pSceneView, void* pSceneLayer, void* pUnk, void* pUnk2);
	void OnOverrideView(CViewSetup* pSetup);
}
