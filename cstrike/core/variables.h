#pragma once

#include "config.h"

#pragma region variables_combo_entries
using VisualOverlayBox_t = int;

enum EVisualOverlayBox : VisualOverlayBox_t
{
	VISUAL_OVERLAY_BOX_NONE = 0,
	VISUAL_OVERLAY_BOX_FULL,
	VISUAL_OVERLAY_BOX_CORNERS,
	VISUAL_OVERLAY_BOX_MAX
};

using VisualChamMaterial_t = int;
enum EVisualsChamMaterials : VisualChamMaterial_t
{
	VISUAL_MATERIAL_ILLUMINATE = 0,
	VISUAL_MATERIAL_FLAT,
	VISUAL_MATERIAL_DEFAULT,
	VISUAL_MATERIAL_GLOW,
	VISUAL_MATERIAL_GLASS,
	VISUAL_MATERIAL_METAL,
	VISUAL_MATERIAL_MAX
};

using MiscDpiScale_t = int;

enum EMiscDpiScale : MiscDpiScale_t
{
	MISC_DPISCALE_DEFAULT = 0,
	MISC_DPISCALE_125,
	MISC_DPISCALE_150,
	MISC_DPISCALE_175,
	MISC_DPISCALE_200,
	MISC_DPISCALE_MAX
};

#pragma endregion

#pragma region variables_multicombo_entries
using MenuAddition_t = unsigned int;
enum EMenuAddition : MenuAddition_t
{
	MENU_ADDITION_NONE = 0U,
	MENU_ADDITION_DIM_BACKGROUND = 1 << 0,
	MENU_ADDITION_BACKGROUND_PARTICLE = 1 << 1,
	MENU_ADDITION_GLOW = 1 << 2,
	MENU_ADDITION_ALL = MENU_ADDITION_DIM_BACKGROUND | MENU_ADDITION_BACKGROUND_PARTICLE | MENU_ADDITION_GLOW
};

using ChamsCategory_t = unsigned int;
enum EChamsCategory : ChamsCategory_t
{
	CHAMS_ENEMY = 0,
	CHAMS_FRIENDLY,
	CHAMS_LOCAL,
	CHAMS_END
};
#pragma endregion

typedef struct s_skin
{
	int	PaintIndex;
	int item_definition;
	bool Enabled;
}	t_skin;

struct Variables_t
{
#pragma region variables_visuals
	C_ADD_VARIABLE(bool, bVisualOverlay, false);

	C_ADD_VARIABLE(FrameOverlayVar_t, overlayBox, FrameOverlayVar_t(false));
	C_ADD_VARIABLE(TextOverlayVar_t, overlayName, TextOverlayVar_t(false));
	C_ADD_VARIABLE(BarOverlayVar_t, overlayHealthBar, BarOverlayVar_t(false, false, false, 1.f, Color_t(0, 255, 0), Color_t(255, 0, 0)));
	C_ADD_VARIABLE(BarOverlayVar_t, overlayArmorBar, BarOverlayVar_t(false, false, false, 1.f, Color_t(0, 255, 255), Color_t(255, 0, 0)));
	C_ADD_VARIABLE(bool, bEnemyWeaponEsp, false);
	C_ADD_VARIABLE(bool, bHealthText, false);
	C_ADD_VARIABLE(bool, bRadarReveal, false);
	C_ADD_VARIABLE(bool, bHitmarker, false);
	C_ADD_VARIABLE(bool, bHitsound, false);
	C_ADD_VARIABLE(unsigned int, arrESPFlags, 0);

	// Enemy chams
	C_ADD_VARIABLE(bool, bVisualChams, false);
	C_ADD_VARIABLE(int, nVisualChamMaterial, VISUAL_MATERIAL_ILLUMINATE);
	C_ADD_VARIABLE(bool, bVisualChamsIgnoreZ, false); // invisible chams
	C_ADD_VARIABLE(int, nVisualChamMaterialIgnoreZ, VISUAL_MATERIAL_ILLUMINATE);
	C_ADD_VARIABLE(Color_t, colVisualChams, Color_t(0, 255, 0));
	C_ADD_VARIABLE(Color_t, colVisualChamsIgnoreZ, Color_t(255, 0, 0));
	C_ADD_VARIABLE(bool, bRagdollChams, false);
	
