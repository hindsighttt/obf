#include "menu.h"

// used: config variables
#include "variables.h"
#include "../Inventory.h"
#include "../skins.h"

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

// used: iinputsystem
#include "interfaces.h"
#include "../sdk/interfaces/iengineclient.h"
#include "../sdk/interfaces/inetworkclientservice.h"
#include "../sdk/interfaces/iglobalvars.h"
#include "../sdk/interfaces/ienginecvar.h"

// used: overlay's context
#include "../features/visuals/overlay.h"

// used: notifications
#include "../utilities/notify.h"

#pragma region menu_array_entries
static constexpr const char* arrMiscDpiScale[] = {
	"100%",
	"125%",
	"150%",
	"175%",
	"200%"
};

enum MenuTab
{
	MenuTab_Rage = 0,
	MenuTab_Legit,
	MenuTab_Visuals,
	MenuTab_Skins,
	MenuTab_Misc,
	MenuTab_Configs,
	MenuTab_About,
	MenuTab_Count
};

static const std::pair<const char*, const std::size_t> arrColors[] = {
	{ "[accent] - main", Vars.colAccent0 },
	{ "[accent] - dark (hover)", Vars.colAccent1 },
	{ "[accent] - darker (active)", Vars.colAccent2 },
	{ "[primitive] - text", Vars.colPrimtv0 },
	{ "[primitive] - background", Vars.colPrimtv1 },
	{ "[primitive] - disabled", Vars.colPrimtv2 },
	{ "[primitive] - frame background", Vars.colPrimtv3 },
	{ "[primitive] - border", Vars.colPrimtv4 },
};

static constexpr const char* arrMenuAddition[] = {
	"dim",
	"particle",
	"glow"
};
#pragma endregion

