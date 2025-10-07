#pragma once

#include <cstdint>
#include <string.h>
#include <unordered_map>
#include <corecrt_malloc.h>
#pragma pack(push, 8)
#define STRINGTOKEN_MURMURHASH_SEED 0x31415926

using c_models_data = std::unordered_map< std::string, std::unordered_map< std::string, std::vector< std::string > > >;

struct knifeData_t {
	std::string name;
	std::string model_path;
	int def_index;
};

struct AgentData_t {
	std::string name;
	std::string model_path;
};

struct GloveData_t
{
	std::string name;
	int def_index;
};

inline std::vector < AgentData_t > agent_data = {
	{ "Ground Rebel  | Elite Crew", "characters/models/tm_leet/tm_leet_variantg.vmdl"},
	{ "Osiris | Elite Crew", "characters/models/tm_leet/tm_leet_varianth.vmdl"},
	{ "Prof. Shahmat | Elite Crew", "characters/models/tm_leet/tm_leet_varianti.vmdl"},
	{ "The Elite Mr. Muhlik | Elite Crew", "characters/models/tm_leet/tm_leet_variantf.vmdl"},
	{ "Jungle Rebel | Elite Crew", "characters/models/tm_leet/tm_leet_variantj.vmdl"},
	{ "Soldier | Phoenix", "characters/models/tm_phoenix/tm_phoenix_varianth.vmdl"},
	{ "Enforcer | Phoenix", "characters/models/tm_phoenix/tm_phoenix_variantf.vmdl"},
	{ "Slingshot | Phoenix", "characters/models/tm_phoenix/tm_phoenix_variantg.vmdl"},
	{ "Street Soldier | Phoenix", "characters/models/tm_phoenix/tm_phoenix_varianti.vmdl"},
	{ "Operator | FBI SWAT", "characters/models/ctm_fbi/ctm_fbi_variantf.vmdl"},
	{ "Markus Delrow | FBI HRT", "characters/models/ctm_fbi/ctm_fbi_variantg.vmdl"},
	{ "Michael Syfers  | FBI Sniper", "characters/models/ctm_fbi/ctm_fbi_varianth.vmdl"},
	{ "Special Agent Ava | FBI", "characters/models/ctm_fbi/ctm_fbi_variantb.vmdl"},
	{ "3rd Commando Company | KSK", "characters/models/ctm_st6/ctm_st6_variantk.vmdl"},
	{ "Seal Team 6 Soldier | NSWC SEAL", "characters/models/ctm_st6/ctm_st6_variante.vmdl"},
	{ "Buckshot | NSWC SEAL", "characters/models/ctm_st6/ctm_st6_variantg.vmdl"},
	{ "'Two Times' McCoy | USAF TACP", "characters/models/ctm_st6/ctm_st6_variantm.vmdl"},
	{ "Lt. Commander Ricksaw | NSWC SEAL", "characters/models/ctm_st6/ctm_st6_varianti.vmdl"},
	{ "'Blueberries' Buckshot | NSWC SEAL", "characters/models/ctm_st6/ctm_st6_variantj.vmdl"},
	{ "'Two Times' McCoy | TACP Cavalry", "characters/models/ctm_st6/ctm_st6_variantl.vmdl"},
	{ "Primeiro Tenente | Brazilian 1st Battalion", "characters/models/ctm_st6/ctm_st6_variantn.vmdl"},
	{ "Dragomir | Sabre", "characters/models/tm_balkan/tm_balkan_variantf.vmdl"},
	{ "Maximus | Sabre", "characters/models/tm_balkan/tm_balkan_varianti.vmdl"},
	{ "Rezan The Ready | Sabre", "characters/models/tm_balkan/tm_balkan_variantg.vmdl"},
	{ "Blackwolf | Sabre", "characters/models/tm_balkan/tm_balkan_variantj.vmdl"},
	{ "'The Doctor' Romanov | Sabre", "characters/models/tm_balkan/tm_balkan_varianth.vmdl"},
	{ "Rezan the Redshirt | Sabre", "characters/models/tm_balkan/tm_balkan_variantk.vmdl"},
	{ "Dragomir | Sabre Footsoldier", "characters/models/tm_balkan/tm_balkan_variantl.vmdl"},
	{ "B Squadron Officer | SAS", "characters/models/ctm_sas/ctm_sas_variantf.vmdl"},
	{ "D Squadron Officer | NZSAS", "characters/models/ctm_sas/ctm_sas_variantg.vmdl"},
	{ "Cmdr. Mae 'Dead Cold' Jamison | SWAT", "characters/models/ctm_swat/ctm_swat_variante.vmdl"},
	{ "1st Lieutenant Farlow | SWAT", "characters/models/ctm_swat/ctm_swat_variantf.vmdl"},
	{ "John 'Van Healen' Kask | SWAT", "characters/models/ctm_swat/ctm_swat_variantg.vmdl"},
	{ "Bio-Haz Specialist | SWAT", "characters/models/ctm_swat/ctm_swat_varianth.vmdl"},
	{ "Sergeant Bombson | SWAT", "characters/models/ctm_swat/ctm_swat_varianti.vmdl"},
	{ "Chem-Haz Specialist | SWAT", "characters/models/ctm_swat/ctm_swat_variantj.vmdl"},
	{ "Lieutenant 'Tree Hugger' Farlow | SWAT", "characters/models/ctm_swat/ctm_swat_variantk.vmdl"},
	{ "Sir Bloody Miami Darryl | The Professionals", "characters/models/tm_professional/tm_professional_varf.vmdl"},
	{ "Sir Bloody Silent Darryl | The Professionals", "characters/models/tm_professional/tm_professional_varf1.vmdl"},
	{ "Sir Bloody Skullhead Darryl | The Professionals", "characters/models/tm_professional/tm_professional_varf2.vmdl"},
	{ "Sir Bloody Darryl Royale | The Professionals", "characters/models/tm_professional/tm_professional_varf3.vmdl"},
	{ "Sir Bloody Loudmouth Darryl | The Professionals", "characters/models/tm_professional/tm_professional_varf4.vmdl"},
	{ "Safecracker Voltzmann | The Professionals", "characters/models/tm_professional/tm_professional_varg.vmdl"},
	{ "Little Kev | The Professionals", "characters/models/tm_professional/tm_professional_varh.vmdl"},
	{ "Number K | The Professionals", "characters/models/tm_professional/tm_professional_vari.vmdl"},
	{ "Getaway Sally | The Professionals", "characters/models/tm_professional/tm_professional_varj.vmdl"},
	{ "Bloody Darryl The Strapped | The Professionals", "characters/models/tm_professional/tm_professional_varf5.vmdl"},
	{ "Sous-Lieutenant Medic | Gendarmerie Nationale", "characters/models/ctm_gendarmerie/ctm_gendarmerie_varianta.vmdl"},
	{ "Chem-Haz Capitaine | Gendarmerie Nationale", "characters/models/ctm_gendarmerie/ctm_gendarmerie_variantb.vmdl"},
	{ "Chef d'Escadron Rouchard | Gendarmerie Nationale", "characters/models/ctm_gendarmerie/ctm_gendarmerie_variantc.vmdl"},
	{ "Aspirant | Gendarmerie Nationale", "characters/models/ctm_gendarmerie/ctm_gendarmerie_variantd.vmdl"},
	{ "Officer Jacques Beltram | Gendarmerie Nationale", "characters/models/ctm_gendarmerie/ctm_gendarmerie_variante.vmdl"},
	{ "Cmdr. Davida 'Goggles' Fernandez | SEAL Frogman", "characters/models/ctm_diver/ctm_diver_varianta.vmdl"},
	{ "Cmdr. Frank 'Wet Sox' Baroud | SEAL Frogman", "characters/models/ctm_diver/ctm_diver_variantb.vmdl"},
	{ "Lieutenant Rex Krikey | SEAL Frogman", "characters/models/ctm_diver/ctm_diver_variantc.vmdl"},
	{ "Elite Trapper Solman | Guerrilla Warfare", "characters/models/tm_jungle_raider/tm_jungle_raider_varianta.vmdl"},
	{ "Crasswater The Forgotten | Guerrilla Warfare", "characters/models/tm_jungle_raider/tm_jungle_raider_variantb.vmdl"},
	{ "Arno The Overgrown | Guerrilla Warfare", "characters/models/tm_jungle_raider/tm_jungle_raider_variantc.vmdl"},
	{ "Col. Mangos Dabisi | Guerrilla Warfare", "characters/models/tm_jungle_raider/tm_jungle_raider_variantd.vmdl"},
	{ "Vypa Sista of the Revolution | Guerrilla Warfare", "characters/models/tm_jungle_raider/tm_jungle_raider_variante.vmdl"},
	{ "Trapper Aggressor | Guerrilla Warfare", "characters/models/tm_jungle_raider/tm_jungle_raider_variantf.vmdl"},
	{ "'Medium Rare' Crasswater | Guerrilla Warfare", "characters/models/tm_jungle_raider/tm_jungle_raider_variantb2.vmdl"},
	{ "Trapper | Guerrilla Warfare", "characters/models/tm_jungle_raider/tm_jungle_raider_variantf2.vmdl"},
};

