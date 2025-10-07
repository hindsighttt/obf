#include "wall.h"
#include "core/sdk.h"

void WALL::Penetrate(data_t& data, const Vector_t local_pos, const Vector_t target_pos, C_BaseEntity* target, C_BaseEntity* local, C_CSPlayerPawn* localpawn, C_CSPlayerPawn* targetpawn, CCSWeaponBaseVData* wpn_data, float& dmg, bool& valid)
{
    data.m_local = local;
    data.m_target = target;
    data.m_local_pawn = localpawn;
    data.m_target_pawn = targetpawn;
    data.m_wpn_data = wpn_data;
    data.m_pos.at(data_t::e_pos::e_local) = local_pos;
    data.m_pos.at(data_t::e_pos::e_target) = target_pos;
    WALL::FireBullet(data, data.m_dmg, data.m_can_hit);
}

bool WALL::FireBullet(data_t& data, float& dmg, bool& valid)
{
    //debug(data.m_local != nullptr || data.m_target != nullptr || data.m_wpn_data != nullptr || data.m_local_pawn != nullptr || data.m_target_pawn != nullptr);
    if (!data.m_local || !data.m_target || !data.m_wpn_data)
        return false;

    trace_data_t trace_data = { };
    trace_data.m_arr_pointer = &trace_data.m_arr;
    void* data_pointer = &trace_data;

    const Vector_t direction =
        data.m_pos.at(data_t::e_pos::e_target) - data.m_pos.at(data_t::e_pos::e_local),
        end_pos = direction * data.m_wpn_data->GetRange(); // 2ms

    TraceFilter_t filter = {};
    I::GameTraceManager->InitTraceFilter(filter, data.m_local_pawn, PENMASK, 3, 7);
    void* filter_pointer = &filter;
    I::GameTraceManager->create_trace(&trace_data, data.m_pos.at(data_t::e_pos::e_local), end_pos, filter, 4);

    struct handle_bullet_data_t {
        handle_bullet_data_t(const float dmg_mod, const float pen, const float range_mod, const float range,
            const int pen_count, const bool failed) :
            m_dmg(dmg_mod),
            m_pen(pen),
            m_range_mod(range_mod),
            m_range(range),
            m_pen_count(pen_count),
            m_failed(failed) {
        }

        float m_dmg{ }, m_pen{ }, m_range_mod{ }, m_range{ };
        int m_pen_count{ };
        bool m_failed{ };
    }

    handle_bullet_data(static_cast<float>(data.m_wpn_data->GetDamage()), data.m_wpn_data->GetPenetration(), data.m_wpn_data->GetRange(),
        data.m_wpn_data->GetRangeModifier(), 4, false);

    //  L_PRINT(LOG_INFO) << "Initialized handlebulletpen data";

    float corrected_dmg = static_cast<float>(data.m_wpn_data->GetDamage());
    float flTraceLength = 0.f;
    auto flMaxRange = data.m_wpn_data->GetRange();
    if (trace_data.m_num_update > 0) {
        for (int i{ }; i < trace_data.m_num_update; i++) {
            auto* value = reinterpret_cast<update_value_t* const>(
                reinterpret_cast<std::uintptr_t>(trace_data.m_pointer_update_value)
                + i * sizeof(update_value_t));

            GameTrace_t game_trace = { };
            I::GameTraceManager->InitTraceInfo(&game_trace);
            I::GameTraceManager->GetTraceInfo(
                &trace_data, &game_trace, 0.0f,
                reinterpret_cast<void*>(
                    reinterpret_cast<std::uintptr_t>(trace_data.m_arr.data())
                    + sizeof(trace_arr_element_t) * (value->m_handle_idx & 0x7fffu))); // 45ms
            

            flMaxRange -= flTraceLength;

            // we didn't hit anything, stop tracing shoot
            if (game_trace.m_flFraction == 1.0f)
            {
                break;
            }

            // calculate the damage based on the distance the bullet traveled
            flTraceLength += game_trace.m_flFraction * flMaxRange;
            corrected_dmg *= std::powf(data.m_wpn_data->GetRangeModifier(), flTraceLength / 500.f);

            // check is actually can shoot through
            if (flTraceLength > 3000.f)
                break;

            if (game_trace.m_pHitEntity && game_trace.m_pHitEntity->GetRefEHandle().GetEntryIndex() == data.m_target_pawn->GetRefEHandle().GetEntryIndex()) {
                ScaleDamage(game_trace.m_pHitboxData->m_nHitGroup, data.m_target_pawn, data.m_wpn_data->GetArmorRatio(), data.m_wpn_data->GetHeadshotMultiplier(), &corrected_dmg);
                dmg = corrected_dmg;//data.m_dmg > 0.f ? data.m_dmg : handle_bullet_data.m_dmg;
                valid = true;
                return true;
            }
            else
                valid = false;
            if (I::GameTraceManager->HandleBulletPenetration(&trace_data, &handle_bullet_data, value, false))
                return false;
            corrected_dmg = handle_bullet_data.m_dmg;
        }
    }
    return false;
}