void MENU::RenderMainWindow()
{
	// INSERT MENU
	ImGuiStyle& style = ImGui::GetStyle();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	static ImVec2 vecMenuPosition = ImVec2(
	(ImGui::GetIO().DisplaySize.x - 790.0f) * 0.5f,
	(ImGui::GetIO().DisplaySize.y - 630.0f) * 0.5f
	);
	if (!bMainWindowOpened)
		return;

	float SideBarWidth = 238.0f;
	float SideBarShadowWidth = 4.0f;



	ImVec2 WindowSize = ImGui::GetWindowSize();
	ImGui::SetNextWindowPos(vecMenuPosition);
	ImGui::SetNextWindowSize(ImVec2(790, 630), ImGuiCond_Always);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2());
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
	ImGui::Begin("obfuscated", &bMainWindowOpened, ImGuiWindowFlags_NoDecoration);
	ImGui::PopStyleVar(2);
	{
		// Render
		{
			ImVec2 pos = ImGui::GetWindowPos();
			ImVec2 size = ImGui::GetWindowSize();
			size.x = 790;
			size.y = 630;
			ImDrawList* pDrawList = ImGui::GetWindowDrawList();

			pDrawList->AddRectFilled(pos, pos + ImVec2(SideBarWidth, size.y), ImGui::GetColorU32(ImGuiCol_PopupBg), style.WindowRounding, ImDrawFlags_RoundCornersLeft);
			pDrawList->AddRectFilled(pos + ImVec2(SideBarWidth, 0), pos + size, ImGui::GetColorU32(ImGuiCol_WindowBg), style.WindowRounding, ImDrawFlags_RoundCornersRight);

			if (style.WindowBorderSize > 0)
			{
				//pDrawList->AddRect(pos, pos + size, ImGui::GetColorU32(ImGuiCol_FrameBgActive), style.WindowRounding, 0, style.WindowBorderSize);
				pDrawList->AddRectFilledMultiColor(pos + ImVec2(SideBarWidth - SideBarShadowWidth, style.WindowBorderSize), pos + ImVec2(SideBarWidth, size.y - style.WindowBorderSize), ImGui::GetColorU32(ImGuiCol_FrameBg, 0.0f), ImGui::GetColorU32(ImGuiCol_FrameBg), ImGui::GetColorU32(ImGuiCol_FrameBg), ImGui::GetColorU32(ImGuiCol_FrameBg, 0.0f));
			}
		}
		// Content
		{
			ImGui::SetCursorScreenPos(ImGui::GetWindowPos());
			ImGui::BeginChild(CS_XOR("SideBar"), ImVec2(SideBarWidth, ImGui::GetWindowHeight()), ImGuiChildFlags_None, ImGuiWindowFlags_NoBackground);
			{
				const float ControlBarSize = 14.0f;

				ImGui::SetCursorScreenPos(ImGui::GetWindowPos() + style.WindowPadding);
				ImGui::BeginChild(CS_XOR("ControlBar"), ImVec2(ImGui::GetWindowWidth() - style.WindowPadding.x * 2, ControlBarSize), ImGuiChildFlags_None, ImGuiWindowFlags_NoBackground);
				{
					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, style.FramePadding);
					{
						ImGui::PushStyleColor(ImGuiCol_Button, ImAdd::HexToColorVec4(0xed6b5e, 1.0f));
						ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImAdd::HexToColorVec4(0xed6b5e, 1.0f));
						ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImAdd::HexToColorVec4(0xed6b5e, 1.0f));
						if (ImAdd::ButtonCircle(CS_XOR("Close"), ImGui::GetWindowHeight() / 2))
						{
							bMainWindowOpened = false;
						}
						ImGui::PopStyleColor(3);

						ImGui::SameLine();

						ImGui::PushStyleColor(ImGuiCol_Button, ImAdd::HexToColorVec4(0xf6be4f, 1.0f));
						ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImAdd::HexToColorVec4(0xf6be4f, 1.0f));
						ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImAdd::HexToColorVec4(0xf6be4f, 1.0f));
						if (ImAdd::ButtonCircle(CS_XOR("Maximize"), ImGui::GetWindowHeight() / 2))
						{

						}
						ImGui::PopStyleColor(3);

						ImGui::SameLine();

						ImGui::PushStyleColor(ImGuiCol_Button, ImAdd::HexToColorVec4(0x61c555, 1.0f));
						ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImAdd::HexToColorVec4(0x61c555, 1.0f));
						ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImAdd::HexToColorVec4(0x61c555, 1.0f));
						if (ImAdd::ButtonCircle(CS_XOR("Minimize"), ImGui::GetWindowHeight() / 2))
						{

						}
						ImGui::PopStyleColor(3);
					}
					ImGui::PopStyleVar();
				}
				ImGui::EndChild();

				ImGui::SetCursorScreenPos(ImGui::GetWindowPos() + ImVec2(style.FramePadding.x + style.CellPadding.x, ControlBarSize + style.WindowPadding.y * 2));
				ImGui::PushItemWidth(ImGui::GetWindowWidth() - (style.FramePadding.x + style.CellPadding.x) * 2);
				{
					ImGui::PushFont(FONT::pMenu[C_GET(int, Vars.nDpiScale)]);
					{
						ImGui::PushStyleVar(
							ImGuiStyleVar_FramePadding,
							ImVec2(
								style.ItemInnerSpacing.y,
								style.ItemInnerSpacing.y
							)
						);
						{
							static char search[200] = "";
							//mAdd::InputText(m_pTextures->tSearch, "Search", m_Search, sizeof(m_Search));
							ImGui::InputText(CS_XOR("##Search"), search, sizeof(search));
						}
						ImGui::PopStyleVar();
					}
					ImGui::PopFont();
				}
				ImGui::PopItemWidth();

				ImGui::SetCursorScreenPos(ImGui::GetWindowPos() + ImVec2(style.FramePadding.x + style.CellPadding.y, ControlBarSize + ImGui::GetFontSize() + style.ItemInnerSpacing.y * 2 + style.WindowPadding.y * 3));
				ImGui::BeginChild(CS_XOR("SideBar"), ImVec2(ImGui::GetWindowWidth() - (style.FramePadding.x + style.CellPadding.y) * 2, 0), ImGuiChildFlags_None, ImGuiWindowFlags_NoBackground);
				{
					ImGui::PushStyleColor(ImGuiCol_Text, ImAdd::HexToColorVec4(0x464646, 1.0f));
					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 1));
					ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, style.ChildRounding);
					{
						
						ImAdd::RadioFrameIcon(nullptr, CS_XOR("Rage"), &C_GET(int, Vars.nCurrentTab), MenuTab_Rage, ImAdd::HexToColorVec4(0xfc5c6c, 1.0f), ImAdd::HexToColorVec4(0xfc2c4d, 1.0f), ImVec2(-0.1f, 0));
						ImAdd::RadioFrameIcon(nullptr, CS_XOR("Legit"), &C_GET(int, Vars.nCurrentTab), MenuTab_Legit, ImAdd::HexToColorVec4(0xfc5c6c, 1.0f), ImAdd::HexToColorVec4(0xfc2c4d, 1.0f), ImVec2(-0.1f, 0));
						ImAdd::RadioFrameIcon(nullptr, CS_XOR("Visuals"), &C_GET(int, Vars.nCurrentTab), MenuTab_Visuals, ImAdd::HexToColorVec4(0x2c91fe, 1.0f), ImAdd::HexToColorVec4(0x0476fc, 1.0f), ImVec2(-0.1f, 0));
						ImAdd::RadioFrameIcon(nullptr, CS_XOR("Skins"), &C_GET(int, Vars.nCurrentTab), MenuTab_Skins, ImAdd::HexToColorVec4(0x2c91fe, 1.0f), ImAdd::HexToColorVec4(0x0476fc, 1.0f), ImVec2(-0.1f, 0));
						ImAdd::RadioFrameIcon(nullptr, CS_XOR("Misc"), &C_GET(int, Vars.nCurrentTab), MenuTab_Misc, ImAdd::HexToColorVec4(0xb7b3b1, 1.0f), ImAdd::HexToColorVec4(0x8f8d8b, 1.0f), ImVec2(-0.1f, 0));

						ImGui::Text(" ");

						ImAdd::RadioFrameIcon(nullptr, CS_XOR("Configs"), &C_GET(int, Vars.nCurrentTab), MenuTab_Configs, ImAdd::HexToColorVec4(0x7574e4, 1.0f), ImAdd::HexToColorVec4(0x5958d7, 1.0f), ImVec2(-0.1f, 0));
						ImAdd::RadioFrameIcon(nullptr, CS_XOR("About"), &C_GET(int, Vars.nCurrentTab), MenuTab_About, ImAdd::HexToColorVec4(0xb7b3b1, 1.0f), ImAdd::HexToColorVec4(0x8f8d8b, 1.0f), ImVec2(-0.1f, 0));
					}
					ImGui::PopStyleVar(2);
					ImGui::PopStyleColor();
				}
				ImGui::EndChild();
			}
			ImGui::EndChild();

			ImGui::SetCursorScreenPos(ImGui::GetWindowPos() + ImVec2(SideBarWidth, 0));
			ImGui::BeginChild(CS_XOR("Content"), ImVec2(ImGui::GetWindowWidth() - SideBarWidth, ImGui::GetWindowHeight()), ImGuiChildFlags_AlwaysUseWindowPadding, ImGuiWindowFlags_NoBackground);
			{
				/*
				ImGui::PushFont(io.Fonts->Fonts[ImFont_Thick]);
				ImGui::Text("   %s", GetPageName(m_iCurrentPage));
				ImGui::PopFont();
				*/

				if (C_GET(int, Vars.nCurrentTab) == MenuTab_Rage)
				{
					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, style.ItemInnerSpacing.y));
					ImGui::BeginGroup();
					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
					//ImGui::PushFont(io.Fonts->Fonts[ImFont_Main_18px]);
					ImGui::Text(CS_XOR("   Rage"));
					//ImGui::PopFont();
					ImGui::TextDisabled(CS_XOR("   description"));
					ImGui::PopStyleVar();
					ImGui::EndGroup();

					ImAdd::BeginChild(CS_XOR("Rage"), ImVec2(0, 520));
					ImGui::PopStyleVar();
					{
						ImAdd::ToggleButton(CS_XOR("Enable ragebot"), &C_GET(bool, Vars.bRageBot));
						ImGui::Separator();
						ImAdd::ToggleButton(CS_XOR("Always on"), &C_GET(bool, Vars.bRageBotAlwaysOn));
						ImGui::Separator();
						if (!C_GET(bool, Vars.bRageBotAlwaysOn))
						{
							ImGui::HotKey(CS_XOR("Ragebot key"), &C_GET(unsigned int, Vars.nRageBotActivationKey));
							ImGui::Separator();
						}
						ImAdd::ToggleButton(CS_XOR("Silent aim"), &C_GET(bool, Vars.bRageSilentAim));
						ImGui::Separator();
						//ImAdd::ToggleButton(CS_XOR("Auto penetration"), &C_GET(bool, Vars.bAutoPenetration));
						//ImGui::Separator();
						ImAdd::ToggleButton(CS_XOR("Auto fire"), &C_GET(bool, Vars.bAutoFire));
						ImGui::Separator();
						//ImAdd::ToggleButton(CS_XOR("No spread"), &C_GET(bool, Vars.bNoSpread));
						//ImGui::Separator();
						ImAdd::ToggleButton(CS_XOR("Auto scope"), &C_GET(bool, Vars.bAutoScope));
						ImGui::Separator();
						ImAdd::SliderFloat(CS_XOR("Hitchance"), &C_GET(float, Vars.flHitchance), 0.0f, 100.0f);
						ImGui::Separator();
						ImAdd::SliderFloat(CS_XOR("Minimum damage"), &C_GET(float, Vars.flMinDmg), 1.0f, 101.0f);
						ImGui::Separator();
						static const char* arrRageHitbox[] = {
							"Head",
							"Chest",
							"Stomach",
							"Pelvis",
							"Arms",
							"Legs",
							"Feet"
						};
						ImGui::MultiCombo(CS_XOR("Hitbox"), &C_GET(unsigned int, Vars.nRageBotHitboxes), arrRageHitbox, IM_ARRAYSIZE(arrRageHitbox));
						ImGui::Separator();
						ImAdd::SliderFloat(CS_XOR("Minimum damage override"), &C_GET(float, Vars.flMinDmgOverride), 1.0f, 101.0f);
						ImGui::Separator();
						ImGui::HotKey("Minimum damage override key", &C_GET(unsigned int, Vars.nRageBotMinDamageKey));
						ImGui::Separator();
						ImAdd::ToggleButton("Autostop", &C_GET(bool, Vars.bAutoStop));
					}
					ImAdd::EndChild();
				}
				else if (C_GET(int, Vars.nCurrentTab) == MenuTab_Legit)
				{
					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, style.ItemInnerSpacing.y));
					ImGui::BeginGroup();
					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
					//ImGui::PushFont(io.Fonts->Fonts[ImFont_Main_18px]);
					ImGui::Text(CS_XOR("   Legit"));
					//ImGui::PopFont();
					ImGui::TextDisabled(CS_XOR("   description"));
					ImGui::PopStyleVar();
					ImGui::EndGroup();

					ImAdd::BeginChild(CS_XOR("Legit"), ImVec2(0, 520));
					ImGui::PopStyleVar();
					{
						ImAdd::Label(CS_XOR("Aimbot"));
						ImAdd::ToggleButton(CS_XOR("Enable##aimbot"), &C_GET(bool, Vars.bLegitbot));
						ImGui::Separator();
						ImAdd::ToggleButton(CS_XOR("Only visible"), &C_GET(bool, Vars.bLegitBotVisCheck));
						ImGui::Separator();
						ImAdd::ToggleButton("Silent aim", &C_GET(bool, Vars.bLegitSilent)); // needs proper implementation still
						ImGui::Separator();
						ImGui::HotKey(CS_XOR("Aimbot key"), &C_GET(unsigned int, Vars.nLegitbotActivationKey));
						ImGui::Separator();
						ImAdd::ToggleButton(CS_XOR("Always on##aimbot"), &C_GET(bool, Vars.bLegitbotAlwaysOn));

						ImGui::Separator();
						ImAdd::Label("Fov mode", ImGui::CalcItemSize(ImVec2(-100, ImGui::GetFontSize() + style.CellPadding.y * 2), 0, 0), ImVec2(0, 0.5f));
						ImGui::SameLine();
						ImAdd::Combo(CS_XOR("aimmode"), &C_GET(int, Vars.nAimMode), CS_XOR("Dynamic\0Static\0"));
						
						if (C_GET(int, Vars.nAimMode) == 1)
						{
							ImGui::Separator();
							ImAdd::ToggleButton(CS_XOR("Fov circle"), &C_GET(bool, Vars.bLegitbotFOV));
						}
						ImGui::Separator();
						//ImGui::Separator();
						//ImAdd::KeyBind("AimBind", &C_GET(int, Vars.nLegitbotActivationKey), 20);


						/*
						ImAdd::Label("Hitbox", ImGui::CalcItemSize(ImVec2(-100, ImGui::GetFontSize() + style.CellPadding.y * 2), 0, 0), ImVec2(0, 0.5f));
						ImGui::SameLine();
						ImAdd::Combo(CS_XOR("hitbox"), &C_GET(int, Vars.nVisualChamMaterial), CS_XOR("Head\0Chest\0"));
						ImGui::Separator();
						*/

						ImAdd::SliderFloat(CS_XOR("Fov"), &C_GET(float, Vars.flAimRange), 0, 130.0f);
						ImGui::Separator();
						ImAdd::SliderFloat(CS_XOR("Smoothing"), &C_GET(float, Vars.flSmoothing), 1.0f, 100.0f);
						ImGui::Separator();
						static const char* arrLegitHitbox[] = {
							"Head",
							"Chest",
							"Stomach"
						};
						ImGui::MultiCombo(CS_XOR("Hitbox"), &C_GET(unsigned int, Vars.nLegitHitbox), arrLegitHitbox, IM_ARRAYSIZE(arrLegitHitbox));

						/*
						static ImVec4 colPick = ImVec4(1, 1, 1, 0.5f);

						ImAdd::Label("Color", ImGui::CalcItemSize(ImVec2(-ImGui::GetFontSize() * 2, ImGui::GetFontSize()), 0, 0), ImVec2(0, 0.5f));
						ImGui::SameLine();
						ImAdd::ColorEdit4("Color Picker", (float*)&colPick);

						ImGui::Separator();

						static int iCombo = 0;
						ImAdd::Label("Combo", ImGui::CalcItemSize(ImVec2(-100, ImGui::GetFontSize() + style.CellPadding.y * 2), 0, 0), ImVec2(0, 0.5f));
						ImGui::SameLine();
						ImAdd::Combo("combo", &iCombo, "item1\0" "item2\0" "item3\0", 100);

						ImGui::Separator();

						static float fSlider = 3;

						ImGui::PushItemWidth(ImGui::CalcItemSize(ImVec2(-0.1f, 0), 0, 0).x);
						ImAdd::SliderFloat("Slider", &fSlider, 0, 10);
						ImGui::PopItemWidth();

						ImGui::Separator();

						ImAdd::Button("button");
						*/

						ImAdd::Label(CS_XOR("##spacing"));
						ImAdd::Label(CS_XOR("Triggerbot"));
						ImAdd::ToggleButton(CS_XOR("Enable##trigerbot"), &C_GET(bool, Vars.bTriggerbot));
						ImGui::Separator();
						ImAdd::ToggleButton(CS_XOR("Always on##triggerbot"), &C_GET(bool, Vars.bTriggerbotAlwaysOn));
						ImGui::Separator();
						ImAdd::ToggleButton(CS_XOR("Scope only##triggerbot"), &C_GET(bool, Vars.bTriggerBotScopedOnly));
						ImGui::Separator();
						ImAdd::ToggleButton(CS_XOR("Auto penetration##triggerbot"), &C_GET(bool, Vars.bTriggerWall));
						ImGui::Separator();
						ImGui::HotKey("Triggerbot key", &C_GET(unsigned int, Vars.nTriggerbotActivationKey));
					}
					ImAdd::EndChild();
				}
				else if (C_GET(int, Vars.nCurrentTab) == MenuTab_Visuals)
				{
					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, style.ItemInnerSpacing.y));
					ImGui::BeginGroup();
					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
					//ImGui::PushFont(io.Fonts->Fonts[ImFont_Main_18px]);
					ImGui::Text(CS_XOR("   Visuals"));
					//ImGui::PopFont();
					ImGui::TextDisabled(CS_XOR("   description"));
					ImGui::PopStyleVar();
					ImGui::EndGroup();
					ImAdd::BeginChild(CS_XOR("Visuals"), ImVec2(0, 520));
					ImGui::PopStyleVar();
					{
						// ESP
						ImAdd::Label(CS_XOR("ESP"));
						ImAdd::ToggleButton(CS_XOR("Enable"), &C_GET(bool, Vars.bVisualOverlay));
						ImGui::Separator();
						ImAdd::ToggleButton(CS_XOR("Box ESP"), &C_GET(FrameOverlayVar_t, Vars.overlayBox).bEnable);
						ImGui::Separator();
						ImAdd::ToggleButton(CS_XOR("Name ESP"), &C_GET(TextOverlayVar_t, Vars.overlayName).bEnable);
						ImGui::Separator();
						ImAdd::ToggleButton(CS_XOR("Health bar"), &C_GET(BarOverlayVar_t, Vars.overlayHealthBar).bEnable);
						ImGui::Separator();
						ImAdd::ToggleButton(CS_XOR("Health text"), &C_GET(bool, Vars.bHealthText));
						//ImAdd::ToggleButton("ArmorBar", &C_GET(BarOverlayVar_t, Vars.overlayArmorBar).bEnable);
						ImGui::Separator();
						ImAdd::ToggleButton(CS_XOR("Weapon ESP"), &C_GET(bool, Vars.bEnemyWeaponEsp));
						ImGui::Separator();
						ImAdd::ToggleButton(CS_XOR("Reveal radar"), &C_GET(bool, Vars.bRadarReveal));

						ImGui::Separator();
						// multicombo box for esp flags
						static const char* arrEspFlags[] = {
							"Defusing",
							"Armor",
							"Scoped",
							"Kit",
							"Reloading",
							"Ping"

							//"Planting",
							//"Bomb"
						};
						ImGui::MultiCombo(CS_XOR("ESP Flags"), &C_GET(unsigned int, Vars.arrESPFlags), arrEspFlags, IM_ARRAYSIZE(arrEspFlags));
						ImAdd::Label(CS_XOR("##spacing"));

						// CHAMS
						ImAdd::Label(CS_XOR("Chams"));
						ImAdd::Label(CS_XOR("Category"), ImGui::CalcItemSize(ImVec2(-100, ImGui::GetFontSize() + style.CellPadding.y * 2), 0, 0), ImVec2(0, 0.5f));
						ImGui::SameLine();
						ImAdd::Combo(CS_XOR("combo"), &C_GET(int, Vars.nChamsCategory), CS_XOR("Enemy\0Team\0Local\0"), 100);

						if (C_GET(int, Vars.nChamsCategory) == CHAMS_ENEMY)
						{
							ImAdd::ToggleButton(CS_XOR("Enable visible"), &C_GET(bool, Vars.bVisualChams));
							ImGui::Separator();

							if (ImAdd::ToggleButton(CS_XOR("Enable invisible"), &C_GET(bool, Vars.bVisualChamsIgnoreZ)))
							{
								auto no_vis = I::Cvar->Find(FNV1A::HashConst("sc_no_vis"));
								if (no_vis)
									no_vis->value.i1 = true;
							}
							ImGui::Separator();

							ImAdd::Label(CS_XOR("Visible material"), ImGui::CalcItemSize(ImVec2(-100, ImGui::GetFontSize() + style.CellPadding.y * 2), 0, 0), ImVec2(0, 0.5f));
							ImGui::SameLine();
							ImAdd::Combo(CS_XOR("vismat"), &C_GET(int, Vars.nVisualChamMaterial), CS_XOR("Illuminate\0Flat\0Default\0Glow\0Glass\0Metal\0"));
							ImGui::Separator();

							ImAdd::Label(CS_XOR("Visible color"), ImGui::CalcItemSize(ImVec2(-ImGui::GetFontSize() * 2, ImGui::GetFontSize()), 0, 0), ImVec2(0, 0.5f));
							ImGui::SameLine();
							ImAdd::ColorEdit4(CS_XOR("##viscol"), &C_GET(Color_t, Vars.colVisualChams));
							ImGui::Separator();

							ImAdd::Label(CS_XOR("Invisible material"), ImGui::CalcItemSize(ImVec2(-100, ImGui::GetFontSize() + style.CellPadding.y * 2), 0, 0), ImVec2(0, 0.5f));
							ImGui::SameLine();
							ImAdd::Combo(CS_XOR("invismat"), &C_GET(int, Vars.nVisualChamMaterialIgnoreZ), CS_XOR("Illuminate\0Flat\0Default\0Glow\0Glass\0Metal\0"));
							ImGui::Separator();

							ImAdd::Label(CS_XOR("Invisible color"), ImGui::CalcItemSize(ImVec2(-ImGui::GetFontSize() * 2, ImGui::GetFontSize()), 0, 0), ImVec2(0, 0.5f));
							ImGui::SameLine();
							ImAdd::ColorEdit4(CS_XOR("##inviscol"), &C_GET(Color_t, Vars.colVisualChamsIgnoreZ));
							ImGui::Separator();
							ImAdd::ToggleButton(CS_XOR("Ragdoll"), &C_GET(bool, Vars.bRagdollChams));
						}
						else if (C_GET(int, Vars.nChamsCategory) == CHAMS_FRIENDLY)
						{
							ImAdd::ToggleButton(CS_XOR("Enable visible"), &C_GET(bool, Vars.bFriendlyVisualChams));
							ImGui::Separator();

							if (ImAdd::ToggleButton(CS_XOR("Enable invisible"), &C_GET(bool, Vars.bFriendlyVisualChamsIgnoreZ)))
							{
								auto no_vis = I::Cvar->Find(FNV1A::HashConst("sc_no_vis"));
								if (no_vis)
									no_vis->value.i1 = true;
							}
							ImGui::Separator();

							ImAdd::Label(CS_XOR("Visible material"), ImGui::CalcItemSize(ImVec2(-100, ImGui::GetFontSize() + style.CellPadding.y * 2), 0, 0), ImVec2(0, 0.5f));
							ImGui::SameLine();
							ImAdd::Combo(CS_XOR("vismat"), &C_GET(int, Vars.nFriendlyVisualChamMaterial), CS_XOR("Illuminate\0Flat\0Default\0Glow\0Glass\0Metal\0"));
							ImGui::Separator();

							ImAdd::Label(CS_XOR("Visible color"), ImGui::CalcItemSize(ImVec2(-ImGui::GetFontSize() * 2, ImGui::GetFontSize()), 0, 0), ImVec2(0, 0.5f));
							ImGui::SameLine();
							ImAdd::ColorEdit4(CS_XOR("##viscol"), &C_GET(Color_t, Vars.colFriendlyVisualChams));
							ImGui::Separator();

							ImAdd::Label(CS_XOR("Invisible material"), ImGui::CalcItemSize(ImVec2(-100, ImGui::GetFontSize() + style.CellPadding.y * 2), 0, 0), ImVec2(0, 0.5f));
							ImGui::SameLine();
							ImAdd::Combo(CS_XOR("invismat"), &C_GET(int, Vars.nFriendlyVisualChamMaterialIgnoreZ), CS_XOR("Illuminate\0Flat\0Default\0Glow\0Glass\0Metal\0"));
							ImGui::Separator();

							ImAdd::Label(CS_XOR("Invisible color"), ImGui::CalcItemSize(ImVec2(-ImGui::GetFontSize() * 2, ImGui::GetFontSize()), 0, 0), ImVec2(0, 0.5f));
							ImGui::SameLine();
							ImAdd::ColorEdit4(CS_XOR("##inviscol"), &C_GET(Color_t, Vars.colFriendlyVisualChamsIgnoreZ));
							ImGui::Separator();

							ImAdd::ToggleButton(CS_XOR("Ragdoll"), &C_GET(bool, Vars.bFriendlyRagdollChams));
						}
						else if (C_GET(int, Vars.nChamsCategory) == CHAMS_LOCAL)
						{
							ImAdd::ToggleButton(CS_XOR("Weapon"), &C_GET(bool, Vars.bLocalWeaponChams));
							ImGui::Separator();

							ImAdd::Label(CS_XOR("Weapon material"), ImGui::CalcItemSize(ImVec2(-100, ImGui::GetFontSize() + style.CellPadding.y * 2), 0, 0), ImVec2(0, 0.5f));
							ImGui::SameLine();
							ImAdd::Combo(CS_XOR("weaponmat"), &C_GET(int, Vars.nLocalWeaponChamsMaterial), CS_XOR("Illuminate\0Flat\0Default\0Glow\0Glass\0Metal\0"));
							ImGui::Separator();

							ImAdd::Label(CS_XOR("weapon color"), ImGui::CalcItemSize(ImVec2(-ImGui::GetFontSize() * 2, ImGui::GetFontSize()), 0, 0), ImVec2(0, 0.5f));
							ImGui::SameLine();
							ImAdd::ColorEdit4(CS_XOR("##weaponcol"), &C_GET(Color_t, Vars.colLocalWeaponChamsColor));
							ImGui::Separator();

							ImAdd::ToggleButton(CS_XOR("Viewmodel"), &C_GET(bool, Vars.bLocalHandsChams));
							ImGui::Separator();

							ImAdd::Label(CS_XOR("Viewmodel material"), ImGui::CalcItemSize(ImVec2(-100, ImGui::GetFontSize() + style.CellPadding.y * 2), 0, 0), ImVec2(0, 0.5f));
							ImGui::SameLine();
							ImAdd::Combo(CS_XOR("viewmodelmat"), &C_GET(int, Vars.nLocalHandsChamsMaterial), CS_XOR("Illuminate\0Flat\0Default\0Glow\0Glass\0Metal\0"));
							ImGui::Separator();

							ImAdd::Label(CS_XOR("Viewmodel color"), ImGui::CalcItemSize(ImVec2(-ImGui::GetFontSize() * 2, ImGui::GetFontSize()), 0, 0), ImVec2(0, 0.5f));
							ImGui::SameLine();
							ImAdd::ColorEdit4(CS_XOR("##viewmodelcolor"), &C_GET(Color_t, Vars.colLocalHandsChamsColor));
							ImGui::Separator();
						}
						ImAdd::Label(CS_XOR("##ssspacing"));
						ImAdd::Label(CS_XOR("Others"));
						ImAdd::ToggleButton(CS_XOR("Grenade camera"), &C_GET(bool, Vars.bGrenadeCamera));
						ImGui::Separator();
						ImAdd::ToggleButton(CS_XOR("Change fov##toggle"), &C_GET(bool, Vars.bFovChange));
						if (C_GET(bool, Vars.bFovChange))
						{
							ImGui::Separator();
							ImAdd::SliderFloat(CS_XOR("Value"), &C_GET(float, Vars.flFov), 0, 200);
							ImGui::Separator();
							ImAdd::ToggleButton(CS_XOR("Override zoom Fov"), &C_GET(bool, Vars.bOverrideZoom));
						}
						ImGui::Separator();
						ImAdd::ToggleButton(CS_XOR("Remove scope overlay"), &C_GET(bool, Vars.bRemoveScope));
						ImGui::Separator();
						ImAdd::ToggleButton(CS_XOR("Thirdperson"), &C_GET(bool, Vars.bThirdPerson));
						if (C_GET(bool, Vars.bThirdPerson))
						{
							ImGui::Separator();
							ImAdd::SliderFloat(CS_XOR("Distance"), &C_GET(float, Vars.flThirdPersonDistance), 20.0f, 500.0f);
							ImGui::Separator();
							//ImAdd::KeyBind(CS_XOR("Thirdperson Key"), &C_GET(int, Vars.nThirdPersonKey), 20);
							ImGui::HotKey(CS_XOR("Thirdperson Key"), &C_GET(unsigned int, Vars.nThirdPersonKey));
						}
						ImGui::Separator();
						ImAdd::ToggleButton(CS_XOR("Spread circle"), &C_GET(bool, Vars.bSpreadCircle));
						if (C_GET(bool, Vars.bSpreadCircle))
						{
							ImGui::Separator();
							ImAdd::Label(CS_XOR("Spread circle color"), ImGui::CalcItemSize(ImVec2(-ImGui::GetFontSize() * 2, ImGui::GetFontSize()), 0, 0), ImVec2(0, 0.5f));
							ImGui::SameLine();
							ImAdd::ColorEdit4(CS_XOR("##circlecolor"), &C_GET(Color_t, Vars.colSpreadCircle));
	
						}
						ImGui::Separator();
						ImAdd::ToggleButton(CS_XOR("Penetration crosshair"), &C_GET(bool, Vars.bAutoWallCrosshair));
						ImGui::Separator();
						ImAdd::ToggleButton(CS_XOR("Enable hitmarkers"), &C_GET(bool, Vars.bHitmarker));
						ImGui::Separator();
						ImAdd::ToggleButton(CS_XOR("Enable hitsound"), &C_GET(bool, Vars.bHitsound));
					}
					ImAdd::EndChild();
				}
				else if (C_GET(int, Vars.nCurrentTab) == MenuTab_Misc)
				{
					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, style.ItemInnerSpacing.y));
					ImGui::BeginGroup();
					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
					//ImGui::PushFont(io.Fonts->Fonts[ImFont_Main_18px]);
					ImGui::Text(CS_XOR("   Misc"));
					//ImGui::PopFont();
					ImGui::TextDisabled(CS_XOR("   description"));
					ImGui::PopStyleVar();
					ImGui::EndGroup();
					ImAdd::BeginChild(CS_XOR("Misc"), ImVec2(0, 520));
					ImGui::PopStyleVar();
					{
						ImAdd::Label(CS_XOR("Movement"));

						ImAdd::ToggleButton(CS_XOR("Auto Bunnyhop"), &C_GET(bool, Vars.bAutoBHop));
						ImGui::Separator();

						ImAdd::ToggleButton(CS_XOR("Auto strafe"), &C_GET(bool, Vars.bAutoStrafe));

						ImAdd::Label(CS_XOR("##spacing"));

						ImAdd::Label(CS_XOR("Other"));
						ImAdd::ToggleButton(CS_XOR("Menu dark mode"), &C_GET(bool, Vars.bMenuDarkMode));
						ImGui::Separator();
						if (ImAdd::Button(CS_XOR("Unlock hidden CVARS")))
							I::Cvar->UnlockHiddenCVars();


					}
					ImAdd::EndChild();
				}
				else if (C_GET(int, Vars.nCurrentTab) == MenuTab_Skins)
				{
					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, style.ItemInnerSpacing.y));
					ImGui::BeginGroup();
					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
					//ImGui::PushFont(io.Fonts->Fonts[ImFont_Main_18px]);
					ImGui::Text(CS_XOR("   Skins"));
					//ImGui::PopFont();
					ImGui::TextDisabled(CS_XOR("   description"));
					ImGui::PopStyleVar();
					ImGui::EndGroup();
					ImAdd::BeginChild(CS_XOR("Skins"), ImVec2(0, 520));
					ImGui::PopStyleVar();
					{
						ImAdd::Label(CS_XOR("Knife##label"));
						ImAdd::ToggleButton(CS_XOR("Enable##knifeEnable"), &C_GET(bool, Vars.bKnifeChanger));
						if (C_GET(bool, Vars.bKnifeChanger))
						{
							ImGui::Separator();
							ImAdd::Label(CS_XOR("Knife"), ImGui::CalcItemSize(ImVec2(-330, ImGui::GetFontSize() + style.CellPadding.y * 2), 0, 0), ImVec2(0, 0.5f));
							ImGui::SameLine();
							ImAdd::Combo(CS_XOR("KnifeModel"), &C_GET(int, Vars.nKnifeIndexModel), CS_XOR("M9 Bayonet\0Classic\0Bayonet\0Flip\0Gut\0Karambit\0Tactical\0Falchion\0Bowie\0Butterfly\0Push\0Cord\0Canis\0Ursus\0Navaja\0Outdoor\0Stiletto\0Talon\0Skeleton\0Kukri\0"));

						}

						ImAdd::Label(CS_XOR("##spacinglol"));
						ImAdd::Label(CS_XOR("Gloves"));
						ImAdd::ToggleButton(CS_XOR("Enable##GloveChanger"), &C_GET(bool, Vars.bGloveChanger));
						if (C_GET(bool, Vars.bGloveChanger))
						{
							ImGui::Separator();
							ImAdd::Label(CS_XOR("Gloves"), ImGui::CalcItemSize(ImVec2(-330, ImGui::GetFontSize() + style.CellPadding.y * 2), 0, 0), ImVec2(0, 0.5f));
							ImGui::SameLine();
							ImAdd::Combo(CS_XOR("GloveModel"), &C_GET(int, Vars.nGloveIndexModel), CS_XOR("Bloodhound Gloves\0Sport Gloves\0Driver Gloves\0Hand Wraps\0Moto Gloves\0Specialist Gloves\0Hydra Gloves\0Broken Fang Gloves\0"));
						}

						ImAdd::Label(CS_XOR("##spacinglossl"));
						ImAdd::Label(CS_XOR("Skin"));
						ImAdd::ToggleButton(CS_XOR("Enable##SkinChanger"), &C_GET(bool, Vars.bSkinChanger));
						if (C_GET(bool, Vars.bSkinChanger))
						{
							ImGui::Separator();
							ImAdd::Label(CS_XOR("Weapon"), ImGui::CalcItemSize(ImVec2(-330, ImGui::GetFontSize() + style.CellPadding.y * 2), 0, 0), ImVec2(0, 0.5f));
							ImGui::SameLine();
							ImGui::Combo(CS_XOR("##weaponselect"), &INVENTORY::current_weapon_index, CS_XOR("Deagle\0Dual Berretas\0Five-Seven\0Glock-18\0AK-47\0AUG\0AWP\0Famas\0G3SG1\0Galil-Ar\0M249\0M4A4\0MAC-10\0P90\0MP5-SD\0UMP-45\0XM1014\0PP-BIZON\0MAG-7\0NEGEV\0SAWED-OFF\0TEC-9\0Zeus\0P2000\0MP7\0MP9\0NOVA\0P25\0Scar-20\0SG553\0SSG-08\0M4A1-S\0USP-S\0CZ75-AUTO\0R8 Revolver\0Bayonet\0Classic Knife\0Flip Knife\0Gut Knife\0Karambit\0M9 Bayonet\0Huntsman Knife\0Falchion Knife\0Bowie Knife\0Butterfly Knife\0Shadow daggers\0Paracord Knife\0Survival Knife\0Ursus Knife\0Navaja Knife\0Nomad Knife\0Stiletto Knife\0Talon Knife\0Skeleton Knife\0Kukri Knife\0"));

							ImGui::Separator();
							ImAdd::Label(CS_XOR("Selected skin"), ImGui::CalcItemSize(ImVec2(-330, ImGui::GetFontSize() + style.CellPadding.y * 2), 0, 0), ImVec2(0, 0.5f));
							ImGui::SameLine();
							ImGui::Combo(CS_XOR("##selectedskin"), GetSkinVar(INVENTORY::current_weapon_index), GetSkinStr(INVENTORY::current_weapon_index));
						}

						ImAdd::Label(CS_XOR("##spacingggggsss"));
						ImAdd::Label(CS_XOR("Agent##Label"));
						ImAdd::ToggleButton(CS_XOR("Enable##agentenable"), &C_GET(bool, Vars.bModelChanger));
						if (C_GET(bool, Vars.bModelChanger))
						{
							ImGui::Separator();
							ImAdd::ToggleButton(CS_XOR("Custom models"), &C_GET(bool, Vars.bCustomModel));
						}
						if (C_GET(bool, Vars.bModelChanger) && !C_GET(bool, Vars.bCustomModel))
						{
							ImGui::Separator();
							ImAdd::Label(CS_XOR("Counter-Terrorist"), ImGui::CalcItemSize(ImVec2(-330, ImGui::GetFontSize() + style.CellPadding.y * 2), 0, 0), ImVec2(0, 0.5f));
							ImGui::SameLine();
							ImGui::Combo(CS_XOR("##ctmodel"), &C_GET(int, Vars.nCtModelIndex), CS_XOR("Ground Rebel  | Elite Crew\0Osiris | Elite Crew\0Prof. Shahmat | Elite Crew\0The Elite Mr. Muhlik | Elite Crew\0Jungle Rebel | Elite Crew\0Soldier | Phoenix\0Enforcer | Phoenix\0Slingshot | Phoenix\0Street Soldier | Phoenix\0Operator | FBI SWAT\0Markus Delrow | FBI HRT\0Michael Syfers  | FBI Sniper\0Special Agent Ava | FBI\03rd Commando Company | KSK\0Seal Team 6 Soldier | NSWC SEAL\0Buckshot | NSWC SEAL\0'Two Times' McCoy | USAF TACP\0Lt. Commander Ricksaw | NSWC SEAL\0'Blueberries' Buckshot | NSWC SEAL\0'Two Times' McCoy | TACP Cavalry\0Primeiro Tenente | Brazilian 1st Battalion\0Dragomir | Sabre\0Maximus | Sabre\0Rezan The Ready | Sabre\0Blackwolf | Sabre\0'The Doctor' Romanov | Sabre\0Rezan the Redshirt | Sabre\0Dragomir | Sabre Footsoldier\0B Squadron Officer | SAS\0D Squadron Officer | NZSAS\0Cmdr. Mae 'Dead Cold' Jamison | SWAT\01st Lieutenant Farlow | SWAT\0John 'Van Healen' Kask | SWAT\0Bio-Haz Specialist | SWAT\0Sergeant Bombson | SWAT\0Chem-Haz Specialist | SWAT\0Lieutenant 'Tree Hugger' Farlow | SWAT\0Sir Bloody Miami Darryl | The Professionals\0Sir Bloody Silent Darryl | The Professionals\0Sir Bloody Skullhead Darryl | The Professionals\0Sir Bloody Darryl Royale | The Professionals\0Sir Bloody Loudmouth Darryl | The Professionals\0Safecracker Voltzmann | The Professionals\0Little Kev | The Professionals\0Number K | The Professionals\0Getaway Sally | The Professionals\0Bloody Darryl The Strapped | The Professionals\0Sous-Lieutenant Medic | Gendarmerie Nationale\0Chem-Haz Capitaine | Gendarmerie Nationale\0Chef d'Escadron Rouchard | Gendarmerie Nationale\0Aspirant | Gendarmerie Nationale\0Officer Jacques Beltram | Gendarmerie Nationale\0Cmdr. Davida 'Goggles' Fernandez | SEAL Frogman\0Cmdr. Frank 'Wet Sox' Baroud | SEAL Frogman\0Lieutenant Rex Krikey | SEAL Frogman\0Elite Trapper Solman | Guerrilla Warfare\0Crasswater The Forgotten | Guerrilla Warfare\0Arno The Overgrown | Guerrilla Warfare\0Col. Mangos Dabisi | Guerrilla Warfare\0Vypa Sista of the Revolution | Guerrilla Warfare\0Trapper Aggressor | Guerrilla Warfare\0'Medium Rare' Crasswater | Guerrilla Warfare\0Trapper | Guerrilla Warfare\0"));
							ImGui::Separator();
							ImAdd::Label(CS_XOR("Terrorist"), ImGui::CalcItemSize(ImVec2(-330, ImGui::GetFontSize() + style.CellPadding.y * 2), 0, 0), ImVec2(0, 0.5f));
							ImGui::SameLine();
							ImGui::Combo(CS_XOR("##tmodel"), &C_GET(int, Vars.nTModelIndex), CS_XOR("Ground Rebel  | Elite Crew\0Osiris | Elite Crew\0Prof. Shahmat | Elite Crew\0The Elite Mr. Muhlik | Elite Crew\0Jungle Rebel | Elite Crew\0Soldier | Phoenix\0Enforcer | Phoenix\0Slingshot | Phoenix\0Street Soldier | Phoenix\0Operator | FBI SWAT\0Markus Delrow | FBI HRT\0Michael Syfers  | FBI Sniper\0Special Agent Ava | FBI\03rd Commando Company | KSK\0Seal Team 6 Soldier | NSWC SEAL\0Buckshot | NSWC SEAL\0'Two Times' McCoy | USAF TACP\0Lt. Commander Ricksaw | NSWC SEAL\0'Blueberries' Buckshot | NSWC SEAL\0'Two Times' McCoy | TACP Cavalry\0Primeiro Tenente | Brazilian 1st Battalion\0Dragomir | Sabre\0Maximus | Sabre\0Rezan The Ready | Sabre\0Blackwolf | Sabre\0'The Doctor' Romanov | Sabre\0Rezan the Redshirt | Sabre\0Dragomir | Sabre Footsoldier\0B Squadron Officer | SAS\0D Squadron Officer | NZSAS\0Cmdr. Mae 'Dead Cold' Jamison | SWAT\01st Lieutenant Farlow | SWAT\0John 'Van Healen' Kask | SWAT\0Bio-Haz Specialist | SWAT\0Sergeant Bombson | SWAT\0Chem-Haz Specialist | SWAT\0Lieutenant 'Tree Hugger' Farlow | SWAT\0Sir Bloody Miami Darryl | The Professionals\0Sir Bloody Silent Darryl | The Professionals\0Sir Bloody Skullhead Darryl | The Professionals\0Sir Bloody Darryl Royale | The Professionals\0Sir Bloody Loudmouth Darryl | The Professionals\0Safecracker Voltzmann | The Professionals\0Little Kev | The Professionals\0Number K | The Professionals\0Getaway Sally | The Professionals\0Bloody Darryl The Strapped | The Professionals\0Sous-Lieutenant Medic | Gendarmerie Nationale\0Chem-Haz Capitaine | Gendarmerie Nationale\0Chef d'Escadron Rouchard | Gendarmerie Nationale\0Aspirant | Gendarmerie Nationale\0Officer Jacques Beltram | Gendarmerie Nationale\0Cmdr. Davida 'Goggles' Fernandez | SEAL Frogman\0Cmdr. Frank 'Wet Sox' Baroud | SEAL Frogman\0Lieutenant Rex Krikey | SEAL Frogman\0Elite Trapper Solman | Guerrilla Warfare\0Crasswater The Forgotten | Guerrilla Warfare\0Arno The Overgrown | Guerrilla Warfare\0Col. Mangos Dabisi | Guerrilla Warfare\0Vypa Sista of the Revolution | Guerrilla Warfare\0Trapper Aggressor | Guerrilla Warfare\0'Medium Rare' Crasswater | Guerrilla Warfare\0Trapper | Guerrilla Warfare\0"));
						}
						if (C_GET(bool, Vars.bModelChanger) && C_GET(bool, Vars.bCustomModel))
						{
							ImGui::Separator();
							ImAdd::SliderInt("Custom model index", &INVENTORY::customModelIndex, 0, INVENTORY::MaxcustomModelIndex - 1);
						}
						ImAdd::Label("##spacingggggssssss");
						if (ImAdd::Button(CS_XOR("Force update")))
						{
							I::NetworkClientService->GetNetworkGameClient()->FullUpdate();
						}
						ImGui::SameLine();
						ImAdd::Label("Can cause crashes (especially with the agent changer)");
					}
					ImAdd::EndChild();
				}
				else if (C_GET(int, Vars.nCurrentTab) == MenuTab_Configs)
				{
					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, style.ItemInnerSpacing.y));
					ImGui::BeginGroup();
					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
					//ImGui::PushFont(io.Fonts->Fonts[ImFont_Main_18px]);
					ImGui::Text(CS_XOR("   Config"));
					//ImGui::PopFont();
					ImGui::TextDisabled(CS_XOR("   description"));
					ImGui::PopStyleVar();
					ImGui::EndGroup();

					ImAdd::BeginChild(CS_XOR("Config"), ImVec2(0, 520));
					ImGui::PopStyleVar();
					{
						if (T::nSelectedConfig == ~1U)
						{
							// set default configuration as selected on first use
							for (std::size_t i = 0U; i < C::vecFileNames.size(); i++)
							{
								if (CRT::StringCompare(C::vecFileNames[i], CS_XOR(CS_CONFIGURATION_DEFAULT_FILE_NAME CS_CONFIGURATION_FILE_EXTENSION)) == 0)
									T::nSelectedConfig = i;
							}
						}
						if (ImGui::BeginListBox(CS_XOR("##config.list"), C::vecFileNames.size(), 5))
						{
							for (std::size_t i = 0U; i < C::vecFileNames.size(); i++)
							{
								// @todo: imgui cant work with wstring
								const wchar_t* wszFileName = C::vecFileNames[i];

								char szFileName[MAX_PATH] = {};
								CRT::StringUnicodeToMultiByte(szFileName, CS_ARRAYSIZE(szFileName), wszFileName);

								// Remove ".obf" at the end if present (only for menu display)
								const char* binExt = ".obf";
								size_t len = strlen(szFileName);
								size_t binLen = strlen(binExt);
								if (len > binLen && strcmp(szFileName + len - binLen, binExt) == 0)
									szFileName[len - binLen] = '\0';

								if (ImGui::Selectable(szFileName, (T::nSelectedConfig == i)))
									T::nSelectedConfig = i;
							}
							ImGui::EndListBox();
						}
						ImGui::InputText(CS_XOR("##configfile"), T::szConfigFile, sizeof(T::szConfigFile));
						ImGui::Separator();
						if (ImAdd::Button(CS_XOR("Create")))
						{
							if (const std::size_t nConfigFileLength = CRT::StringLength(T::szConfigFile); nConfigFileLength > 0U)
							{
								CRT::WString_t wszConfigFile(T::szConfigFile);

								if (C::CreateFile(wszConfigFile.Data()))
									// set created config as selected @todo: dependent on current 'C::CreateFile' behaviour, generally it must be replaced by search
									T::nSelectedConfig = C::vecFileNames.size() - 1U;
								// clear string
								CRT::MemorySet(T::szConfigFile, 0U, sizeof(T::szConfigFile));
							}
						}
						ImGui::SameLine(0, 5.0F);
						if (ImAdd::Button(CS_XOR("Save")))
							C::SaveFile(T::nSelectedConfig);
						ImGui::SameLine(0, 5.0F);
						if (ImAdd::Button(CS_XOR("Load")))
							C::LoadFile(T::nSelectedConfig);
						ImGui::SameLine(0, 5.0F);
						if (ImAdd::Button(CS_XOR("Delete")))
							C::RemoveFile(T::nSelectedConfig);
						ImGui::SameLine(0, 5.0F);
						if (ImAdd::Button(CS_XOR("Refresh")))
							C::Refresh();
					}
					ImAdd::EndChild();
				}
			}
			ImGui::EndChild();
		}
	}
	ImGui::End();
}

