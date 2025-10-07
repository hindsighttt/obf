#pragma once

#include "sdk/interfaces/cgametracemanager.h"
#include "sdk/interfaces/ienginecvar.h"
#include "sdk/entity.h"

inline struct data_t {
    enum e_pos { e_local, e_target };
    data_t() = default;

    inline data_t(const Vector_t local_pos, const Vector_t target_pos,
        C_BaseEntity* target,
        C_BaseEntity* local,
        C_CSPlayerPawn* localpawn, C_CSPlayerPawn* targetpawn,
        CCSWeaponBaseVData* wpn_data, const bool fire_bullet) noexcept :
        m_pos{ local_pos, target_pos },
        m_target(target),
        m_local(local),
        m_local_pawn(localpawn),
        m_target_pawn(targetpawn),
        m_wpn_data(wpn_data),
        m_can_hit(fire_bullet) {
    }


    std::array< Vector_t, static_cast<int>(sizeof(e_pos)) > m_pos{ };
    C_BaseEntity* m_target{ }, * m_local{ };
    C_CSPlayerPawn* m_local_pawn{};
    C_CSPlayerPawn* m_target_pawn{};
    CCSWeaponBaseVData* m_wpn_data{ };

    bool m_can_hit{ };
    float m_dmg{ };
} m_data;

static constexpr std::uint32_t PENMASK = 0x1C300Bu; // mask_shot_hull | contents_hitbox?

namespace WALL
{
    void Penetrate(data_t& data, const Vector_t local_pos, const Vector_t target_pos, C_BaseEntity* target, C_BaseEntity* local, C_CSPlayerPawn* localpawn, C_CSPlayerPawn* targetpawn, CCSWeaponBaseVData* wpn_data, float& dmg, bool& valid);
    bool FireBullet(data_t& data, float& dmg, bool& valid);
	void ScaleDamage(const int nHitGroup, C_CSPlayerPawn* pCSPlayer, const float flWeaponArmorRatio, const float flWeaponHeadShotMultiplier, float* pflDamageToScale);
}