	// friendly chams
	C_ADD_VARIABLE(bool, bFriendlyVisualChams, false);
	C_ADD_VARIABLE(int, nFriendlyVisualChamMaterial, VISUAL_MATERIAL_ILLUMINATE);
	C_ADD_VARIABLE(bool, bFriendlyVisualChamsIgnoreZ, false); // invisible chams
	C_ADD_VARIABLE(int, nFriendlyVisualChamMaterialIgnoreZ, VISUAL_MATERIAL_ILLUMINATE);
	C_ADD_VARIABLE(Color_t, colFriendlyVisualChams, Color_t(0, 255, 0));
	C_ADD_VARIABLE(Color_t, colFriendlyVisualChamsIgnoreZ, Color_t(255, 0, 0));
	C_ADD_VARIABLE(bool, bFriendlyRagdollChams, false);

	// Local Chams
	C_ADD_VARIABLE(bool, bLocalWeaponChams, false);
	C_ADD_VARIABLE(int, nLocalWeaponChamsMaterial, 0);
	C_ADD_VARIABLE(Color_t, colLocalWeaponChamsColor, Color_t(255, 255, 255));

	C_ADD_VARIABLE(bool, bLocalHandsChams, false);
	C_ADD_VARIABLE(int, nLocalHandsChamsMaterial, 0);
	C_ADD_VARIABLE(Color_t, colLocalHandsChamsColor, Color_t(255, 255, 255));

	// OTHERS
	C_ADD_VARIABLE(bool, bGrenadeCamera, false);
	C_ADD_VARIABLE(bool, bFovChange, false);
	C_ADD_VARIABLE(float, flFov, 90.0f);
	C_ADD_VARIABLE(bool, bOverrideZoom, true);
	C_ADD_VARIABLE(bool, bRemoveScope, false);
	C_ADD_VARIABLE(bool, bThirdPerson, false);
	C_ADD_VARIABLE(float, flThirdPersonDistance, 100.0f);
	C_ADD_VARIABLE(unsigned int, nThirdPersonKey, NULL);
	C_ADD_VARIABLE(bool, bAutoWallCrosshair, false);
	C_ADD_VARIABLE(bool, bSpreadCircle, false);
	C_ADD_VARIABLE(Color_t, colSpreadCircle, Color_t(0, 0, 0));

#pragma endregion

#pragma region variables_misc
	C_ADD_VARIABLE(bool, bAntiUntrusted, true);
	C_ADD_VARIABLE(bool, bWatermark, true);

	C_ADD_VARIABLE(bool, bAutoBHop, false);
	C_ADD_VARIABLE(int, nAutoBHopChance, 100);

	C_ADD_VARIABLE(bool, bAutoStrafe, false);
#pragma endregion

#pragma region variables_menu
	C_ADD_VARIABLE(unsigned int, nMenuKey, VK_DELETE);
	C_ADD_VARIABLE(unsigned int, nPanicKey, VK_END);
	C_ADD_VARIABLE(int, nDpiScale, 0);
	C_ADD_VARIABLE(int, nCurrentTab, 0);
	C_ADD_VARIABLE(int, nChamsCategory, 0);

	/*
	 * color navigation:
	 * [definition N][purpose]
	 * 1. primitive:
	 * - primtv 0 (text)
	 * - primtv 1 (background)
	 * - primtv 2 (disabled)
	 * - primtv 3 (control bg)
	 * - primtv 4 (border)
	 * - primtv 5 (hover)
	 *
	 * 2. accents:
	 * - accent 0 (main)
	 * - accent 1 (dark)
	 * - accent 2 (darker)
	 */
	C_ADD_VARIABLE(unsigned int, bMenuAdditional, MENU_ADDITION_ALL);
	C_ADD_VARIABLE(float, flAnimationSpeed, 1.f);


	C_ADD_VARIABLE(ColorPickerVar_t, colPrimtv0, ColorPickerVar_t(255, 255, 255)); // (text)
	C_ADD_VARIABLE(ColorPickerVar_t, colPrimtv1, ColorPickerVar_t(50, 55, 70)); // (background)
	C_ADD_VARIABLE(ColorPickerVar_t, colPrimtv2, ColorPickerVar_t(190, 190, 190)); // (disabled)
	C_ADD_VARIABLE(ColorPickerVar_t, colPrimtv3, ColorPickerVar_t(20, 20, 30)); // (control bg)
	C_ADD_VARIABLE(ColorPickerVar_t, colPrimtv4, ColorPickerVar_t(0, 0, 0)); // (border)

