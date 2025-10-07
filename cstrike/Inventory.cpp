#include "Inventory.h"
#include "core/interfaces.h"
#include "core/sdk.h"
#include "sdk/entity.h"
#include "sdk/interfaces/cgameentitysystem.h"
#include "sdk/interfaces/iengineclient.h"
#include "sdk/interfaces/inetworkclientservice.h"
#include "sdk/interfaces/iressourcemanager.h"
#include "knifechanger_utils.h"
#include "core/variables.h"
#include "skins.h"

#include <iostream>
#include <Windows.h>
#include <cstdint>
#include <cstdio>
#include <intrin.h>
#include <xmmintrin.h>
#include <array>
#include <vector>
#include <algorithm>
#include <cctype>
#include <string>
#include <chrono>
#include <thread>
#include <memory>
#include <sstream>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <deque>
#include <functional>
#include <map>
#include <shlobj.h>
#include <filesystem>
#include <streambuf>
#include <string>
#include <algorithm>
#include <mutex>
#include <filesystem>
#include <intrin.h>
#include <strsafe.h>
#include <Psapi.h>
#include <thread>
#include <dxgi.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <xmmintrin.h>

auto custom_models_data = c_models_data{};
int INVENTORY::customModelIndex = 0;
int INVENTORY::MaxcustomModelIndex = 1;
int INVENTORY::current_weapon_index = 0;

static std::string base_path = []() {
	char buffer[1024];
	DWORD len = GetModuleFileNameA(nullptr, buffer, sizeof(buffer));
	if (len == 0 || len == sizeof(buffer))
		return std::string("");
	std::string exe_path(buffer, len);
	std::replace(exe_path.begin(), exe_path.end(), '\\', '/');
	// Go four directories back from the executable path
	for (int i = 0; i < 4; ++i) {
		auto last_slash = exe_path.find_last_of("/");
		if (last_slash == std::string::npos)
			return std::string("");
		exe_path = exe_path.substr(0, last_slash);
	}
	// Append the relative path to the models directory
	return exe_path + CS_XOR("/game/csgo/characters/models");
}();

