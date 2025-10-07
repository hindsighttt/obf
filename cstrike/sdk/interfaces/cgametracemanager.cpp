// used: game trace manager
#include "cgametracemanager.h"
// used: c_csplayerpawn
#include "../../sdk/entity.h"
#include "../../utilities\memory.h"

SurfaceData_t* GameTrace_t::GetSurfaceData()
{
	using fnGetSurfaceData = std::uint64_t(__fastcall*)(void*);
	static fnGetSurfaceData oGetSurfaceData = reinterpret_cast< fnGetSurfaceData >( MEM::GetAbsoluteAddress( MEM::FindPattern( CLIENT_DLL, CS_XOR( "E8 ? ? ? ? 48 85 C0 74 ? 44 38 68" ) ), 0x1, 0x0 ) );

#ifdef CS_PARANOID
	CS_ASSERT(oGetSurfaceData != nullptr);
#endif

	return reinterpret_cast<SurfaceData_t*>(oGetSurfaceData(m_pSurface));
}

int GameTrace_t::GetHitboxId()
{
	if (m_pHitboxData)
		return m_pHitboxData->m_nHitboxId;
	return 0;
}

int GameTrace_t::GetHitgroup()
{
	if (m_pHitboxData)
		return m_pHitboxData->m_nHitGroup;
	return 0;
}

bool GameTrace_t::IsVisible() const
{
	return (m_flFraction > 0.97f);
}

TraceFilter_t::TraceFilter_t(std::uint64_t uMask, C_CSPlayerPawn* pSkip1, C_CSPlayerPawn* pSkip2, int nLayer)
{
	m_uTraceMask = uMask;
	m_v1[0] = m_v1[1] = 0;
	m_v2 = 7;
	m_v3 = nLayer;
	m_v4 = 0x49;
	m_v5 = 0;

	if (pSkip1 != nullptr)
	{
		m_arrSkipHandles[0] = pSkip1->GetRefEHandle().GetEntryIndex();
		m_arrSkipHandles[2] = pSkip1->GetOwnerHandleIndex();
		m_arrCollisions[0] = pSkip1->GetCollisionMask();
	}

	if (pSkip2 != nullptr)
	{
		m_arrSkipHandles[0] = pSkip2->GetRefEHandle().GetEntryIndex();
		m_arrSkipHandles[2] = pSkip2->GetOwnerHandleIndex();
		m_arrCollisions[0] = pSkip2->GetCollisionMask();
	}
}

bool CGameTraceManager::HandleBulletPenetration(trace_data_t* const trace, void* stats, update_value_t* const mod_value, const bool draw_showimpacts)
{
	auto fn = H::hkHandleBulletPenetration.GetOriginal();
	return fn(trace, stats, mod_value, nullptr, nullptr, nullptr, nullptr, nullptr, draw_showimpacts);
}

void CGameTraceManager::GetTraceInfo(trace_data_t* const trace, GameTrace_t* const hit, const float unknown_float, void* unknown)
{
	static auto pfnGetTraceInfo = reinterpret_cast<decltype(&H::GetTraceInfo)>(
		MEM::FindPattern(CLIENT_DLL, "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 48 83 EC 60 48 8B E9 0F"));

#ifdef CS_PARANOID
	CS_ASSERT(pfnGetTraceInfo != nullptr);
#endif

	return pfnGetTraceInfo(trace, hit, unknown_float, unknown);
}

void CGameTraceManager::InitTraceInfo(GameTrace_t* const hit)
{
	static auto pfnInitTraceInfo = reinterpret_cast<decltype(&H::InitTraceInfo)>(
		MEM::FindPattern(CLIENT_DLL, "48 89 5C 24 ? 57 48 83 EC ? 48 8B D9 33 FF 48 8B 0D ? ? ? ? 48 85 C9"));

#ifdef CS_PARANOID
	CS_ASSERT(pfnInitTraceInfo != nullptr);
#endif

	return pfnInitTraceInfo(hit);
}


void CGameTraceManager::create_trace(trace_data_t* const trace, const Vector_t start, const Vector_t end, const TraceFilter_t& filler, const int penetration_count)
{
	auto fn = H::hkCreateTrace.GetOriginal();
	return fn(trace, start, end, filler, penetration_count);
}

void CGameTraceManager::InitTraceFilter(TraceFilter_t& filter, C_CSPlayerPawn* skip, uint64_t mask, uint8_t layer, uint16_t idk)
{
	//reinterpret_cast<decltype(&H::InitTraceFilter)>(MEM::FindPattern(CLIENT_DLL, "48 89 5C 24 08 57 48 83 EC 20 48 8B D9 33 FF 48 8B 0D"))(filter, skip, mask, layer, idk);
	
	auto fn = H::hkInitTraceFilter.GetOriginal();
	fn(filter, skip, mask, layer, idk);
	
}