	C_ADD_VARIABLE(ColorPickerVar_t, colAccent0, ColorPickerVar_t(85, 90, 160)); // (main)
	C_ADD_VARIABLE(ColorPickerVar_t, colAccent1, ColorPickerVar_t(100, 105, 175)); // (dark)
	C_ADD_VARIABLE(ColorPickerVar_t, colAccent2, ColorPickerVar_t(115, 120, 190)); // (darker)

	C_ADD_VARIABLE(bool, bMenuDarkMode, false);
#pragma endregion

#pragma region variables_ragebot
	C_ADD_VARIABLE(bool, bRageBot, false);
	C_ADD_VARIABLE(bool, bRageSilentAim, false);
	C_ADD_VARIABLE(bool, bAutoPenetration, false);
	C_ADD_VARIABLE(bool, bRageBotAlwaysOn, true);
	C_ADD_VARIABLE(bool, bNoSpread, false);
	C_ADD_VARIABLE(unsigned int, nRageBotActivationKey, NULL);
	C_ADD_VARIABLE(bool, bAutoFire, false);
	C_ADD_VARIABLE(float, flHitchance, 0.0f);
	C_ADD_VARIABLE(float, flMinDmg, 1.0f);
	C_ADD_VARIABLE(float, flMinDmgOverride, 1.0f);
	C_ADD_VARIABLE(unsigned int, nRageBotMinDamageKey, NULL);
	C_ADD_VARIABLE(unsigned int, nRageBotHitboxes, 0);
	C_ADD_VARIABLE(bool, bAutoStop, false);
	C_ADD_VARIABLE(bool, bAutoScope, false);
#pragma endregion


#pragma region variables_legitbot
	C_ADD_VARIABLE(bool, bLegitbot, false);
	C_ADD_VARIABLE(bool, bLegitbotFOV, false);
	C_ADD_VARIABLE(bool, bLegitBotVisCheck, true);
	C_ADD_VARIABLE(float, flAimRange, 5.0f);
	C_ADD_VARIABLE(float, flSmoothing, 2.0f);
	C_ADD_VARIABLE(bool, bLegitbotAlwaysOn, false);
	C_ADD_VARIABLE(bool, bLegitSilent, false);
	C_ADD_VARIABLE(int, nAimMode, 0);
	C_ADD_VARIABLE(unsigned int, nLegitHitbox, 0);
	C_ADD_VARIABLE(unsigned int, nLegitbotActivationKey, VK_LBUTTON);
	C_ADD_VARIABLE(unsigned int, nTriggerbotActivationKey, NULL);
	C_ADD_VARIABLE(bool, bTriggerbot, false);
	C_ADD_VARIABLE(bool, bTriggerbotAlwaysOn, false);
	C_ADD_VARIABLE(bool, bTriggerBotScopedOnly, false);
	C_ADD_VARIABLE(bool, bTriggerWall, false);
#pragma endregion

#pragma region skin_changer
	C_ADD_VARIABLE(bool, bKnifeChanger, false);
	C_ADD_VARIABLE(int, nKnifeIndexModel, 0);
	C_ADD_VARIABLE(bool, bModelChanger, false);
	C_ADD_VARIABLE(bool, bCustomModel, false);
	C_ADD_VARIABLE(int, nCtModelIndex, 0);
	C_ADD_VARIABLE(int, nTModelIndex, 0);
	C_ADD_VARIABLE(int, nGloveIndexModel, 0);
	C_ADD_VARIABLE(bool, bGloveChanger, false);
	C_ADD_VARIABLE(bool, bSkinChanger, false);