int GetPaintIndex(int nWeaponDefinitionIndex)
{
	switch (nWeaponDefinitionIndex)
	{
	case WEAPON_DESERT_EAGLE:
		return (DeagleSkin[C_GET(int, Vars.WEAPON_DESERT_EAGLE)].skin_index);
	case WEAPON_DUAL_BERETTAS:
		return (DuelBerretaSkin[C_GET(int, Vars.WEAPON_DUAL_BERETTAS)].skin_index);
	case WEAPON_FIVE_SEVEN:
		return (FiveSevenSkin[C_GET(int, Vars.WEAPON_FIVE_SEVEN)].skin_index);
	case WEAPON_GLOCK_18:
		return (Glock18Skin[C_GET(int, Vars.WEAPON_GLOCK_18)].skin_index);
	case WEAPON_AK_47:
		return (Ak47Skin[C_GET(int, Vars.WEAPON_AK_47)].skin_index);
	case WEAPON_AUG:
		return (0); // fucked up
	case WEAPON_AWP:
		return (AWPSkin[C_GET(int, Vars.WEAPON_AWP)].skin_index);
	case WEAPON_FAMAS:
		return (FamasSkin[C_GET(int, Vars.WEAPON_FAMAS)].skin_index);
	case WEAPON_G3SG1:
		return (G3SG1Skin[C_GET(int, Vars.WEAPON_G3SG1)].skin_index);
	case WEAPON_GALIL_AR:
		return (GalilARSkin[C_GET(int, Vars.WEAPON_GALIL_AR)].skin_index);
	case WEAPON_M249:
		return (M249Skin[C_GET(int, Vars.WEAPON_M249)].skin_index);
	case WEAPON_M4A4:
		return (M4A4Skin[C_GET(int, Vars.WEAPON_M4A4)].skin_index);
	case WEAPON_MAC_10:
		return (MAC_10Skin[C_GET(int, Vars.WEAPON_MAC_10)].skin_index);
	case WEAPON_P90:
		return (P90Skin[C_GET(int, Vars.WEAPON_P90)].skin_index);
	case WEAPON_MP5_SD:
		return (MP5_SDSkin[C_GET(int, Vars.WEAPON_MP5_SD)].skin_index);
	case WEAPON_UMP_45:
		return (UMP_45Skin[C_GET(int, Vars.WEAPON_UMP_45)].skin_index);
	case WEAPON_XM1014:
		return (XM1014Skin[C_GET(int, Vars.WEAPON_XM1014)].skin_index);
	case WEAPON_PP_BIZON:
		return (PP_BizonSkin[C_GET(int, Vars.WEAPON_PP_BIZON)].skin_index);
	case WEAPON_MAG_7:
		return (MAG_7Skin[C_GET(int, Vars.WEAPON_MAG_7)].skin_index);
	case WEAPON_NEGEV:
		return (NegevSkin[C_GET(int, Vars.WEAPON_NEGEV)].skin_index);
	case WEAPON_SAWED_OFF:
		return (Sawed_OffSkin[C_GET(int, Vars.WEAPON_SAWED_OFF)].skin_index);
	case WEAPON_TEC_9:
		return (Tec_9Skin[C_GET(int, Vars.WEAPON_TEC_9)].skin_index);
	case WEAPON_ZEUS_X27:
		return (ZeusSkin[C_GET(int, Vars.WEAPON_ZEUS_X27)].skin_index);
	case WEAPON_P2000:
		return (P2000Skin[C_GET(int, Vars.WEAPON_P2000)].skin_index);
	case WEAPON_MP7:
		return (MP7Skin[C_GET(int, Vars.WEAPON_MP7)].skin_index);
	case WEAPON_MP9:
		return (MP9Skin[C_GET(int, Vars.WEAPON_MP9)].skin_index);
	case WEAPON_NOVA:
		return (NovaSkin[C_GET(int, Vars.WEAPON_NOVA)].skin_index);
	case WEAPON_P250:
		return (P250Skin[C_GET(int, Vars.WEAPON_P250)].skin_index);
	case WEAPON_SCAR_20:
		return (SCAR_20Skin[C_GET(int, Vars.WEAPON_SCAR_20)].skin_index);
	case WEAPON_SG_553:
		return (SG553Skin[C_GET(int, Vars.WEAPON_SG_553)].skin_index);
	case WEAPON_SSG_08:
		return (SSG08Skin[C_GET(int, Vars.WEAPON_SSG_08)].skin_index);
	case WEAPON_M4A1_S:
		return (M4A1_SSkin[C_GET(int, Vars.WEAPON_M4A1_S)].skin_index);
	case WEAPON_USP_S:
		return (USP_SSkin[C_GET(int, Vars.WEAPON_USP_S)].skin_index);
	case WEAPON_CZ75_AUTO:
		return (CZ75_AutoSkin[C_GET(int, Vars.WEAPON_CZ75_AUTO)].skin_index);
	case WEAPON_R8_REVOLVER:
		return (R8RevolverSkin[C_GET(int, Vars.WEAPON_R8_REVOLVER)].skin_index);
	case WEAPON_BAYONET:
		return (BayonetSkin[C_GET(int, Vars.WEAPON_BAYONET)].skin_index);
	case WEAPON_CLASSIC_KNIFE:
		return (ClassicKnifeSkin[C_GET(int, Vars.WEAPON_CLASSIC_KNIFE)].skin_index);
	case WEAPON_FLIP_KNIFE:
		return (FlipKnifeSkin[C_GET(int, Vars.WEAPON_FLIP_KNIFE)].skin_index);
	case WEAPON_GUT_KNIFE:
		return (GutKnifeSkin[C_GET(int, Vars.WEAPON_GUT_KNIFE)].skin_index);
	case WEAPON_KARAMBIT:
		return (KarambitSkin[C_GET(int, Vars.WEAPON_KARAMBIT)].skin_index);
	case WEAPON_M9_BAYONET:
		return (M9BayonetSkin[C_GET(int, Vars.WEAPON_M9_BAYONET)].skin_index);
	case WEAPON_HUNTSMAN_KNIFE:
		return (HuntsmanKnifeSkin[C_GET(int, Vars.WEAPON_HUNTSMAN_KNIFE)].skin_index);
	case WEAPON_FALCHION_KNIFE:
		return (FalchionKnifeSkin[C_GET(int, Vars.WEAPON_FALCHION_KNIFE)].skin_index);
	case WEAPON_BOWIE_KNIFE:
		return (BowieKnifeSkin[C_GET(int, Vars.WEAPON_BOWIE_KNIFE)].skin_index);
	case WEAPON_BUTTERFLY_KNIFE:
		return (ButterflyKnifeSkin[C_GET(int, Vars.WEAPON_BUTTERFLY_KNIFE)].skin_index);
	case WEAPON_SHADOW_DAGGERS:
		return (ShadowDaggersSkin[C_GET(int, Vars.WEAPON_SHADOW_DAGGERS)].skin_index);
	case WEAPON_PARACORD_KNIFE:
		return (ParacordKnifeSkin[C_GET(int, Vars.WEAPON_PARACORD_KNIFE)].skin_index);
	case WEAPON_SURVIVAL_KNIFE:
		return (SurvivalKnifeSkin[C_GET(int, Vars.WEAPON_SURVIVAL_KNIFE)].skin_index);
	case WEAPON_URSUS_KNIFE:
		return (UrsusKnifeSkin[C_GET(int, Vars.WEAPON_URSUS_KNIFE)].skin_index);
	case WEAPON_NAVAJA_KNIFE:
		return (NavajaKnifeSkin[C_GET(int, Vars.WEAPON_NAVAJA_KNIFE)].skin_index);
	case WEAPON_NOMAD_KNIFE:
		return (NomadKnifeSkin[C_GET(int, Vars.WEAPON_NOMAD_KNIFE)].skin_index);
	case WEAPON_STILETTO_KNIFE:
		return (StilettoKnifeSkin[C_GET(int, Vars.WEAPON_STILETTO_KNIFE)].skin_index);
	case WEAPON_TALON_KNIFE:
		return (TalonKnifeSkin[C_GET(int, Vars.WEAPON_TALON_KNIFE)].skin_index);
	case WEAPON_SKELETON_KNIFE:
		return (SkeletonKnifeSkin[C_GET(int, Vars.WEAPON_SKELETON_KNIFE)].skin_index);
	case 526:
		return (KukriKnifeSkin[C_GET(int, Vars.WEAPON_KUKRI_KNIFE)].skin_index);
	default:
		return 0;
	}
}