inline std::vector < knifeData_t > knifes_data = {
		{ "M9 Bayonet", "weapons/models/knife/knife_m9/weapon_knife_m9.vmdl", 508 },
		{ "Classic", "weapons/models/knife/knife_css/weapon_knife_css.vmdl", 503 },
		{ "Bayonet", "weapons/models/knife/knife_bayonet/weapon_knife_bayonet.vmdl", 500 },
		{ "Flip", "weapons/models/knife/knife_flip/weapon_knife_flip.vmdl", 505 },
		{ "Gut", "weapons/models/knife/knife_gut/weapon_knife_gut.vmdl", 506 },
		{ "Karambit", "weapons/models/knife/knife_karambit/weapon_knife_karambit.vmdl", 507 },
		{ "Tactical", "weapons/models/knife/knife_tactical/weapon_knife_tactical.vmdl", 509 },
		{ "Falchion", "weapons/models/knife/knife_falchion/weapon_knife_falchion.vmdl", 512 },
		{ "Bowie", "weapons/models/knife/knife_bowie/weapon_knife_bowie.vmdl", 514 },
		{ "Butterfly", "weapons/models/knife/knife_butterfly/weapon_knife_butterfly.vmdl", 515 },
		{ "Push", "weapons/models/knife/knife_push/weapon_knife_push.vmdl", 516 },
		{ "Cord", "weapons/models/knife/knife_cord/weapon_knife_cord.vmdl", 517 },
		{ "Canis", "weapons/models/knife/knife_canis/weapon_knife_canis.vmdl", 518 },
		{ "Ursus", "weapons/models/knife/knife_ursus/weapon_knife_ursus.vmdl", 519 },
		{ "Navaja", "weapons/models/knife/knife_navaja/weapon_knife_navaja.vmdl", 520 },
		{ "Outdoor", "weapons/models/knife/knife_outdoor/weapon_knife_outdoor.vmdl", 521 },
		{ "Stiletto", "weapons/models/knife/knife_stiletto/weapon_knife_stiletto.vmdl", 522 },
		{ "Talon", "weapons/models/knife/knife_talon/weapon_knife_talon.vmdl", 523 },
		{ "Skeleton", "weapons/models/knife/knife_skeleton/weapon_knife_skeleton.vmdl", 525 },
		{ "Kukri", "weapons/models/knife/knife_kukri/weapon_knife_kukri.vmdl", 526 },
};