void MENU::RenderOverlayPreviewWindow()
{
	using namespace F::VISUALS::OVERLAY;

	ImGuiStyle& style = ImGui::GetStyle();
	// @note: call this function inside rendermainwindow, else expect a crash...
	const ImVec2 vecMenuPos = ImGui::GetWindowPos();
	const ImVec2 vecMenuSize = ImGui::GetWindowSize();

	const ImVec2 vecOverlayPadding = ImVec2(30.f * flDpiScale, 50.f * flDpiScale);

	ImGui::SetNextWindowPos(ImVec2(vecMenuPos.x + vecMenuSize.x + style.WindowPadding.x, vecMenuPos.y), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(175 * flDpiScale, vecMenuSize.y), ImGuiCond_Always);
	ImGui::Begin(CS_XOR("preview"), &bMainWindowOpened, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
	{
		const ImVec2 vecWindowPos = ImGui::GetWindowPos();
		const ImVec2 vecWindowSize = ImGui::GetWindowSize();

		ImDrawList* pDrawList = ImGui::GetWindowDrawList();
		Context_t context;

		ImVec4 vecBox = {
			vecWindowPos.x + vecOverlayPadding.x,
			vecWindowPos.y + vecOverlayPadding.y,
			vecWindowPos.x + vecWindowSize.x - vecOverlayPadding.x,
			vecWindowPos.y + vecWindowSize.y - vecOverlayPadding.y
		};

		if (const auto& boxOverlayConfig = C_GET(FrameOverlayVar_t, Vars.overlayBox); boxOverlayConfig.bEnable)
		{
			const bool bHovered = context.AddBoxComponent(pDrawList, vecBox, 1, boxOverlayConfig.flThickness, boxOverlayConfig.flRounding, boxOverlayConfig.colPrimary, boxOverlayConfig.colOutline);
			if (bHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
				ImGui::OpenPopup(CS_XOR("context##box.component"));
		}

		if (const auto& nameOverlayConfig = C_GET(TextOverlayVar_t, Vars.overlayName); nameOverlayConfig.bEnable)
			context.AddComponent(new CTextComponent(true, SIDE_TOP, DIR_TOP, FONT::pVisual, CS_XOR("obfuscated"), Vars.overlayName));

		if (const auto& healthOverlayConfig = C_GET(BarOverlayVar_t, Vars.overlayHealthBar); healthOverlayConfig.bEnable)
		{
			const float flFactor = M_SIN(ImGui::GetTime() * 5.f) * 0.55f + 0.45f;
			context.AddComponent(new CBarComponent(true, SIDE_LEFT, vecBox, flFactor, Vars.overlayHealthBar));
		}

		if (const auto& armorOverlayConfig = C_GET(BarOverlayVar_t, Vars.overlayArmorBar); armorOverlayConfig.bEnable)
		{
			const float flArmorFactor = M_SIN(ImGui::GetTime() * 5.f) * 0.55f + 0.45f;
			context.AddComponent(new CBarComponent(false, SIDE_BOTTOM, vecBox, flArmorFactor, Vars.overlayArmorBar));
		}

		// only render context preview if overlay is enabled
		context.Render(pDrawList, vecBox);

		if (ImGui::BeginPopup(CS_XOR("context##box.component"), ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, 0));

			ImGui::ColorEdit4(CS_XOR("primary color##box.component"), &C_GET(FrameOverlayVar_t, Vars.overlayBox).colPrimary);
			ImGui::ColorEdit4(CS_XOR("outline color##box.component"), &C_GET(FrameOverlayVar_t, Vars.overlayBox).colOutline);
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.75f);
			ImGui::SliderFloat(CS_XOR("thickness##box.component"), &C_GET(FrameOverlayVar_t, Vars.overlayBox).flThickness, 1.f, 5.f, CS_XOR("%.1f"), ImGuiSliderFlags_AlwaysClamp);
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.75f);
			ImGui::SliderFloat(CS_XOR("rounding##box.component"), &C_GET(FrameOverlayVar_t, Vars.overlayBox).flRounding, 1.f, 5.f, CS_XOR("%.1f"), ImGuiSliderFlags_AlwaysClamp);

			ImGui::PopStyleVar();

			ImGui::EndPopup();
		}
	}
	ImGui::End();
}