void KnifeChanger(int nStage)
{
	if (!C_GET(bool, Vars.bKnifeChanger))
		return;
	if (nStage != 6 || !I::Engine->IsInGame())
		return;
	// not yet implemented
	auto pLocalPawn = SDK::LocalPawn;
	if (!pLocalPawn)
		return;
	auto pWeaponServices = pLocalPawn->GetWeaponServices();
	if (!pWeaponServices)
		return;
	CBaseHandle hWeaponHandle = pWeaponServices->GetActiveWeapon();
	auto pWeapon = I::GameResourceService->pGameEntitySystem->Get<C_CSWeaponBase>(hWeaponHandle);
	if (!pWeapon)
		return;
	auto pWeaponVData = reinterpret_cast<CCSWeaponBaseVData*>(pWeapon->GetVData());
	if (!pWeaponVData)
		return;
	if (pWeaponVData->GetWeaponType() != WEAPONTYPE_KNIFE)
		return;
	auto pViewModelServices = pLocalPawn->GetViewModelServices();
	if (!pViewModelServices)
		return;
	CBaseHandle hViewModelHandle = pViewModelServices->GetViewModel2();
	auto pViewModel2 = I::GameResourceService->pGameEntitySystem->Get<C_CSGOViewModel2>(hViewModelHandle);
	if (!pViewModel2)
		return;
	auto pAttributeContainer = pWeapon->GetAttributeManager();
	if (!pAttributeContainer)
		return;
	auto pWeaponItemView = pAttributeContainer->GetItem();
	if (!pWeaponItemView)
		return;
	auto pWeaponDefinition = pWeaponItemView->GetStaticData();
	if (!pWeaponDefinition)
		return;
	auto pWeaponSceneNode = pWeapon->GetGameSceneNode();
	if (!pWeaponSceneNode)
		return;
	if (pWeapon->GetOriginalOwnerXuid() != SDK::LocalController->GetSteamId()) // only our knife
		return;

	auto modelpath = knifes_data[C_GET(int, Vars.nKnifeIndexModel)].model_path.c_str();
	int def_index = knifes_data[C_GET(int, Vars.nKnifeIndexModel)].def_index;

	pWeaponItemView->GetDisallowSOC() = false;
	pWeaponItemView->GetItemDefinitionIndex() = def_index;
	std::uint32_t hash_ = CUtlStringToken1(std::to_string(def_index).c_str()).GetHashCode1();
	pWeapon->GetSubclassID() = hash_;
	pWeapon->UpdateSubClass();
	pWeapon->UpdateVData();

	pWeapon->SetModel(modelpath);
	CBaseHandle hWeapon = pWeapon->GetRefEHandle();

	if (pViewModel2 && (pViewModel2->GetWeapon().GetEntryIndex() == hWeapon.GetEntryIndex()))
	{
		pViewModel2->SetModel(modelpath);
		pViewModel2->pAnimationGraphInstance->pAnimGraphNetworkedVariables = nullptr;
	}
}