inline std::vector <GloveData_t> glove_data = {
	{"Bloodhound Gloves", 5027},
	{"Sport Gloves", 5030},
	{"Driver Gloves", 5031},
	{"Hand Wraps", 5032},
	{"Moto Gloves", 5033},
	{"Specialist Gloves", 5034},
	{"Hydra Gloves", 5035},
	{"Broken Fang Gloves", 4725},
};

inline uint32_t MurmurHash2(const void* key, int len, uint32_t seed)
{
	/* 'm' and 'r' are mixing constants generated offline.
	   They're not really 'magic', they just happen to work well.  */

	const uint32_t m = 0x5bd1e995;
	const int r = 24;

	/* Initialize the hash to a 'random' value */

	uint32_t h = seed ^ len;

	/* Mix 4 bytes at a time into the hash */

	const unsigned char* data = (const unsigned char*)key;

	while (len >= 4)
	{
		uint32_t k = *(uint32_t*)data;

		k *= m;
		k ^= k >> r;
		k *= m;

		h *= m;
		h ^= k;

		data += 4;
		len -= 4;
	}

	/* Handle the last few bytes of the input array  */

	switch (len)
	{
	case 3:
		h ^= data[2] << 16;
	case 2:
		h ^= data[1] << 8;
	case 1:
		h ^= data[0];
		h *= m;
	};

	/* Do a few final mixes of the hash to ensure the last few
	// bytes are well-incorporated.  */

	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;

	return h;
}