void MENU::RenderWatermark()
{
	if (!C_GET(bool, Vars.bWatermark) || !bMainWindowOpened)
		return;

	ImGuiStyle& style = ImGui::GetStyle();

	ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(0.f, 0.f, 0.f, 0.03f));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.f, 0.f, 0.f, 0.03f));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.f, 0.f, 0.f, 0.03f));
	ImGui::PushFont(FONT::pExtra);
	ImGui::BeginMainMenuBar();
	{
		ImGui::Dummy(ImVec2(1, 1));

#ifdef _DEBUG
		ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), CS_XOR("debug"));
#endif
		if (CRT::StringString(GetCommandLineW(), CS_XOR(L"-insecure")) != nullptr)
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), CS_XOR("insecure"));

		if (I::Engine->IsInGame())
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), CS_XOR("in-game"));

		static ImVec2 vecNameSize = ImGui::CalcTextSize(CS_XOR("obfuscated | " __DATE__ " " __TIME__));
		ImGui::SameLine(ImGui::GetContentRegionMax().x - vecNameSize.x - style.FramePadding.x);
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), CS_XOR("obfuscated | " __DATE__ " " __TIME__));
	}
	ImGui::EndMainMenuBar();
	ImGui::PopFont();
	ImGui::PopStyleColor(3);
}