std::vector<INVENTORY::ModelInfo> INVENTORY::flatModelList;

// Replace your custom_models function with this one
c_models_data custom_models(const std::string& folder)
{
	c_models_data data;
	INVENTORY::flatModelList.clear(); // Clear the flat list before populating it

	if (!std::filesystem::is_directory(folder))
		return data;

	for (const auto& entry : std::filesystem::directory_iterator(folder))
	{
		if (!entry.is_directory())
			continue;

		std::string categoryName = entry.path().filename().string();
		std::unordered_map<std::string, std::vector<std::string>> packs;

		for (const auto& pack : std::filesystem::directory_iterator(entry.path()))
		{
			if (!pack.is_directory())
				continue;

			std::string packName = pack.path().filename().string();
			std::vector<std::string> model_list;

			for (const auto& model : std::filesystem::directory_iterator(pack.path()))
			{
				if (model.is_regular_file() && model.path().extension() == CS_XOR(".vmdl_c"))
				{
					std::string model_name = model.path().filename().string();

					if (model_name.find(CS_XOR("_arms")) == std::string::npos &&
						model_name.find(CS_XOR("_arm")) == std::string::npos)
					{
						if (model_name.ends_with(CS_XOR(".vmdl_c")))
							model_name.erase(model_name.size() - 2);

						model_list.push_back(model_name);

						// Add to flat list
						std::string fullPath = folder + "/" + categoryName + "/" + packName + "/" + model_name;
						size_t gameIndex = fullPath.find(CS_XOR("game/csgo/"));
						std::string relativePath = "";

						if (gameIndex != std::string::npos) {
							relativePath = fullPath.substr(gameIndex + 10);
						}

						INVENTORY::flatModelList.push_back({
							categoryName,
							packName,
							model_name,
							fullPath,
							relativePath
							});
					}
				}
			}

			if (!model_list.empty())
				packs[packName] = model_list;
		}

		if (!packs.empty())
			data[categoryName] = packs;
	}

	// Set max model index to the size of the flat list
	INVENTORY::MaxcustomModelIndex = INVENTORY::flatModelList.size();

	return data;
}

void AgentChanger(int nStage)
{
	if (!C_GET(bool, Vars.bModelChanger))
		return;
	if (nStage != 6 || !I::Engine->IsInGame())
		return;
	auto pLocalPawn = SDK::LocalPawn;
	if (!pLocalPawn)
		return;
	int team = pLocalPawn->GetTeam(); // CT == 3 // T == 2

	if (!C_GET(bool, Vars.bCustomModel))
	{
		if (team == 3) // CT
			pLocalPawn->SetModel(agent_data[C_GET(int, Vars.nCtModelIndex)].model_path.c_str());
		if (team == 2) // T
			pLocalPawn->SetModel(agent_data[C_GET(int, Vars.nTModelIndex)].model_path.c_str());
		return;
	}
	else
	{
		static bool customModelLoaded = false;
		static std::vector<int> loadedModelIndices;

		if (!customModelLoaded)
		{
			custom_models_data = custom_models(base_path);
			customModelLoaded = true;

			// Log the loaded models to debug output
			L_PRINT(LOG_INFO) << "Loaded " << INVENTORY::flatModelList.size() << " custom models";
			for (size_t i = 0; i < INVENTORY::flatModelList.size(); i++)
			{
				const auto& model = INVENTORY::flatModelList[i];
				L_PRINT(LOG_INFO) << "[" << i << "] Category: " << model.category.c_str()
					<< ", Pack: " << model.pack.c_str()
					<< ", Model: " << model.modelName.c_str();
			}
		}

		if (INVENTORY::flatModelList.empty())
			return;

		// Validate model index
		if (INVENTORY::customModelIndex >= INVENTORY::flatModelList.size())
			INVENTORY::customModelIndex = 0;

		// Get the selected model from the flat list
		const auto& selectedModel = INVENTORY::flatModelList[INVENTORY::customModelIndex];

		if (selectedModel.relativePath.empty())
			return;

		// Pre-cache and apply the model
		I::RessourceManager->PreCache(selectedModel.relativePath.c_str());

		// Track loaded models
		if (std::find(loadedModelIndices.begin(), loadedModelIndices.end(), INVENTORY::customModelIndex) == loadedModelIndices.end())
			loadedModelIndices.push_back(INVENTORY::customModelIndex);

		// Apply the model
		if (team == 3 || team == 2) // Both CT and T
			pLocalPawn->SetModel(selectedModel.relativePath.c_str());
	}
}