#define TOLOWERU(c) ((uint32_t)(((c >= 'A') && (c <= 'Z')) ? c + 32 : c))


inline uint32_t MurmurHash2LowerCaseA1(const char* pString, int len, uint32_t nSeed)
{
	char* p = (char*)malloc(len + 1);
	for (int i = 0; i < len; i++)
	{
		p[i] = TOLOWERU(pString[i]);
	}
	return MurmurHash2(p, len, nSeed);
}

class CUtlStringToken1
{
public:
	std::uint32_t m_nHashCode;
#if DEBUG_STRINGTOKENS
	const char* m_pDebugName;
#endif

	CUtlStringToken1(const char* szString)
	{
		this->SetHashCode1(this->MakeStringToken1(szString));
	}

	bool operator==(const CUtlStringToken1& other) const
	{
		return (other.m_nHashCode == m_nHashCode);
	}

	bool operator!=(const CUtlStringToken1& other) const
	{
		return (other.m_nHashCode != m_nHashCode);
	}

	bool operator<(const CUtlStringToken1& other) const
	{
		return (m_nHashCode < other.m_nHashCode);
	}

	/// access to the hash code for people who need to store thse as 32-bits, regardless of the
	/// setting of DEBUG_STRINGTOKENS (for instance, for atomic operations).
	uint32_t GetHashCode1(void) const
	{
		return m_nHashCode;
	}

	void SetHashCode1(uint32_t nCode)
	{
		m_nHashCode = nCode;
	}

	__forceinline std::uint32_t MakeStringToken1(const char* szString, int nLen)
	{
		std::uint32_t nHashCode = MurmurHash2LowerCaseA1(szString, nLen, STRINGTOKEN_MURMURHASH_SEED);
		return nHashCode;
	}

	__forceinline std::uint32_t MakeStringToken1(const char* szString)
	{
		return MakeStringToken1(szString, (int)strlen(szString));
	}

	//__forceinline std::uint32_t MakeStringToken(CUtlString& str)
	//{
	//    return MakeStringToken(str.Get(), str.Length());
	//}

	CUtlStringToken1()
	{
		m_nHashCode = 0;
	}
};

struct c_paint_kit
{
	std::uint64_t m_id;
	const char* m_name;
	const char* m_description_string;
	const char* m_description_tag;
	std::byte pad_001[0x8];
	const char* m_pattern;
	const char* m_normal;
	const char* m_logo_material;
	std::byte pad_002[0x4];
	std::uint32_t m_rarity;
	std::uint32_t m_style;
	float m_rgba_color[4][4];
	float m_rgba_logo_color[4][4];
	float m_wear_default;
	float m_wear_remap_min;
	float m_wear_remap_max;
	std::uint8_t m_fixed_seed;
	std::uint8_t m_phong_exponent;
	std::uint8_t m_phong_albedo_boost;
	std::uint8_t m_phong_intensity;
	float m_pattern_scale;
	float m_pattern_offset_x_start;
	float m_pattern_offset_x_end;
	float m_pattern_offset_y_start;
	float m_pattern_offset_y_end;
	float m_pattern_rotate_start;
	float m_pattern_rotate_end;
	float m_logo_scale;
	float m_logo_offset_x;
	float m_logo_offset_y;
	float m_logo_rotation;
	bool m_ignore_weapon_size_scale;
	std::byte pad_003[0x3];
	std::uint32_t m_view_model_exponent_override_size;
	bool m_only_first_material;
	bool m_use_normal_model;
	bool m_use_legacy_model;
	std::byte pad_004[0x1];
	float m_pearlescent;
	const char* m_vmt_path;
	std::byte pad_005[0x8];
	const char* m_composite_material_path;
	void* m_vmt_overrides;
	std::byte pad_006[0x8];
};