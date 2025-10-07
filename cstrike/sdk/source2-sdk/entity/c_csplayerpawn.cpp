#include "c_csplayerpawn.hpp"

#include "../../interfaces/interfaces.hpp"
#include "../../memory/memory.hpp"

bool C_CSPlayerPawn::IsEnemyWithTeam(int team) {
    static ConVar* mp_teammates_are_enemies =
        interfaces::pCvar->FindVarByName("mp_teammates_are_enemies");

    CCSPlayerController* pPlayerController =
        m_hController().Get<CCSPlayerController>();

    if (pPlayerController && pPlayerController->m_bIsLocalPlayerController())
        return false;

    return mp_teammates_are_enemies->GetValue<bool>() ? true
                                                      : m_iTeamNum() != team;
}

bool C_CSPlayerPawn::IsVisible(C_CSPlayerPawn* pPawn, Vector vecPoint, bool bSmokeCheck, float flMaxDensity)
{
	if (!this || !pPawn)
		return false;

	TraceFilter_t filter(0x1C3003, this, nullptr, 4);
	Ray_t ray = Ray_t();
	GameTrace_t trace = GameTrace_t();

	Vector vecSource = this->GetEyePosition();
	interfaces::pTraceMgr->TraceShape(&ray, vecSource, vecPoint, &filter, &trace);

	if ((trace.IsVisible() || (trace.m_pHitEntity && trace.m_pHitEntity->GetRefEHandle().GetEntryIndex() == pPawn->GetRefEHandle().GetEntryIndex())))// && !(bSmokeCheck && !Utilities::LineGoesThroughSmoke(vecSource, vecPoint, flMaxDensity)))
		return true;

	return false;
}

C_CSWeaponBase* C_CSPlayerPawn::GetWeaponActive()
{
	auto service = m_pWeaponServices();

	if (!service)
		return nullptr;

	auto handle = service->m_hActiveWeapon();
	auto weapon = reinterpret_cast<C_CSWeaponBase*>(interfaces::pGameResourceService->GetGameEntitySystem()->GetByIndex<C_CSWeaponBase>(handle));

	if (!weapon)
		return nullptr;

	return weapon;
}