void SkinChanger(int nStage)
{
	if (nStage != 6 || !I::Engine->IsInGame())
		return;
	auto pLocalPawn = SDK::LocalPawn;
	if (!pLocalPawn)
		return;
	auto pWeaponServices = pLocalPawn->GetWeaponServices();
	if (!pWeaponServices)
		return;
	if (!C_GET(bool, Vars.bSkinChanger))
		return;
	CBaseHandle hActiveWeapon = pWeaponServices->GetActiveWeapon();
	C_CSWeaponBase *pActiveWeapon = I::GameResourceService->pGameEntitySystem->Get<C_CSWeaponBase>(hActiveWeapon);
	if (!pActiveWeapon)
		return;
	auto pWeaponAttributeContainer = pActiveWeapon->GetAttributeManager(); //  ->GetItem()->GetItemDefinitionIndex();
	if (!pWeaponAttributeContainer)
		return;
	auto pWeaponItem = pWeaponAttributeContainer->GetItem();
	if (!pWeaponItem)
		return;
	std::uint16_t nWeaponDefinitionIndex = pWeaponItem->GetItemDefinitionIndex();
	if (!nWeaponDefinitionIndex)
		return;
	auto pWeaponSceneNode = pActiveWeapon->GetGameSceneNode();
	if (!pWeaponSceneNode)
		return;
	auto pViewModelServices = pLocalPawn->GetViewModelServices();
	if (!pViewModelServices)
		return;
	CBaseHandle hViewModelHandle = pViewModelServices->GetViewModel2();
	auto pViewModel2 = I::GameResourceService->pGameEntitySystem->Get<C_CSGOViewModel2>(hViewModelHandle);
	if (!pViewModel2)
		return;
	auto pViewSceneNode = pViewModel2->GetGameSceneNode();
	if (!pViewSceneNode)
		return;

	bool bShouldUpdate = pActiveWeapon->GetFallbackPaintKit() != GetPaintIndex(nWeaponDefinitionIndex);
	pWeaponItem->GetDisallowSOC() = false;
	pWeaponItem->GetItemIDHigh() = -1;
	pActiveWeapon->GetFallbackPaintKit() = GetPaintIndex(nWeaponDefinitionIndex);
	pActiveWeapon->GetFallbackWear() = 0.1f;
	pWeaponSceneNode->SetMeshGroupMask(2);
	pViewSceneNode->SetMeshGroupMask(2);
	bShouldUpdate = true;
	if (bShouldUpdate)
	{
		// #STR: "clientside_reload_custom_econ"
		pActiveWeapon->UpdateComposite();
		//"holster_attachment_fallback",
		pActiveWeapon->UpdateCompositeSec();
		pActiveWeapon->UpdateModel(true);
	}

}

void GlovesChanger(int nStage)
{
	try
	{
		if (!C_GET(bool, Vars.bGloveChanger))
			return;
		if (nStage != 6 || !I::Engine->IsInGame())
			return;
		auto pLocalPawn = SDK::LocalPawn;
		if (!pLocalPawn)
			return;
		auto pGloves = &pLocalPawn->GetEconGloves();
		if (!pGloves)
			return;
		static float last_spawn_time = -1.0f;
		try
		{
			if (pGloves->GetItemDefinitionIndex() != glove_data[C_GET(int, Vars.nGloveIndexModel)].def_index || last_spawn_time != pLocalPawn->GetSpawnTimeIndex())
				pGloves->GetItemDefinitionIndex() = glove_data[C_GET(int, Vars.nGloveIndexModel)].def_index;
		}
		catch (...)
		{
			return;
		}
		pGloves->GetInitialized() = true;
		pLocalPawn->SetBodyGroup();
		pLocalPawn->NeedToReApplyGloves() = true;
		pGloves->GetDisallowSOC() = false;
		last_spawn_time = pLocalPawn->GetSpawnTimeIndex();
		pGloves->GetItemIDHigh() = -1;
	}
	catch (...)
	{
		return;
	}
}


void INVENTORY::OnFrameStageNotify(int nStage)
{
	AgentChanger(nStage);
	KnifeChanger(nStage);
	SkinChanger(nStage);
	GlovesChanger(nStage);
}
