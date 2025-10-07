#pragma once

#include <windows.h>
#include <vector>
#include <string>


typedef struct skin_s
{
	std::string name;
	int			skin_index;
} skin_t;

extern std::vector<skin_t> DeagleSkin;
extern std::vector<skin_t> DuelBerretaSkin;
extern std::vector<skin_t> FiveSevenSkin;
extern std::vector<skin_t> Glock18Skin;
extern std::vector<skin_t> Ak47Skin;
extern std::vector<skin_t> AWPSkin;
extern std::vector<skin_t> FamasSkin;
extern std::vector<skin_t> G3SG1Skin;
extern std::vector<skin_t> GalilARSkin;
extern std::vector<skin_t> M249Skin;
extern std::vector<skin_t> M4A4Skin;
extern std::vector<skin_t> MAC_10Skin;
extern std::vector<skin_t> P90Skin;
extern std::vector<skin_t> MP5_SDSkin;
extern std::vector<skin_t> UMP_45Skin;
extern std::vector<skin_t> XM1014Skin;
extern std::vector<skin_t> PP_BizonSkin;
extern std::vector<skin_t> MAG_7Skin;
extern std::vector<skin_t> NegevSkin;
extern std::vector<skin_t> Sawed_OffSkin;
extern std::vector<skin_t> Tec_9Skin;
extern std::vector<skin_t> ZeusSkin;
extern std::vector<skin_t> P2000Skin;
extern std::vector<skin_t> MP7Skin;
extern std::vector<skin_t> MP9Skin;
extern std::vector<skin_t> NovaSkin;
extern std::vector<skin_t> P250Skin;
extern std::vector<skin_t> SCAR_20Skin;
extern std::vector<skin_t> SG553Skin;
extern std::vector<skin_t> SSG08Skin;
extern std::vector<skin_t> M4A1_SSkin;
extern std::vector<skin_t> USP_SSkin;
extern std::vector<skin_t> CZ75_AutoSkin;
extern std::vector<skin_t> R8RevolverSkin;
extern std::vector<skin_t> BayonetSkin;
extern std::vector<skin_t> ClassicKnifeSkin;
extern std::vector<skin_t> FlipKnifeSkin;
extern std::vector<skin_t> GutKnifeSkin;
extern std::vector<skin_t> KarambitSkin;
extern std::vector<skin_t> M9BayonetSkin;
extern std::vector<skin_t> HuntsmanKnifeSkin;
extern std::vector<skin_t> FalchionKnifeSkin;
extern std::vector<skin_t> BowieKnifeSkin;
extern std::vector<skin_t> ButterflyKnifeSkin;
extern std::vector<skin_t> ShadowDaggersSkin;
extern std::vector<skin_t> ParacordKnifeSkin;
extern std::vector<skin_t> SurvivalKnifeSkin;
extern std::vector<skin_t> UrsusKnifeSkin;
extern std::vector<skin_t> NavajaKnifeSkin;
extern std::vector<skin_t> NomadKnifeSkin;
extern std::vector<skin_t> StilettoKnifeSkin;
extern std::vector<skin_t> TalonKnifeSkin;
extern std::vector<skin_t> SkeletonKnifeSkin;
extern std::vector<skin_t> KukriKnifeSkin;

extern const char* DesertEagleStr;
extern const char* DualBerretaStr;
extern const char* FiveSevenStr;
extern const char* Glock18Str;
extern const char* AK47Str;
extern const char* AUGStr;
extern const char* WeaponAWPStr;
extern const char* WeaponFAMASStr;
extern const char* WeaponG3SG1Str;
extern const char* WeaponGalilARStr;
extern const char* WeaponM249Str;
extern const char* WeaponM4A4Str;
extern const char* WeaponMAC_10Str;
extern const char* WeaponP90Str;
extern const char* WeaponMP5_SDStr;
extern const char* WeaponUMP_45Str;
extern const char* WeaponXM1014Str;
extern const char* WeaponPP_BizonStr;
extern const char* WeaponMAG_7Str;
extern const char* WeaponNegevStr;
extern const char* WeaponSawed_OffStr;
extern const char* WeaponTec_9Str;
extern const char* WeaponZeusx27Str;
extern const char* WeaponP2000Str;
extern const char* WeaponMP7Str;
extern const char* WeaponMP9Str;
extern const char* WeaponNovaStr;
extern const char* WeaponP250Str;
extern const char* WeaponSCAR_20Str;
extern const char* WeaponSG553Str;
extern const char* WeaponSSG08Str;
extern const char* WeaponM4A1_SStr;
extern const char* WeaponUSP_SStr;
extern const char* WeaponCZ75_AutoStr;
extern const char* WeaponR8RevolverStr;
extern const char* WeaponBayonetStr;
extern const char* WeaponClassicKnifeStr;
extern const char* WeaponFlipKnifeStr;
extern const char* WeaponGutKnifeStr;
extern const char* WeaponKarambitStr;
extern const char* WeaponM9BayonetStr;
extern const char* WeaponHuntsmanKnifeStr;
extern const char* WeaponFalchionKnifeStr;
extern const char* WeaponBowieKnifeStr;
extern const char* WeaponButterflyKnifeStr;
extern const char* WeaponShadowDaggersStr;
extern const char* WeaponParacordKnifeStr;
extern const char* WeaponSurvivalKnifeStr;
extern const char* WeaponUrsusKnifeStr;
extern const char* WeaponNavajaKnifeStr;
extern const char* WeaponNomadKnifeStr;
extern const char* WeaponStilettoKnifeStr;
extern const char* WeaponTalonKnifeStr;
extern const char* WeaponSkeletonKnifeStr;
extern const char* WeaponKukriKnifeStr;
extern const char* WeaponBloodhoundGlovesStr;
extern const char* WeaponSportGlovesStr;
extern const char* WeaponDriverGlovesStr;
extern const char* WeaponHandWrapsStr;
extern const char* WeaponMotoGlovesStr;
extern const char* WeaponSpecialistGlovesStr;
extern const char* WeaponHydraGlovesStr;
extern const char* WeaponBrokenFangGlovesStr;


const char* GetSkinStr(int current_selected_skin_index);
int* GetSkinVar(int current_selected_skin_index);