	// UGLY STUFF HERE
	C_ADD_VARIABLE(int, WEAPON_DESERT_EAGLE, 0);
	C_ADD_VARIABLE(int, WEAPON_DUAL_BERETTAS, 0);
	C_ADD_VARIABLE(int, WEAPON_FIVE_SEVEN, 0);
	C_ADD_VARIABLE(int, WEAPON_GLOCK_18, 0);
	C_ADD_VARIABLE(int, WEAPON_AK_47, 0);
	C_ADD_VARIABLE(int, WEAPON_AUG, 0);
	C_ADD_VARIABLE(int, WEAPON_AWP, 0);
	C_ADD_VARIABLE(int, WEAPON_FAMAS, 0);
	C_ADD_VARIABLE(int, WEAPON_G3SG1, 0);
	C_ADD_VARIABLE(int, WEAPON_GALIL_AR, 0);
	C_ADD_VARIABLE(int, WEAPON_M249, 0);
	C_ADD_VARIABLE(int, WEAPON_M4A4, 0);
	C_ADD_VARIABLE(int, WEAPON_MAC_10, 0);
	C_ADD_VARIABLE(int, WEAPON_P90, 0);
	C_ADD_VARIABLE(int, WEAPON_MP5_SD, 0);
	C_ADD_VARIABLE(int, WEAPON_UMP_45, 0);
	C_ADD_VARIABLE(int, WEAPON_XM1014, 0);
	C_ADD_VARIABLE(int, WEAPON_PP_BIZON, 0);
	C_ADD_VARIABLE(int, WEAPON_MAG_7, 0);
	C_ADD_VARIABLE(int, WEAPON_NEGEV, 0);
	C_ADD_VARIABLE(int, WEAPON_SAWED_OFF, 0);
	C_ADD_VARIABLE(int, WEAPON_TEC_9, 0);
	C_ADD_VARIABLE(int, WEAPON_ZEUS_X27, 0);
	C_ADD_VARIABLE(int, WEAPON_P2000, 0);
	C_ADD_VARIABLE(int, WEAPON_MP7, 0);
	C_ADD_VARIABLE(int, WEAPON_MP9, 0);
	C_ADD_VARIABLE(int, WEAPON_NOVA, 0);
	C_ADD_VARIABLE(int, WEAPON_P250, 0);
	C_ADD_VARIABLE(int, WEAPON_SCAR_20, 0);
	C_ADD_VARIABLE(int, WEAPON_SG_553, 0);
	C_ADD_VARIABLE(int, WEAPON_SSG_08, 0);
	C_ADD_VARIABLE(int, WEAPON_M4A1_S, 0);
	C_ADD_VARIABLE(int, WEAPON_USP_S, 0);
	C_ADD_VARIABLE(int, WEAPON_CZ75_AUTO, 0);
	C_ADD_VARIABLE(int, WEAPON_R8_REVOLVER, 0);
	C_ADD_VARIABLE(int, WEAPON_BAYONET, 0);
	C_ADD_VARIABLE(int, WEAPON_CLASSIC_KNIFE, 0);
	C_ADD_VARIABLE(int, WEAPON_FLIP_KNIFE, 0);
	C_ADD_VARIABLE(int, WEAPON_GUT_KNIFE, 0);
	C_ADD_VARIABLE(int, WEAPON_KARAMBIT, 0);
	C_ADD_VARIABLE(int, WEAPON_M9_BAYONET, 0);
	C_ADD_VARIABLE(int, WEAPON_HUNTSMAN_KNIFE, 0);
	C_ADD_VARIABLE(int, WEAPON_FALCHION_KNIFE, 0);
	C_ADD_VARIABLE(int, WEAPON_BOWIE_KNIFE, 0);
	C_ADD_VARIABLE(int, WEAPON_BUTTERFLY_KNIFE, 0);
	C_ADD_VARIABLE(int, WEAPON_SHADOW_DAGGERS, 0);
	C_ADD_VARIABLE(int, WEAPON_PARACORD_KNIFE, 0);
	C_ADD_VARIABLE(int, WEAPON_SURVIVAL_KNIFE, 0);
	C_ADD_VARIABLE(int, WEAPON_URSUS_KNIFE, 0);
	C_ADD_VARIABLE(int, WEAPON_NAVAJA_KNIFE, 0);
	C_ADD_VARIABLE(int, WEAPON_NOMAD_KNIFE, 0);
	C_ADD_VARIABLE(int, WEAPON_STILETTO_KNIFE, 0);
	C_ADD_VARIABLE(int, WEAPON_TALON_KNIFE, 0);
	C_ADD_VARIABLE(int, WEAPON_SKELETON_KNIFE, 0);
	C_ADD_VARIABLE(int, WEAPON_KUKRI_KNIFE, 0);
#pragma endregion
};

inline Variables_t Vars = {};