void WALL::ScaleDamage(const int nHitGroup, C_CSPlayerPawn* pCSPlayer, const float flWeaponArmorRatio, const float flWeaponHeadShotMultiplier, float* pflDamageToScale)
{

	auto mp_damage_scale_ct_head = I::Cvar->Find(FNV1A::HashConst("mp_damage_scale_ct_head"));
	auto mp_damage_scale_t_head = I::Cvar->Find(FNV1A::HashConst("mp_damage_scale_t_head"));
	auto mp_damage_scale_ct_body = I::Cvar->Find(FNV1A::HashConst("mp_damage_scale_ct_body"));
	auto mp_damage_scale_t_body = I::Cvar->Find(FNV1A::HashConst("mp_damage_scale_t_body"));

	float damage_scale_ct_head = mp_damage_scale_ct_head->value.fl;
	float damage_scale_t_head = mp_damage_scale_t_head->value.fl;
	float damage_scale_ct_body = mp_damage_scale_ct_body->value.fl;
	float damage_scale_t_body = mp_damage_scale_ct_body->value.fl;

	const bool is_ct = pCSPlayer->GetTeam() == 3, is_t = pCSPlayer->GetTeam() == 2;
	float head_damage_scale = is_ct ? damage_scale_ct_head : is_t ? damage_scale_t_head : 1.0f;
	const float body_damage_scale = is_ct ? damage_scale_ct_body : is_t ? damage_scale_t_body : 1.0f;

	/* // Needs implementation
	if (pentity->HasHeavyArmor())
		head_damage_scale *= 0.5f;
	*/
	switch (nHitGroup)
	{
	case HITGROUP_HEAD:
		*pflDamageToScale *= flWeaponHeadShotMultiplier * head_damage_scale;
		break;
	case HITGROUP_CHEST:
	case HITGROUP_LEFTARM:

	case HITGROUP_RIGHTARM:
	case HITGROUP_NECK:
		*pflDamageToScale *= body_damage_scale;
		break;
	case HITGROUP_STOMACH:
		*pflDamageToScale *= 1.25f * body_damage_scale;
		break;
	case HITGROUP_LEFTLEG:
	case HITGROUP_RIGHTLEG:
		*pflDamageToScale *= 0.75f * body_damage_scale;
		break;
	default:
		break;
	}

	/*
	if (pCSPlayer->Has(iHitGroup))
	{
		// @ida CCSPlayer::OnTakeDamage(): server.dll -> "80 BF ? ? ? ? ? F3 0F 10 5C 24 ? F3 0F 10 35"

		const int iArmor = pCSPlayer->GetArmorValue();
		float flHeavyArmorBonus = 1.0f, flArmorBonus = 0.5f, flArmorRatio = flWeaponArmorRatio * 0.5f;

		if (bHeavyArmor)
		{
			flHeavyArmorBonus = 0.25f;
			flArmorBonus = 0.33f;
			flArmorRatio *= 0.20f;
		}

		float flDamageToHealth = *pflDamageToScale * flArmorRatio;
		if (const float flDamageToArmor = (*pflDamageToScale - flDamageToHealth) * (flHeavyArmorBonus * flArmorBonus); flDamageToArmor > static_cast<float>(iArmor))
			flDamageToHealth = *pflDamageToScale - static_cast<float>(iArmor) / flArmorBonus;

		*pflDamageToScale = flDamageToHealth;
	}
	*/
}
