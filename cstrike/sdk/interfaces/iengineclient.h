#pragma once

// used: callvfunc
#include "../../utilities/memory.h"
#include "../../core/schema.h"

#include <string>

class CEconItemDefinition;
class CAttributeString
{
public:
	char pad[0x10];
	std::string* data2;
	std::string* data;
};

class CSchemaAttributeType_String
{

};

class CEconItemAttributeDefinition
{
public:
	char pad[0x18];
	CSchemaAttributeType_String* m_pSchemaAttributeType_String;
};
inline constexpr uint64_t Helper_GetAlternateIconKeyForWeaponPaintWearItem(
	uint16_t nDefIdx, uint32_t nPaintId, uint32_t nWear) {
	return (nDefIdx << 16) + (nPaintId << 2) + nWear;
}

struct AlternateIconData_t {
	const char* path_image;
	const char* large_path_image;

private:
	char pad0[0x8];  // no idea
	char pad1[0x8];  // no idea

};
class CPaintKit {
public:

	int id;
	const char* name;
	const char* description_string;
	const char* description_name;
	char pad0[0x8];  // no idea
	char pad1[0x8];  // no idea
	char pad2[0x8];  // no idea
	char pad3[0x8];  // no idea
	char pad4[0x4];  // no idea
	int rarity;

	int64_t PaintKitId() {
		return *reinterpret_cast<int64_t*>((uintptr_t)(this));
	}

	const char* PaintKitName() {
		return *reinterpret_cast<const char**>((uintptr_t)(this) + 0x8);
	}

	const char* PaintKitDescriptionString() {
		return *reinterpret_cast<const char**>((uintptr_t)(this) + 0x10);
	}

	const char* PaintKitDescriptionTag() {
		return *reinterpret_cast<const char**>((uintptr_t)(this) + 0x18);
	}

	int32_t PaintKitRarity() {
		return *reinterpret_cast<int32_t*>((uintptr_t)(this) + 0x44);
	}

	bool UsesOldModel() {
		return *reinterpret_cast<bool*>((uintptr_t)(this) + 0xB2);
	}
};
#include "../datatypes/utlmap.h"


class CSkiterKit {
public:
	int id;
	int rarity;
	const char* name;
	const char* description_name;
	const char* item_name;
	const char* material_path;
	const char* material_path_no_drips;
	const char* inventory_image;
	char pad[0x18];
	float rotate_start;
	float rotate_end;
	float scale_min;
	float scale_max;
	const char* path_image;
	void* unk;
	const char* path_image_large;
	char pad2[0x20];
};
class CEconItemSchema {
public:
	CEconItemAttributeDefinition* get_attribute_definition_by_name(const char* name);

	auto GetAttributeDefinitionInterface(int iAttribIndex) {
		return MEM::CallVFunc<void*, 27U>(this, iAttribIndex);
	}

	auto& GetSortedItemDefinitionMap() {
		return *reinterpret_cast<CUtlMap<int, CEconItemDefinition*>*>(
			(uintptr_t)(this) + 0x128);
	}

	auto& GetAlternateIconsMap() {
		return *reinterpret_cast<CUtlMap<uint64_t, AlternateIconData_t>*>(
			(uintptr_t)(this) + 0x278);
	}

	auto& GetPaintKits() {
		return *reinterpret_cast<CUtlMap<int, CPaintKit*>*>((uintptr_t)(this) +
			0x2F0);
	}
	auto& GetStickers() {
		return *reinterpret_cast<CUtlMap<int, CSkiterKit*>*>((uintptr_t)(this) + 0x318);
	}
};

class CEconItemSystem {
public:
	auto GetEconItemSchema() {
		return *reinterpret_cast<CEconItemSchema**>((uintptr_t)(this) + 0x8);
	}
};

enum EClientFrameStage : int
{
	FRAME_UNDEFINED = -1,
	FRAME_START,
	// a network packet is being received
	FRAME_NET_UPDATE_START,
	// data has been received and we are going to start calling postdataupdate
	FRAME_NET_UPDATE_POSTDATAUPDATE_START,
	// data has been received and called postdataupdate on all data recipients
	FRAME_NET_UPDATE_POSTDATAUPDATE_END,
	// received all packets, we can now do interpolation, prediction, etc
	FRAME_NET_UPDATE_END,
	// start rendering the scene
	FRAME_RENDER_START,
	// finished rendering the scene
	FRAME_RENDER_END,
	FRAME_NET_FULL_FRAME_UPDATE_ON_REMOVE
};

class IEngineClient
{
public:
	int GetMaxClients()
	{
		return MEM::CallVFunc<int, 34U>(this);
	}

	bool IsInGame()
	{
		return MEM::CallVFunc<bool, 35U>(this);
	}

	bool IsConnected()
	{
		return MEM::CallVFunc<bool, 36U>(this);
	}

	// return CBaseHandle index
	int GetLocalPlayer()
	{
		int nIndex = -1;

		MEM::CallVFunc<void, 49U>(this, std::ref(nIndex), 0);

		return nIndex + 1;
	}

	[[nodiscard]] const char* GetLevelName()
	{
		return MEM::CallVFunc<const char*, 56U>(this);
	}

	[[nodiscard]] const char* GetLevelNameShort()
	{
		return MEM::CallVFunc<const char*, 57U>(this);
	}

	[[nodiscard]] const char* GetProductVersionString()
	{
		return MEM::CallVFunc<const char*, 84U>(this);
	}

	
	[[nodiscard]] void ClientCmd_Unrestricted(const char* szCmd) {
		return MEM::CallVFunc<void, 45U>(this, 0, szCmd, 0x7FFEF001);
	}
};
