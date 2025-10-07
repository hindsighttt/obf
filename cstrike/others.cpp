#include "others.h"

#include <DirectXMath.h>

#include "sdk/entity.h"
#include "core/variables.h"
#include "core/interfaces.h"
#include "core/sdk.h"
#include "sdk/interfaces/ccsgoinput.h"
#include "sdk/interfaces/cgametracemanager.h"
#include "sdk\datatypes\viewsetup.h"
#include "sdk/interfaces/iinputsystem.h"
#include "utilities\inputsystem.h"
#include "features/misc/movement.h"
#include "antiaim.h"

bool F::VISUALS::OTHERS::bInThirdperson = false;

Vector_t CalcAngle(const Vector_t& vecSource, const Vector_t& vecDestination)
{
	Vector_t vecDelta = vecDestination - vecSource;

	// Calculate the angle
	float yaw = atan2f(vecDelta.y, vecDelta.x) * 180.0f / DirectX::XM_PI;
	float pitch = -atan2f(vecDelta.z, sqrtf(vecDelta.x * vecDelta.x + vecDelta.y * vecDelta.y)) * 180.0f / DirectX::XM_PI;

	return Vector_t(pitch, yaw, 0.0f);
}

Vector_t CalculateCameraPosition(Vector_t anchorPos, float distance, QAngle_t viewAngles)
{
	float yaw = DirectX::XMConvertToRadians(viewAngles.y);
	float pitch = DirectX::XMConvertToRadians(viewAngles.x);

	float x = anchorPos.x + distance * cosf(yaw) * cosf(pitch);
	float y = anchorPos.y + distance * sinf(yaw) * cosf(pitch);
	float z = anchorPos.z + distance * sinf(pitch);

	return Vector_t{ x, y, z };
}

void Thirdperson(CViewSetup* pSetup)
{
	static bool bShould = true;
	if (C_GET(unsigned int, Vars.nThirdPersonKey) != NULL && IPT::IsKeyReleased(C_GET(unsigned int, Vars.nThirdPersonKey)))
		bShould = !bShould;
	if (!bShould && C_GET(unsigned int, Vars.nThirdPersonKey) != NULL)
	{
		F::VISUALS::OTHERS::bInThirdperson = false;
		return;
	}
		

	QAngle_t angOriginalAngles = I::Input->GetViewAngles();
	Vector_t vecAdjustedAngles = Vector_t(angOriginalAngles.x, angOriginalAngles.y, angOriginalAngles.z);
	QAngle_t angAdjustedAngles = I::Input->GetViewAngles();
	angAdjustedAngles.x = -angAdjustedAngles.x;
	pSetup->vecOrigin = CalculateCameraPosition(SDK::LocalPawn->GetEyePosition(), -C_GET(float, Vars.flThirdPersonDistance), angAdjustedAngles);

	Ray_t ray{};
	GameTrace_t trace{};
	TraceFilter_t filter{ 0x1C3003, SDK::LocalPawn, NULL, 4 };

	if (I::GameTraceManager->TraceShape(&ray, SDK::LocalPawn->GetEyePosition(), pSetup->vecOrigin, &filter, &trace)) {
		if (trace.m_pHitEntity != nullptr) {
			pSetup->vecOrigin = trace.m_vecPosition;
		}
	}

	Vector_t p = CalcAngle(pSetup->vecOrigin, SDK::LocalPawn->GetEyePosition());
	pSetup->angView = QAngle_t{ p.x, p.y }.Normalize();
	F::VISUALS::OTHERS::bInThirdperson = true;
}

void F::VISUALS::OTHERS::OnOverrideView(CViewSetup* pSetup)
{
	if (C_GET(bool, Vars.bThirdPerson))
		Thirdperson(pSetup);
	F::MISC::MOVEMENT::angCorrectionView = F::RAGEBOT::ANTIAIM::OriginalAngles;
}