void MENU::UpdateStyle2(ImGuiStyle* pStyle)
{
	ImGuiStyle& style = pStyle != nullptr ? *pStyle : ImGui::GetStyle();

	ImGui::StyleColorsDark();

	// Custom Styles
	style.WindowRounding = 10;
	style.ChildRounding = 6;
	style.FrameRounding = 4;
	style.TabRounding = 4;
	style.ScrollbarRounding = 2;

	style.ButtonTextAlign = { 0.5f, 0.5f };
	style.WindowTitleAlign = { 0.5f, 0.5f };
	style.FramePadding = { 10.0f, 10.0f };
	style.WindowPadding = { 22.0f, 22.0f };
	style.ItemSpacing = { 10.0f, 30.0f };
	style.ItemInnerSpacing = { 8.0f, 8.0f };
	style.CellPadding = { 4.0f, 3.0f };

	style.WindowBorderSize = 1;
	style.FrameBorderSize = 0;
	style.PopupBorderSize = 1;

	style.ScrollbarSize = 12;
	style.GrabMinSize = 6;

	style.PopupRounding = 4;

	// Custom Colors
	style.Colors[ImGuiCol_WindowBg] = ImAdd::HexToColorVec4(0x171717, 1.00f);
	style.Colors[ImGuiCol_ChildBg] = ImAdd::HexToColorVec4(0x181818, 1.00f);
	style.Colors[ImGuiCol_PopupBg] = ImAdd::HexToColorVec4(0x171717, 1.00f);

	style.Colors[ImGuiCol_CheckMark] = ImAdd::HexToColorVec4(0xffffff, 1.00f);
	style.Colors[ImGuiCol_Text] = ImAdd::HexToColorVec4(0xcfcfcf, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImAdd::HexToColorVec4(0xcfcfcf, 1.00f);

	style.Colors[ImGuiCol_SliderGrab] = ImAdd::HexToColorVec4(0x5191f3, 1.00f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImAdd::HexToColorVec4(0x4376c3, 1.00f);

	style.Colors[ImGuiCol_SeparatorHovered] = style.Colors[ImGuiCol_SliderGrab];
	style.Colors[ImGuiCol_SeparatorActive] = style.Colors[ImGuiCol_SliderGrabActive];

	style.Colors[ImGuiCol_BorderShadow] = ImVec4();
	style.Colors[ImGuiCol_Border] = ImAdd::HexToColorVec4(0x212121, 1.00f);
	style.Colors[ImGuiCol_Separator] = style.Colors[ImGuiCol_Border];

	style.Colors[ImGuiCol_Button] = ImAdd::HexToColorVec4(0x262626, 1.00f);
	style.Colors[ImGuiCol_ButtonHovered] = ImAdd::HexToColorVec4(0x303030, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImAdd::HexToColorVec4(0x3d3d3d, 1.00f);

	style.Colors[ImGuiCol_FrameBg] = ImAdd::HexToColorVec4(0x212121, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImAdd::HexToColorVec4(0x262626, 1.00f);
	style.Colors[ImGuiCol_FrameBgActive] = ImAdd::HexToColorVec4(0x303030, 1.00f);

	style.Colors[ImGuiCol_Header] = style.Colors[ImGuiCol_Button];
	style.Colors[ImGuiCol_HeaderHovered] = style.Colors[ImGuiCol_ButtonHovered];
	style.Colors[ImGuiCol_HeaderActive] = style.Colors[ImGuiCol_ButtonActive];

	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4();
	style.Colors[ImGuiCol_ScrollbarGrab] = ImAdd::HexToColorVec4(0x262626, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = style.Colors[ImGuiCol_ScrollbarGrab];
	style.Colors[ImGuiCol_ScrollbarGrabActive] = style.Colors[ImGuiCol_ScrollbarGrab];

	style.Colors[ImGuiCol_ResizeGrip] = ImVec4();
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4();
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4();
	style.Colors[ImGuiCol_SeparatorActive] = ImVec4();
	style.Colors[ImGuiCol_SeparatorHovered] = ImVec4();
}

void MENU::UpdateStyle(ImGuiStyle* pStyle)
{
	ImGuiStyle& style = pStyle != nullptr ? *pStyle : ImGui::GetStyle();

	ImGui::StyleColorsDark();

	// Custom Styles
	style.WindowRounding = 10;
	style.ChildRounding = 6;
	style.FrameRounding = 4;
	style.TabRounding = 4;
	style.ScrollbarRounding = 2;

	style.ButtonTextAlign = { 0.5f, 0.5f };
	style.WindowTitleAlign = { 0.5f, 0.5f };
	style.FramePadding = { 10.0f, 10.0f };
	style.WindowPadding = { 22.0f, 22.0f };
	style.ItemSpacing = { 10.0f, 30.0f };
	style.ItemInnerSpacing = { 8.0f, 8.0f };
	style.CellPadding = { 4.0f, 3.0f };

	style.WindowBorderSize = 1;
	style.FrameBorderSize = 0;
	style.PopupBorderSize = 1;

	style.ScrollbarSize = 12;
	style.GrabMinSize = 6;

	style.PopupRounding = 4;

	// Custom Colors
	style.Colors[ImGuiCol_WindowBg] = ImColor(246, 246, 246); // ImAdd::HexToColorVec4(0xf6f6f6, 1.00f);
	style.Colors[ImGuiCol_ChildBg] = ImColor(242, 242, 242); // ImAdd::HexToColorVec4(0xf2f2f2, 1.00f);
	style.Colors[ImGuiCol_PopupBg] = ImColor(233, 233, 233); // ImAdd::HexToColorVec4(0xe9e9e9, 1.00f);

	style.Colors[ImGuiCol_CheckMark] = ImColor(255, 255, 255); //  ImAdd::HexToColorVec4(0xffffff, 1.00f);
	style.Colors[ImGuiCol_Text] = ImColor(44, 44, 44); // ImAdd::HexToColorVec4(0x2c2c2b, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImColor(165, 162, 160); // ImAdd::HexToColorVec4(0xa5a2a0, 1.00f);

	style.Colors[ImGuiCol_SliderGrab] = ImColor(81, 145, 243); // ImAdd::HexToColorVec4(0x5191f3, 1.00f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImColor(67, 118, 195); // ImAdd::HexToColorVec4(0x4376c3, 1.00f);

	style.Colors[ImGuiCol_SeparatorHovered] = style.Colors[ImGuiCol_SliderGrab];
	style.Colors[ImGuiCol_SeparatorActive] = style.Colors[ImGuiCol_SliderGrabActive];

	style.Colors[ImGuiCol_BorderShadow] = ImVec4();
	style.Colors[ImGuiCol_Border] = ImColor(231, 231, 231); // ImAdd::HexToColorVec4(0xe7e7e7, 1.00f);
	style.Colors[ImGuiCol_Separator] = style.Colors[ImGuiCol_Border];

	style.Colors[ImGuiCol_Button] = ImColor(230, 230, 230); //ImAdd::HexToColorVec4(0xe6e6e6, 1.00f);
	style.Colors[ImGuiCol_ButtonHovered] = ImColor(233, 233, 233); // ImAdd::HexToColorVec4(0xe9e9e9, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImColor(227, 227, 227);//ImAdd::HexToColorVec4(0xe3e3e3, 1.00f);

	style.Colors[ImGuiCol_FrameBg] = ImColor(220, 220, 220);//ImAdd::HexToColorVec4(0xdcdcdc, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImColor(217, 217, 217);//ImAdd::HexToColorVec4(0xd9d9d9, 1.00f);
	style.Colors[ImGuiCol_FrameBgActive] = ImColor(214, 214, 214);//::HexToColorVec4(0xd6d6d6, 1.00f);

	style.Colors[ImGuiCol_Header] = style.Colors[ImGuiCol_Button];
	style.Colors[ImGuiCol_HeaderHovered] = style.Colors[ImGuiCol_ButtonHovered];
	style.Colors[ImGuiCol_HeaderActive] = style.Colors[ImGuiCol_ButtonActive];

	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4();
	style.Colors[ImGuiCol_ScrollbarGrab] = ImColor(230, 230, 230); //ImAdd::HexToColorVec4(0xe6e6e6, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = style.Colors[ImGuiCol_ScrollbarGrab];
	style.Colors[ImGuiCol_ScrollbarGrabActive] = style.Colors[ImGuiCol_ScrollbarGrab];

	style.Colors[ImGuiCol_ResizeGrip] = ImVec4();
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4();
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4();
	style.Colors[ImGuiCol_SeparatorActive] = ImVec4();
	style.Colors[ImGuiCol_SeparatorHovered] = ImVec4();
}

#pragma region menu_tabs

void T::Render(const char* szTabBar, const CTab* arrTabs, const unsigned long long nTabsCount, int* nCurrentTab, ImGuiTabBarFlags flags)
{
	if (ImGui::BeginTabBar(szTabBar, flags))
	{
		for (std::size_t i = 0U; i < nTabsCount; i++)
		{
			// add tab
			if (ImGui::BeginTabItem(arrTabs[i].szName))
			{
				// set current tab index
				*nCurrentTab = (int)i;
				ImGui::EndTabItem();
			}
		}

		// render inner tab
		if (arrTabs[*nCurrentTab].pRenderFunction != nullptr)
			arrTabs[*nCurrentTab].pRenderFunction();

		ImGui::EndTabBar();
	}
}

void T::RageBot()
{ }

void T::LegitBot()
{
	ImGui::BeginChild(CS_XOR("legitbot.aimbot"), ImVec2{}, true, ImGuiWindowFlags_MenuBar);
	{
		if (ImGui::BeginMenuBar())
		{
			ImGui::TextUnformatted(CS_XOR("aimbot"));
			ImGui::EndMenuBar();
		}

		ImGui::Checkbox(CS_XOR("enable##aimbot"), &C_GET(bool, Vars.bLegitbot));
		ImGui::SliderFloat(CS_XOR("fov"), &C_GET(float, Vars.flAimRange), 0.f, 135.f);
		ImGui::SliderFloat(CS_XOR("smoothing"), &C_GET(float, Vars.flSmoothing), 1.f, 100.f);

		ImGui::NewLine();
		// Key
		ImGui::Checkbox(CS_XOR("always on##aimbot"), &C_GET(bool, Vars.bLegitbotAlwaysOn));
		ImGui::BeginDisabled(C_GET(bool, Vars.bLegitbotAlwaysOn));
		{
			ImGui::HotKey(CS_XOR("toggle key"), &C_GET(unsigned int, Vars.nLegitbotActivationKey));
		}
		ImGui::EndDisabled();
	}
	ImGui::EndChild();
}

void T::Visuals()
{
	ImGuiStyle& style = ImGui::GetStyle();

	MENU::RenderOverlayPreviewWindow();

	ImGui::Columns(2, CS_XOR("##visuals_collumns"), false);
	{
		static float flOverlayChildSize = 0.f;
		ImGui::BeginChild(CS_XOR("visuals.overlay"), ImVec2(0.f, flOverlayChildSize), true, ImGuiWindowFlags_MenuBar);
		{
			if (ImGui::BeginMenuBar())
			{
				ImGui::TextUnformatted("overlay");
				ImGui::EndMenuBar();
			}

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, 0));

			ImGui::Checkbox(CS_XOR("enable##overlay"), &C_GET(bool, Vars.bVisualOverlay));

			ImGui::BeginDisabled(!C_GET(bool, Vars.bVisualOverlay));
			{
				ImGui::Checkbox(CS_XOR("bounding box"), &C_GET(FrameOverlayVar_t, Vars.overlayBox).bEnable);
				ImGui::Checkbox(CS_XOR("name"), &C_GET(TextOverlayVar_t, Vars.overlayName).bEnable);
				ImGui::Checkbox(CS_XOR("health bar"), &C_GET(BarOverlayVar_t, Vars.overlayHealthBar).bEnable);
				ImGui::Checkbox(CS_XOR("armor bar"), &C_GET(BarOverlayVar_t, Vars.overlayArmorBar).bEnable);
			}
			ImGui::EndDisabled();

			ImGui::PopStyleVar();

			flOverlayChildSize = ImGui::GetCursorPosY() + style.ItemSpacing.y;
		}
		ImGui::EndChild();

		ImGui::BeginChild(CS_XOR("visuals.chams"), ImVec2{}, true, ImGuiWindowFlags_MenuBar);
		{
			if (ImGui::BeginMenuBar())
			{
				ImGui::TextUnformatted("chams");
				ImGui::EndMenuBar();
			}

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, 0));

			ImGui::Checkbox(CS_XOR("enable##chams"), &C_GET(bool, Vars.bVisualChams));
			ImGui::Combo(CS_XOR("materials"), &C_GET(int, Vars.nVisualChamMaterial), CS_XOR("illuminate\0flat\0"));
			ImGui::Checkbox(CS_XOR("enable invisible chams##chams"), &C_GET(bool, Vars.bVisualChamsIgnoreZ));

			ImGui::ColorEdit4(CS_XOR("visible color"), &C_GET(Color_t, Vars.colVisualChams));
			if (C_GET(bool, Vars.bVisualChamsIgnoreZ))
				ImGui::ColorEdit4(CS_XOR("invisible color"), &C_GET(Color_t, Vars.colVisualChamsIgnoreZ));

			ImGui::PopStyleVar();
		}
		ImGui::EndChild();
	}
	ImGui::NextColumn();
	{
	}
	ImGui::Columns(1);
}

void T::Miscellaneous()
{
	ImGuiStyle& style = ImGui::GetStyle();

	ImGui::Columns(2, CS_XOR("##misc_collumns"), false);
	{
		ImGui::BeginChild(CS_XOR("misc.general"), ImVec2(0, ImGui::GetContentRegionAvail().y / 2.f), true, ImGuiWindowFlags_MenuBar);
		{
			if (ImGui::BeginMenuBar())
			{
				ImGui::TextUnformatted(CS_XOR("general"));
				ImGui::EndMenuBar();
			}

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, 0));

			ImGui::Checkbox(CS_XOR("watermark"), &C_GET(bool, Vars.bWatermark));

			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 0.f, 1.f));
			if (ImGui::Button(CS_XOR("unlock hidden cvars"), ImVec2(-1, 15 * MENU::flDpiScale)))
			{
				I::Cvar->UnlockHiddenCVars();
				NOTIFY::Push({ N_TYPE_INFO, CS_XOR("unlocked all hidden cvars") });
			}

			ImGui::PopStyleColor();

			ImGui::PopStyleVar();
		}
		ImGui::EndChild();

		ImGui::BeginChild(CS_XOR("misc.movement"), ImVec2{}, true, ImGuiWindowFlags_MenuBar);
		{
			if (ImGui::BeginMenuBar())
			{
				ImGui::TextUnformatted(CS_XOR("movement"));
				ImGui::EndMenuBar();
			}

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, 0));

			ImGui::Checkbox(CS_XOR("auto bunny-hopping"), &C_GET(bool, Vars.bAutoBHop));
			if (C_GET(bool, Vars.bAutoBHop))
				ImGui::SliderInt(CS_XOR("chance"), &C_GET(int, Vars.nAutoBHopChance), 0, 100, CS_XOR("%d%%"));

			ImGui::Checkbox(CS_XOR("auto strafe"), &C_GET(bool, Vars.bAutoStrafe));

			ImGui::PopStyleVar();
		}
		ImGui::EndChild();
	}
	ImGui::NextColumn();
	{
		static int nCurrentSettingSubTab = 0;
		ImGui::BeginChild(CS_XOR("misc.settings"), ImVec2(0, ImGui::GetContentRegionAvail().y / 2.f), true, ImGuiWindowFlags_MenuBar);
		{
			if (ImGui::BeginMenuBar())
			{
				ImGui::TextUnformatted(CS_XOR("settings"));

				ImGui::EndMenuBar();
			}

			static const CTab arrSettingTabs[] = {
				CTab{ "configuration", []()
				{
					ImGui::Columns(2, CS_XOR("#CONFIG"), false);
					{
						ImGui::PushItemWidth(-1);

						// check selected configuration for magic value
						if (nSelectedConfig == ~1U)
						{
							// set default configuration as selected on first use
							for (std::size_t i = 0U; i < C::vecFileNames.size(); i++)
							{
								if (CRT::StringCompare(C::vecFileNames[i], CS_XOR(CS_CONFIGURATION_DEFAULT_FILE_NAME CS_CONFIGURATION_FILE_EXTENSION)) == 0)
									nSelectedConfig = i;
							}
						}

						if (ImGui::BeginListBox(CS_XOR("##config.list"), C::vecFileNames.size(), 5))
						{
							for (std::size_t i = 0U; i < C::vecFileNames.size(); i++)
							{
								// @todo: imgui cant work with wstring
								const wchar_t* wszFileName = C::vecFileNames[i];

								char szFileName[MAX_PATH] = {};
								CRT::StringUnicodeToMultiByte(szFileName, CS_ARRAYSIZE(szFileName), wszFileName);

								if (ImGui::Selectable(szFileName, (nSelectedConfig == i)))
									nSelectedConfig = i;
							}

							ImGui::EndListBox();
						}

						ImGui::PopItemWidth();
					}
					ImGui::NextColumn();
					{
						ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(ImGui::GetStyle().FramePadding.x, 0));
						ImGui::PushItemWidth(-1);
						if (ImGui::InputTextWithHint(CS_XOR("##config.file"), CS_XOR("create new..."), szConfigFile, sizeof(szConfigFile), ImGuiInputTextFlags_EnterReturnsTrue))
						{
							// check if the filename isn't empty
							if (const std::size_t nConfigFileLength = CRT::StringLength(szConfigFile); nConfigFileLength > 0U)
							{
								CRT::WString_t wszConfigFile(szConfigFile);

								if (C::CreateFile(wszConfigFile.Data()))
									// set created config as selected @todo: dependent on current 'C::CreateFile' behaviour, generally it must be replaced by search
									nSelectedConfig = C::vecFileNames.size() - 1U;

								// clear string
								CRT::MemorySet(szConfigFile, 0U, sizeof(szConfigFile));
							}
						}
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(CS_XOR("press enter to create new configuration"));

						if (ImGui::Button(CS_XOR("save"), ImVec2(-1, 15 * MENU::flDpiScale)))
							C::SaveFile(nSelectedConfig);

						if (ImGui::Button(CS_XOR("load"), ImVec2(-1, 15 * MENU::flDpiScale)))
							C::LoadFile(nSelectedConfig);

						if (ImGui::Button(CS_XOR("remove"), ImVec2(-1, 15 * MENU::flDpiScale)))
							ImGui::OpenPopup(CS_XOR("confirmation##config.remove"));

						if (ImGui::Button(CS_XOR("refresh"), ImVec2(-1, 15 * MENU::flDpiScale)))
							C::Refresh();

						ImGui::PopItemWidth();
						ImGui::PopStyleVar();
					}
					ImGui::Columns(1);

					if (ImGui::BeginPopupModal(CS_XOR("confirmation##config.remove"), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
					{
						CRT::String_t<MAX_PATH> szCurrentConfig(C::vecFileNames[nSelectedConfig]);

						ImGui::Text(CS_XOR("are you sure you want to remove \"%s\" configuration?"), szCurrentConfig);
						ImGui::Spacing();

						if (ImGui::Button(CS_XOR("no"), ImVec2(ImGui::GetContentRegionAvail().x / 2.f, 0)))
							ImGui::CloseCurrentPopup();

						ImGui::SameLine();

						if (ImGui::Button(CS_XOR("yes"), ImVec2(ImGui::GetContentRegionAvail().x, 0)))
						{
							C::RemoveFile(nSelectedConfig);

							// reset selected configuration index
							nSelectedConfig = ~0U;

							ImGui::CloseCurrentPopup();
						}

						ImGui::EndPopup();
					}
				} },
				CTab{ "menu", []()
				{
					static int nSelectedColor = 0;

					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(ImGui::GetStyle().FramePadding.x, 0));

					ImGui::HotKey(CS_XOR("menu key"), &C_GET(unsigned int, Vars.nMenuKey));

					if (ImGui::BeginCombo(CS_XOR("dpi scale"), arrMiscDpiScale[C_GET(int, Vars.nDpiScale)]))
					{
						for (int i = 0; i < IM_ARRAYSIZE(arrMiscDpiScale); i++)
						{
							if (ImGui::Selectable(arrMiscDpiScale[i], (C_GET(int, Vars.nDpiScale) == i)))
								C_GET(int, Vars.nDpiScale) = i;
						}

						ImGui::EndCombo();
					}

					ImGui::MultiCombo(CS_XOR("additional settings"), &C_GET(unsigned int, Vars.bMenuAdditional), arrMenuAddition, CS_ARRAYSIZE(arrMenuAddition));

					ImGui::SliderFloat(CS_XOR("animation speed"), &C_GET(float, Vars.flAnimationSpeed), 1.f, 10.f);

					ImGui::SeparatorText(CS_XOR("colors pallete"));

					ImGui::PushItemWidth(-1);

					if (ImGui::BeginListBox(CS_XOR("##themes.select"), CS_ARRAYSIZE(arrColors), 5))
					{
						for (std::size_t i = 0U; i < CS_ARRAYSIZE(arrColors); i++)
						{
							const char* szColorName = arrColors[i].first;

							if (ImGui::Selectable(szColorName, (i == nSelectedColor)))
								nSelectedColor = (int)i;
						}

						ImGui::EndListBox();
					}

					ImGui::ColorEdit4(CS_XOR("##themes.picker"), &C_GET(ColorPickerVar_t, arrColors[nSelectedColor].second), ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_DisplayRGB);
					ImGui::PopItemWidth();
					ImGui::PopStyleVar();
				} }
			};

			Render(CS_XOR("##misc.setttings.tab"), arrSettingTabs, CS_ARRAYSIZE(arrSettingTabs), &nCurrentSettingSubTab);
		}
		ImGui::EndChild();
	}
	ImGui::Columns(1);
}

void T::SkinsChanger()
{
}

#pragma endregion
