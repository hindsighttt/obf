// used: [stl] vector
#include <vector>
// used: [stl] sort
#include <algorithm>

#include "overlay.h"

// used: cheat variables
#include "../../core/variables.h"
#include "../../sound.h"

// used: entity
#include "../../sdk/entity.h"
#include "../../sdk/interfaces/cgameentitysystem.h"
#include "../../sdk/interfaces/iengineclient.h"
#include "../../sdk/interfaces/cgametracemanager.h"
#include "../../sdk/interfaces/inetworkclientservice.h"
#include "../../sdk/interfaces/ccsgoinput.h"
#include "../../sdk/interfaces/ienginecvar.h"

// used: sdk variables
#include "../../core/sdk.h"

// used: l_print
#include "../../utilities/log.h"
// used: inputsystem
#include "../../utilities/inputsystem.h"
// used: draw system
#include "../../utilities/draw.h"
#include "../../wall.h"

// used: mainwindowopened
#include "../../core/menu.h"
#include "../../utilities/math.h"



using namespace F::VISUALS;

#pragma region visual_overlay_components

ImVec2 OVERLAY::CBaseComponent::GetBasePosition(const ImVec4& box) const
{
	/*
	return { box[this->nSide == SIDE_RIGHT ? SIDE_RIGHT : SIDE_LEFT], box[this->nSide == SIDE_BOTTOM ? SIDE_BOTTOM : SIDE_TOP] };
	*/
	return (ImVec2());
}

ImVec2 OVERLAY::CBaseDirectionalComponent::GetBasePosition(const ImVec4& box) const
{
	ImVec2 vecBasePosition = {};
	/*
	if (this->nSide == SIDE_TOP || this->nSide == SIDE_BOTTOM)
	{
		CS_ASSERT(this->nDirection != (this->nSide ^ SIDE_BOTTOM) + 1); // this direction isn't supported for this side
		vecBasePosition = { (box.x + box.z) * 0.5f, box[this->nSide] };
	}
	else if (this->nSide == SIDE_LEFT || this->nSide == SIDE_RIGHT)
	{
		CS_ASSERT(this->nDirection != (this->nSide ^ SIDE_RIGHT)); // this direction isn't supported for this side
		vecBasePosition = { box[this->nSide], box[this->nDirection == DIR_TOP ? SIDE_BOTTOM : SIDE_TOP] };
	}
	else
	{
		L_PRINT(LOG_ERROR) << CS_XOR("CBaseDirectionalComponent::GetBasePosition: invalid side: ") << this->nSide;
		CS_ASSERT(false); // this side isn't supported for this component
		return vecBasePosition;
	}

	if (this->nSide != SIDE_RIGHT && this->nDirection != DIR_RIGHT)
		vecBasePosition.x -= this->vecSize.x * ((static_cast<std::uint8_t>(this->nDirection) == static_cast<std::uint8_t>(this->nSide) && (this->nSide & 1U) == 1U) ? 0.5f : 1.0f);

	if (this->nSide == SIDE_TOP || this->nDirection == DIR_TOP)
		vecBasePosition.y -= this->vecSize.y;
	*/
	return vecBasePosition;
}

OVERLAY::CBarComponent::CBarComponent(const bool bIsMenuItem, const EAlignSide nAlignSide, const ImVec4& vecBox, const float flProgressFactor, const std::size_t uOverlayVarIndex) :
	bIsMenuItem(bIsMenuItem), uOverlayVarIndex(uOverlayVarIndex), flProgressFactor(MATH::Clamp(flProgressFactor, 0.f, 1.f))
{
	this->nSide = nAlignSide;

	const bool bIsHorizontal = ((nAlignSide & 1U) == 1U);

	const BarOverlayVar_t& overlayConfig = C_GET(BarOverlayVar_t, uOverlayVarIndex);
	this->vecSize = { (bIsHorizontal ? vecBox.z - vecBox.x : overlayConfig.flThickness), (bIsHorizontal ? overlayConfig.flThickness : vecBox.w - vecBox.y) };
}

void OVERLAY::CBarComponent::Render(ImDrawList* pDrawList, const ImVec2& vecPosition)
{
	BarOverlayVar_t& overlayConfig = C_GET(BarOverlayVar_t, uOverlayVarIndex);
	const ImVec2 vecThicknessOffset = { overlayConfig.flThickness, overlayConfig.flThickness };
	ImVec2 vecMin = vecPosition, vecMax = vecPosition + this->vecSize;
	float xDelta = vecMax.x - vecMin.x;
	float yDelta = vecMax.y - vecMin.y;
	float barWidth = 2.0f;
	if (yDelta > xDelta)
		vecMin.x -= barWidth;
	else
		vecMin.y -= barWidth;

	// background glow
	//pDrawList->AddShadowRect(vecMin, vecMax, overlayConfig.colBackground.GetU32(), 1.f, ImVec2(0, 0));
	// outline
	pDrawList->AddRect(ImVec2(vecMin.x - 1, vecMin.y - 1), ImVec2(vecMax.x + 1, vecMax.y + 1), overlayConfig.colOutline.GetU32(), 0.f, ImDrawFlags_None, 1);

	// account outline offset
	vecMin += vecThicknessOffset;
	vecMax -= vecThicknessOffset;

	const ImVec2 vecLineSize = vecMax - vecMin;

	// modify active side axis by factor
	if ((this->nSide & 1U) == 0U)
		vecMin.y += vecLineSize.y * (1.0f - this->flProgressFactor);
	else
		vecMax.x -= vecLineSize.x * (1.0f - this->flProgressFactor);

	// bar
	if (overlayConfig.bGradient && !overlayConfig.bUseFactorColor)
	{
		if (this->nSide == SIDE_LEFT || this->nSide == SIDE_RIGHT)
			pDrawList->AddRectFilledMultiColor(vecMin, vecMax, overlayConfig.colPrimary.GetU32(), overlayConfig.colPrimary.GetU32(), overlayConfig.colSecondary.GetU32(), overlayConfig.colSecondary.GetU32());
		else
			pDrawList->AddRectFilledMultiColor(vecMin, vecMax, overlayConfig.colSecondary.GetU32(), overlayConfig.colPrimary.GetU32(), overlayConfig.colPrimary.GetU32(), overlayConfig.colSecondary.GetU32());
	}
	else
	{
		const ImU32 u32Color = overlayConfig.bUseFactorColor ? Color_t::FromHSB((flProgressFactor * 120.f) / 360.f, 1.0f, 1.0f).GetU32() : overlayConfig.colPrimary.GetU32();
		pDrawList->AddRectFilled(vecMin, vecMax, u32Color, 0.f, ImDrawFlags_None);
	}

	// only open menu item if menu is opened and overlay is enabled
	bIsMenuItem &= (MENU::bMainWindowOpened && overlayConfig.bEnable);
	if (bIsMenuItem)
	{
		// @note: padding 2.f incase the thickness is too small
		this->bIsHovered = ImRect(vecPosition - ImVec2(2.f, 2.f), vecPosition + this->vecSize + ImVec2(2.f, 2.f)).Contains(ImGui::GetIO().MousePos);
		// if component is hovered + right clicked
		if (this->bIsHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
			ImGui::OpenPopup(CS_XOR("context##component.bar"));
		
		if (ImGui::BeginPopup(CS_XOR("context##component.bar"), ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(ImGui::GetStyle().FramePadding.x, -1));

			ImGui::Checkbox(CS_XOR("use factor color##component.bar"), &overlayConfig.bUseFactorColor);
			if (!overlayConfig.bUseFactorColor)
			ImGui::Checkbox(CS_XOR("use gradient##component.bar"), &overlayConfig.bGradient);

			ImGui::ColorEdit3(CS_XOR("primary color##component.bar"), &overlayConfig.colPrimary);
			if (overlayConfig.bGradient && !overlayConfig.bUseFactorColor)
				ImGui::ColorEdit3(CS_XOR("secondary color##component.bar"), &overlayConfig.colSecondary);

			ImGui::ColorEdit4(CS_XOR("outline color##component.bar"), &overlayConfig.colOutline);
			ImGui::ColorEdit4(CS_XOR("background color##component.bar"), &overlayConfig.colBackground);

			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.75f);
			ImGui::SliderFloat(CS_XOR("thickness##component.bar"), &overlayConfig.flThickness, 1.0f, 10.0f, CS_XOR("%.1f"), ImGuiSliderFlags_NoInput);
			ImGui::PopStyleVar();

			ImGui::EndPopup();
		}
	}
	else
		// dont process hovered on menu close...
		this->bIsHovered = false;
}

OVERLAY::CTextComponent::CTextComponent(const bool bIsMenuItem, const EAlignSide nAlignSide, const EAlignDirection nAlignDirection, const ImFont* pFont, const char* szText, const std::size_t uOverlayVarIndex) :
	bIsMenuItem(bIsMenuItem), pFont(pFont), uOverlayVarIndex(uOverlayVarIndex)
{
	// allocate own buffer to safely store a copy of the string
	this->szText = new char[CRT::StringLength(szText) + 1U];
	CRT::StringCopy(this->szText, szText);

	this->nSide = nAlignSide;
	this->nDirection = nAlignDirection;
	this->vecSize = pFont->CalcTextSizeA(pFont->FontSize, FLT_MAX, 0.0f, szText);
}

OVERLAY::CTextComponent::~CTextComponent()
{
	// deallocate buffer of the copied string
	delete[] this->szText;
}

void OVERLAY::CTextComponent::Render(ImDrawList* pDrawList, const ImVec2& vecPosition)
{
	TextOverlayVar_t& overlayConfig = C_GET(TextOverlayVar_t, this->uOverlayVarIndex);

	const ImVec2 vecOutlineOffset = { overlayConfig.flThickness, overlayConfig.flThickness };

	// @test: used for spacing debugging
	//pDrawList->AddRect(vecPosition, vecPosition + this->vecSize, IM_COL32(255, 255, 255, 255));

	// @todo: fix this cringe shit after gui merge
	if (overlayConfig.flThickness >= 1.0f)
	{
		//pDrawList->AddText(this->pFont, this->pFont->FontSize, vecPosition + vecOutlineOffset * 2.0f, overlayConfig.colOutline.GetU32(), this->szText);
		//pDrawList->AddText(this->pFont, this->pFont->FontSize, vecPosition, overlayConfig.colOutline.GetU32(), this->szText);
		pDrawList->AddText(this->pFont, this->pFont->FontSize, ImVec2(vecPosition.x, vecPosition.y - 1.0f), ImColor(0, 0, 0), this->szText);
		//pDrawList->AddText(this->pFont, this->pFont->FontSize, ImVec2(vecPosition.x, vecPosition.y), ImColor(255, 255, 255), this->szText);
	}

	//pDrawList->AddText(this->pFont, this->pFont->FontSize, vecPosition + vecOutlineOffset, overlayConfig.colPrimary.GetU32(), this->szText);
	pDrawList->AddText(this->pFont, this->pFont->FontSize, vecPosition, ImColor(255, 255, 255), this->szText);
	// only open menu item if menu is opened and overlay is enabled
	bIsMenuItem &= MENU::bMainWindowOpened && overlayConfig.bEnable; 
	if (bIsMenuItem)
	{
		this->bIsHovered = ImRect(vecPosition, vecPosition + this->vecSize).Contains(ImGui::GetIO().MousePos);
		//pDrawList->AddRect(vecPosition, vecPosition + this->vecSize, IM_COL32(this->bIsHovered ? 0 : 255, this->bIsHovered ? 255 : 0, 0, 255));

		// if component is hovered + right clicked
		if (this->bIsHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
			ImGui::OpenPopup(CS_XOR("context##component.text"));

		if (ImGui::BeginPopup(CS_XOR("context##component.text")))
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(ImGui::GetStyle().FramePadding.x, -1));

			ImGui::ColorEdit3(CS_XOR("primary color##component.bar"), &overlayConfig.colPrimary);
			ImGui::ColorEdit4(CS_XOR("outline color##component.bar"), &overlayConfig.colOutline);

			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.75f);
			ImGui::SliderFloat(CS_XOR("outline thickness##component.bar"), &overlayConfig.flThickness, 1.0f, 10.0f, CS_XOR("%.1f"), ImGuiSliderFlags_NoInput);

			ImGui::PopStyleVar();

			ImGui::EndPopup();
		}
	}
}

#pragma endregion

#pragma region visual_overlay_context

bool OVERLAY::Context_t::AddBoxComponent(ImDrawList* pDrawList, const ImVec4& vecBox, const int nType, float flThickness, float flRounding, const Color_t& colPrimary, const Color_t& colOutline)
{
	flThickness = std::floorf(flThickness);
	const ImVec2 vecThicknessOffset = { flThickness, flThickness };

	switch (nType)
	{
	case VISUAL_OVERLAY_BOX_FULL:
	{
		const ImVec2 vecBoxMin = { vecBox.x, vecBox.y };
		const ImVec2 vecBoxMax = { vecBox.z, vecBox.w };

		// inner outline
		pDrawList->AddRect(vecBoxMin + vecThicknessOffset * 2.0f, vecBoxMax - vecThicknessOffset * 2.0f, colOutline.GetU32(), flRounding, ImDrawFlags_RoundCornersAll, flThickness);
		// primary box
		pDrawList->AddRect(vecBoxMin + vecThicknessOffset, vecBoxMax - vecThicknessOffset, colPrimary.GetU32(), flRounding, ImDrawFlags_RoundCornersAll, flThickness);
		// outer outline
		pDrawList->AddRect(vecBoxMin, vecBoxMax, colOutline.GetU32(), flRounding, ImDrawFlags_RoundCornersAll, flThickness);

		break;
	}
	case VISUAL_OVERLAY_BOX_CORNERS:
	{
		// corner part of the whole line
		constexpr float flPartRatio = 0.25f;

		const float flCornerWidth = ((vecBox.z - vecBox.x) * flPartRatio);
		const float flCornerHeight = ((vecBox.w - vecBox.y) * flPartRatio);

		const ImVec2 arrCornerPoints[4][3] = {
			// top-left
			{ ImVec2(vecBox.x, vecBox.y + flCornerHeight) + vecThicknessOffset, ImVec2(vecBox.x, vecBox.y) + vecThicknessOffset, ImVec2(vecBox.x + flCornerWidth, vecBox.y) + vecThicknessOffset },

			// top-right
			{ ImVec2(vecBox.z - flCornerWidth - vecThicknessOffset.x, vecBox.y + vecThicknessOffset.y * 2.0f), ImVec2(vecBox.z - vecThicknessOffset.x, vecBox.y + vecThicknessOffset.y * 2.0f), ImVec2(vecBox.z - vecThicknessOffset.x, vecBox.y + flCornerHeight + vecThicknessOffset.y * 2.0f) },

			// bottom-left
			{ ImVec2(vecBox.x + flCornerWidth + vecThicknessOffset.x, vecBox.w - vecThicknessOffset.y * 2.0f), ImVec2(vecBox.x + vecThicknessOffset.x, vecBox.w - vecThicknessOffset.y * 2.0f), ImVec2(vecBox.x + vecThicknessOffset.x, vecBox.w - flCornerHeight - vecThicknessOffset.y * 2.0f) },

			// bottom-right
			{ ImVec2(vecBox.z, vecBox.w - flCornerHeight) - vecThicknessOffset, ImVec2(vecBox.z, vecBox.w) - vecThicknessOffset, ImVec2(vecBox.z - flCornerWidth, vecBox.w) - vecThicknessOffset }
		};

		for (std::size_t i = 0U; i < CS_ARRAYSIZE(arrCornerPoints); i++)
		{
			const auto& arrLinePoints = arrCornerPoints[i];
			const ImVec2 vecHalfPixelOffset = ((i & 1U) == 1U ? ImVec2(-0.5f, -0.5f) : ImVec2(0.5f, 0.5f));

			// @todo: we can even do not clear path and reuse it
			pDrawList->PathLineTo(arrLinePoints[0] + vecHalfPixelOffset);
			pDrawList->PathLineTo(arrLinePoints[1] + vecHalfPixelOffset);
			pDrawList->PathLineTo(arrLinePoints[2] + vecHalfPixelOffset);
			pDrawList->PathStroke(colOutline.GetU32(), false, flThickness + 1.0f);

			pDrawList->PathLineTo(arrLinePoints[0] + vecHalfPixelOffset);
			pDrawList->PathLineTo(arrLinePoints[1] + vecHalfPixelOffset);
			pDrawList->PathLineTo(arrLinePoints[2] + vecHalfPixelOffset);
			pDrawList->PathStroke(colPrimary.GetU32(), false, flThickness);
		}

		break;
	}
	default:
		break;
	}

	// accumulate spacing for next side/directional components
	for (float& flSidePadding : this->arrSidePaddings)
		flSidePadding += this->flComponentSpacing;

	return ImRect(vecBox).Contains(ImGui::GetIO().MousePos);
}

ImVec4 OVERLAY::Context_t::AddFrameComponent(ImDrawList* pDrawList, const ImVec2& vecScreen, const EAlignSide nSide, const Color_t& colBackground, const float flRounding, const ImDrawFlags nRoundingCorners)
{
	// calculate frame size by previously added components on active side
	const ImVec2 vecFrameSize = this->GetTotalDirectionalSize(nSide);

	ImVec2 vecFrameMin = { vecScreen.x - vecFrameSize.x * 0.5f, vecScreen.y - vecFrameSize.y };
	ImVec2 vecFrameMax = { vecScreen.x + vecFrameSize.x * 0.5f, vecScreen.y };

	pDrawList->AddRectFilled(ImVec2(vecFrameMin.x - this->flComponentSpacing, vecFrameMin.y - this->flComponentSpacing), ImVec2(vecFrameMax.x + this->flComponentSpacing, vecFrameMax.y + this->flComponentSpacing), colBackground.GetU32(), flRounding, nRoundingCorners);

	// accumulate spacing for next side/directional components
	for (float& flSidePadding : this->arrSidePaddings)
		flSidePadding += this->flComponentSpacing;

	return { vecFrameMin.x, vecFrameMin.y, vecFrameMax.x, vecFrameMax.y };
}

/*
 * @todo: currently not well designed, make it more flexible for use cases where we need e.g. previous frame bar factor etc
 * also to optimize this, allocate components at stack instead of heap + make all context units static and do not realloc components storage every frame, but reset (like memset idk) it at the end of frame
 */
void OVERLAY::Context_t::AddComponent(CBaseComponent* pComponent)
{
	// guarantee that first directional component on each side is in the primary direction
	if (pComponent->IsDirectional())
	{
		CBaseDirectionalComponent* pDirectionalComponent = static_cast<CBaseDirectionalComponent*>(pComponent);

		// check if it's not an exception direction and there are no components in the primary direction
		if (((pDirectionalComponent->nSide & 1U) == 1U || pDirectionalComponent->nDirection != DIR_TOP) && this->arrSideDirectionPaddings[pDirectionalComponent->nSide][pDirectionalComponent->nSide] == 0.0f)
			pDirectionalComponent->nDirection = static_cast<EAlignDirection>(pDirectionalComponent->nSide);
	}

	float& flSidePadding = this->arrSidePaddings[pComponent->nSide];

	if (pComponent->IsDirectional())
	{
		CBaseDirectionalComponent* pDirectionalComponent = static_cast<CBaseDirectionalComponent*>(pComponent);
		float(&arrDirectionPaddings)[DIR_MAX] = this->arrSideDirectionPaddings[pDirectionalComponent->nSide];

		// directional components don't change side paddings, but take them into account
		pComponent->vecOffset[pDirectionalComponent->nSide & 1U] += ((pDirectionalComponent->nSide < 2U) ? -flSidePadding : flSidePadding);

		// check if the component is in the same direction as the side and it's the first component in this direction
		if (static_cast<std::uint8_t>(pDirectionalComponent->nDirection) == static_cast<std::uint8_t>(pDirectionalComponent->nSide) && arrDirectionPaddings[pDirectionalComponent->nDirection] == 0.0f)
		{
			// accumulate paddings for sub-directions
			for (std::uint8_t nSubDirection = DIR_LEFT; nSubDirection < DIR_MAX; nSubDirection++)
			{
				/*
				 * exclude conflicting sub-directions
				 *
				 * SIDE_LEFT[0]: DIR_LEFT[0], DIR_BOTTOM[3] | ~2 & ~1
				 * SIDE_TOP[1]: DIR_LEFT[0], DIR_TOP[1], DIR_RIGHT[2] | ~3
				 * SIDE_RIGHT[2]: DIR_RIGHT[2], DIR_BOTTOM[3] | ~0 & ~1
				 * SIDE_BOTTOM[3]: DIR_LEFT[0], DIR_RIGHT[2], DIR_BOTTOM[3] | ~1
				 */
				if (nSubDirection == pDirectionalComponent->nSide || nSubDirection == ((pDirectionalComponent->nSide + 2U) & 3U) || (nSubDirection == DIR_TOP && (pDirectionalComponent->nSide & 1U) == 0U))
					continue;

				arrDirectionPaddings[nSubDirection] += (pDirectionalComponent->vecSize[nSubDirection == DIR_BOTTOM ? SIDE_TOP : SIDE_LEFT] * (((pDirectionalComponent->nSide & 1U) == 1U) ? 0.5f : 1.0f) + this->flComponentSpacing);
			}
		}

		float& flSideDirectionPadding = arrDirectionPaddings[pDirectionalComponent->nDirection];
		
		// append direction padding to offset
		pComponent->vecOffset[pDirectionalComponent->nDirection & 1U] += ((pDirectionalComponent->nDirection < 2U) ? -flSideDirectionPadding : flSideDirectionPadding);

		// accumulate direction padding for next component
		flSideDirectionPadding += pDirectionalComponent->vecSize[pDirectionalComponent->nDirection & 1U];

		// accumulate spacing for next directional components
		flSideDirectionPadding += this->flComponentSpacing;
	}
	else
	{
		// append side padding to offset
		pComponent->vecOffset[pComponent->nSide & 1U] += ((pComponent->nSide < 2U) ? -(flSidePadding + pComponent->vecSize[pComponent->nSide]) : flSidePadding);

		// accumulate side padding for next component
		flSidePadding += pComponent->vecSize[pComponent->nSide & 1U];

		// accumulate spacing for next components
		flSidePadding += this->flComponentSpacing;
	}

	this->vecComponents.push_back(pComponent);
}

ImVec2 OVERLAY::Context_t::GetTotalDirectionalSize(const EAlignSide nSide) const
{
	ImVec2 vecSideSize = {};

	// @todo: we should peek max of bottom + side or top directions at horizontal sides
	const float(&arrDirectionPaddings)[DIR_MAX] = this->arrSideDirectionPaddings[nSide];
	for (std::uint8_t nSubDirection = DIR_LEFT; nSubDirection < DIR_MAX; nSubDirection++)
		vecSideSize[nSubDirection & 1U] += arrDirectionPaddings[nSubDirection];

	return vecSideSize;
}

void OVERLAY::Context_t::Render(ImDrawList* pDrawList, const ImVec4& vecBox) const
{
	bool bCenteredFirstSideDirectional[SIDE_MAX] = {};

	for (CBaseComponent* const pComponent : this->vecComponents)
	{
		ImVec2 vecPosition = pComponent->GetBasePosition(vecBox);

		// check if the component is in the side that supports multi-component centering
		if (pComponent->nSide == SIDE_TOP || pComponent->nSide == SIDE_BOTTOM)
		{
			// check if the component is directional
			if (CBaseDirectionalComponent* const pDirectionalComponent = static_cast<CBaseDirectionalComponent*>(pComponent); pDirectionalComponent->IsDirectional())
			{
				const float(&arrDirectionPaddings)[DIR_MAX] = this->arrSideDirectionPaddings[pComponent->nSide];

				// check if the component has horizontal direction
				if (static_cast<std::uint8_t>(pDirectionalComponent->nDirection) != static_cast<std::uint8_t>(pDirectionalComponent->nSide))
					// add centering offset to the component's offset
					pDirectionalComponent->vecOffset.x += (arrDirectionPaddings[DIR_LEFT] - arrDirectionPaddings[DIR_RIGHT]) * 0.5f;
				// otherwise check if it's the first component in direction as side
				else if (!bCenteredFirstSideDirectional[pDirectionalComponent->nSide])
				{
					// add centering offset to the component's offset
					pDirectionalComponent->vecOffset.x += (arrDirectionPaddings[DIR_LEFT] - arrDirectionPaddings[DIR_RIGHT]) * 0.5f;

					bCenteredFirstSideDirectional[pDirectionalComponent->nSide] = true;
				}
			}
		}

		// add final component offset to the base position
		vecPosition += pComponent->vecOffset;

		pComponent->Render(pDrawList, vecPosition);
	}
}

void OVERLAY::AddText(const char* Text, ImVec2 Position, ImColor Color)
{
	D::pDrawListActive->AddText(FONT::pMenu[0], FONT::pMenu[0]->FontSize, ImVec2(Position.x, Position.y + 1), ImColor(0, 0, 0), Text);
	D::pDrawListActive->AddText(FONT::pMenu[0], FONT::pMenu[0]->FontSize, ImVec2(Position.x, Position.y), Color, Text);
}

void OVERLAY::AddTextFlag(const char* Text, ImVec2 Position, ImColor Color)
{
	D::pDrawListActive->AddText(FONT::pMenu[0], 10, ImVec2(Position.x, Position.y + 1), ImColor(0, 0, 0), Text);
	D::pDrawListActive->AddText(FONT::pMenu[0], 10, ImVec2(Position.x, Position.y), Color, Text);
}

#pragma endregion
		
void OVERLAY::OnFrameStageNotify(CCSPlayerController* pLocalController)
{
	// only render when in-game
	if (!I::Engine->IsConnected() || !I::Engine->IsInGame())
		return;

	ScopeOverlay();
	PenetrationCrosshair(pLocalController);
	HitMarker();
	FOVCircle();
	SpreadCircle(pLocalController);
	MinDammage();

	if (C_GET(bool, Vars.bGrenadeCamera))
	{
		auto grenade_cam = I::Cvar->Find(FNV1A::HashConst("sv_grenade_trajectory_prac_pipreview"));
		if (grenade_cam && grenade_cam->value.i1 != C_GET(bool, Vars.bGrenadeCamera))
			grenade_cam->value.i1 = C_GET(bool, Vars.bGrenadeCamera);
	}

	// PLAYER ESP
	if (!C_GET(bool, Vars.bVisualOverlay))
		return;

	enum ESortEntityType : int
	{
		SORT_ENTITY_NONE = -1,
		SORT_ENTITY_PLAYER = 0,
	};

	struct SortEntityObject_t
	{
		 SortEntityObject_t(C_BaseEntity* pEntity, CBaseHandle hEntity, float flDistance, ESortEntityType nSortType) :
			pEntity(pEntity), hEntity(hEntity), flDistance(flDistance), nSortType(nSortType) { }

		C_BaseEntity* pEntity;
		CBaseHandle hEntity;
		float flDistance;
		ESortEntityType nSortType;
	};

	const int nHighestIndex = 126;

	std::vector<SortEntityObject_t> vecSortedEntities = {};
	vecSortedEntities.reserve(nHighestIndex);

	// @note: 0 is resved for world entity 'CWorld'
	for (int nIndex = 1; nIndex <= nHighestIndex; nIndex++)
	{
		C_BaseEntity* pEntity = I::GameResourceService->pGameEntitySystem->Get(nIndex);
		if (pEntity == nullptr)
			continue;

		SchemaClassInfoData_t* pClassInfo = nullptr;
		pEntity->GetSchemaClassInfo(&pClassInfo);
		if (pClassInfo == nullptr)
			continue;

		const FNV1A_t uHashedName = FNV1A::Hash(pClassInfo->szName);

		ESortEntityType nEntityType = SORT_ENTITY_NONE;
		Vector_t vecOrigin = Vector_t();

		if (uHashedName == FNV1A::HashConst("CCSPlayerController"))
		{
			nEntityType = SORT_ENTITY_PLAYER;
			CCSPlayerController* pPlayer = reinterpret_cast<CCSPlayerController*>(pEntity);
			if (pPlayer == nullptr)
				continue;

			vecOrigin = pPlayer->GetPawnOrigin();
		}

		// only add sortable entities
		if (nEntityType != SORT_ENTITY_NONE)
			vecSortedEntities.emplace_back(pEntity, pEntity->GetRefEHandle(), SDK::CameraPosition.DistTo(vecOrigin), nEntityType);
	}

	// sort entities by distance to draw them from the farthest to the nearest
	std::ranges::sort(vecSortedEntities.begin(), vecSortedEntities.end(), std::ranges::greater{}, &SortEntityObject_t::flDistance);

	for (auto& [pEntity, hEntity, flDistance, nSortType] : vecSortedEntities)
	{
		// if the handle is invalid, skip this entity
		if (!hEntity.IsValid())
			continue;

		switch (nSortType)
		{
		case SORT_ENTITY_PLAYER:
		{
			CCSPlayerController* pPlayer = I::GameResourceService->pGameEntitySystem->Get<CCSPlayerController>(hEntity);
			if (pPlayer == nullptr)
				break;

			if (!pPlayer->IsPawnAlive())
				break;

			Player(pLocalController, pPlayer, flDistance);

			break;
		}
		default:
			break;
		}
	}
}

bool OVERLAY::GetEntityBoundingBox(C_CSPlayerPawn* pEntity, ImVec4* pVecOut)
{
	CCollisionProperty* pCollision = pEntity->GetCollision();
	if (pCollision == nullptr)
		return false;

	CGameSceneNode* pGameSceneNode = pEntity->GetGameSceneNode();
	if (pGameSceneNode == nullptr)
		return false;

	CTransform nodeToWorldTransform = pGameSceneNode->GetNodeToWorld();
	const Matrix3x4_t matTransform = nodeToWorldTransform.quatOrientation.ToMatrix(nodeToWorldTransform.vecPosition);

	const Vector_t vecMins = pCollision->GetMins();
	const Vector_t vecMaxs = pCollision->GetMaxs();

	pVecOut->x = pVecOut->y = std::numeric_limits<float>::max();
	pVecOut->z = pVecOut->w = -std::numeric_limits<float>::max();

	for (int i = 0; i < 8; ++i)
	{
		const Vector_t vecPoint{ 
			i & 1 ? vecMaxs.x : vecMins.x, 
			i & 2 ? vecMaxs.y : vecMins.y,
			i & 4 ? vecMaxs.z : vecMins.z 
		};
		ImVec2 vecScreen;
		if (!D::WorldToScreen(vecPoint.Transform(matTransform), &vecScreen))
			return false;

		pVecOut->x = MATH::Min(pVecOut->x, vecScreen.x);
		pVecOut->y = MATH::Min(pVecOut->y, vecScreen.y);
		pVecOut->z = MATH::Max(pVecOut->z, vecScreen.x);
		pVecOut->w = MATH::Max(pVecOut->w, vecScreen.y);
	}
	return true;
}

void OVERLAY::MinDammage()
{
	if (!I::Engine->IsConnected() || !I::Engine->IsInGame())
		return;
	if (!IPT::IsKeyDown(C_GET(unsigned int, Vars.nRageBotMinDamageKey)))
		return;
	ImVec2 DisplaySize = ImGui::GetIO().DisplaySize;
	std::string szMinDamage = std::to_string((int)(C_GET(float, Vars.flMinDmgOverride)));
	ImVec2 TextSize = ImGui::CalcTextSize(szMinDamage.c_str());
	OVERLAY::AddTextFlag(szMinDamage.c_str(), ImVec2(DisplaySize.x / 2 + 2, DisplaySize.y / 2 - TextSize.y / 2 - 2), ImColor(255, 255, 255));
}

void OVERLAY::HitMarker()  
{  
	static int nOldHitCount = 0;  
	static int hitMarkerStartTick = 0;  
	int hitmarkertime = 25;  
	ImVec2 DisplaySize = ImGui::GetIO().DisplaySize;  
	float size = 7;  
	float gap = 6;
	SOUND::Waveform waveform;
	float flDesiredVolume = 0.6f;
	auto cVolume = I::Cvar->Find(FNV1A::HashConst("volume"));
	if (cVolume)
		flDesiredVolume = cVolume->value.fl * 2;
	if (!I::Engine->IsConnected() || !I::Engine->IsInGame())
	{
		nOldHitCount = 0;
		return;
	}
	if (!C_GET(bool, Vars.bHitmarker) && !C_GET(bool, Vars.bHitsound))
		return;

	auto pLocal = SDK::LocalPawn;
	if (!pLocal)  
		return;  
	auto pLocalController = SDK::LocalController;
	if (!pLocalController || !pLocalController->IsPawnAlive())
		return;
	if (pLocal->GetHealth() <= 0)
		return;
	auto pBaseCmd = SDK::Cmd->csgoUserCmd.pBaseCmd;  
	if (!pBaseCmd)  
		return;
	auto pBulletServices = pLocal->GetBulletServices();  
	if (!pBulletServices)  
		return;
	int nHitCount = pBulletServices->GetTotalHitsOnServer();  
	if (nHitCount == 0)  
	{  
		nOldHitCount = 0;  
		return;  
	}
	if (nHitCount < 0)
		return;
	if (nHitCount != nOldHitCount)  
	{  
		hitMarkerStartTick = pBaseCmd->nClientTick;
		if (C_GET(bool, Vars.bHitsound))
			waveform.Play(flDesiredVolume);
	}  
	if (hitMarkerStartTick + hitmarkertime > pBaseCmd->nClientTick)  
	{  
		int elapsed = pBaseCmd->nClientTick - hitMarkerStartTick;  
		float fade = 1.0f - (float)elapsed / (float)hitmarkertime;  
		int alpha = static_cast<int>(fade * 255.0f);  
		alpha = std::clamp(alpha, 0, 255);  
		ImColor CurrentColor(255, 255, 255, alpha);  

		if (C_GET(bool, Vars.bHitmarker))
		{
			D::pDrawListActive->AddLine(ImVec2(DisplaySize.x / 2 - gap, DisplaySize.y / 2 - gap), ImVec2(DisplaySize.x / 2 - gap - size, DisplaySize.y / 2 - gap - size), CurrentColor, 1.f);  
			D::pDrawListActive->AddLine(ImVec2(DisplaySize.x / 2 - gap, DisplaySize.y / 2 + gap), ImVec2(DisplaySize.x / 2 - gap - size, DisplaySize.y / 2 + gap + size), CurrentColor, 1.f);  
			D::pDrawListActive->AddLine(ImVec2(DisplaySize.x / 2 + gap, DisplaySize.y / 2 - gap), ImVec2(DisplaySize.x / 2 + gap + size, DisplaySize.y / 2 - gap - size), CurrentColor, 1.f);  
			D::pDrawListActive->AddLine(ImVec2(DisplaySize.x / 2 + gap, DisplaySize.y / 2 + gap), ImVec2(DisplaySize.x / 2 + gap + size, DisplaySize.y / 2 + gap + size), CurrentColor, 1.f);   
		}

	}  
	nOldHitCount = nHitCount;  
}

void OVERLAY::Player(CCSPlayerController* pLocal, CCSPlayerController* pPlayer, const float flDistance)
{
	C_CSPlayerPawn* pLocalPawn = I::GameResourceService->pGameEntitySystem->Get<C_CSPlayerPawn>(pLocal->GetPawnHandle());
	C_CSPlayerPawn* pPlayerPawn = I::GameResourceService->pGameEntitySystem->Get<C_CSPlayerPawn>(pPlayer->GetPawnHandle());

	if (pLocalPawn == nullptr || pPlayerPawn == nullptr)
		return;

	// make them spotted for the radar reveal
	if (!I::Engine->IsConnected() || !I::Engine->IsInGame())
		return;
	if (!pLocal->IsPawnAlive())
		return;
	if (C_GET(bool, Vars.bRadarReveal))
	{
		if (!pPlayer->IsPawnAlive())
			return;
		auto pSpottedState = &pPlayerPawn->IsSpottedState();
		if (!pSpottedState)
			return;
		pSpottedState->bSpotted = true;
	}


	// @note: this is a simple example of how to check if the player is visible

	// initialize trace, construct filterr and initialize ray
	//GameTrace_t trace = GameTrace_t();
	//TraceFilter_t filter = TraceFilter_t(0x1C3003, pLocalPawn, nullptr, 4);
	//Ray_t ray = Ray_t();

	// cast a ray from local player eye positon -> player head bone
	// @note: would recommend checking for nullptrs
	//I::GameTraceManager->TraceShape(&ray, pLocalPawn->GetEyePosition(), pPlayerPawn->GetGameSceneNode()->GetSkeletonInstance()->pBoneCache->GetOrigin(6), &filter, &trace);
	// check if the hit entity is the one we wanted to check and if the trace end point is visible
	//if (trace.m_pHitEntity != pPlayerPawn || !trace.IsVisible( ))
	//	return;

	bool bIsEnemy = (pLocalPawn->IsOtherEnemy(pPlayerPawn));

	// @note: only enemy overlay for now
	if (!bIsEnemy)
		return;

	ImVec4 vecBox = {};
	if (!GetEntityBoundingBox(pPlayerPawn, &vecBox))
		return;

	Context_t context;

	if (const auto& frameOverlayConfig = C_GET(FrameOverlayVar_t, Vars.overlayBox); frameOverlayConfig.bEnable)
	{
		//context.AddBoxComponent(D::pDrawListActive, vecBox, 1, frameOverlayConfig.flThickness, frameOverlayConfig.flRounding, frameOverlayConfig.colPrimary, frameOverlayConfig.colOutline);
		D::pDrawListActive->AddRect(ImVec2(vecBox.x - 1, vecBox.y - 1), ImVec2(vecBox.z + 1, vecBox.w + 1), ImColor(0, 0, 0));
		D::pDrawListActive->AddRect(ImVec2(vecBox.x + 1, vecBox.y + 1), ImVec2(vecBox.z - 1, vecBox.w - 1), ImColor(0, 0, 0));
		D::pDrawListActive->AddRect(ImVec2(vecBox.x, vecBox.y), ImVec2(vecBox.z, vecBox.w), ImColor(255, 255, 255));
	}
		

	if (const auto& nameOverlayConfig = C_GET(TextOverlayVar_t, Vars.overlayName); nameOverlayConfig.bEnable)
	{
		const char* szPlayerName = pPlayer->GetPlayerName();
		ImVec2 vecTextSize = ImGui::CalcTextSize(szPlayerName);
		float flBoxWidth = abs(vecBox.z - vecBox.x);
		OVERLAY::AddText(szPlayerName, ImVec2(vecBox.z - flBoxWidth / 2 - vecTextSize.x / 2, vecBox.y - vecTextSize.y), ImColor(255, 255, 255));
	}

	if (const auto& healthOverlayConfig = C_GET(BarOverlayVar_t, Vars.overlayHealthBar); healthOverlayConfig.bEnable)
	{
		// @note: pPlayerPawn->GetMaxHealth() sometime return 0.f
		float flBoxWidth = abs(vecBox.z - vecBox.x);
		const float flHealthFactor = pPlayerPawn->GetHealth() / 100.f;
		ImColor FullHealthColor = ImColor(0, 255, 0, 255);
		ImColor LowestHealthColor = ImColor(255, 0, 0, 255);
		int r = FullHealthColor.Value.x * 255 * flHealthFactor + LowestHealthColor.Value.x * 255 * (1 - flHealthFactor);
		int g = FullHealthColor.Value.y * 255 * flHealthFactor + LowestHealthColor.Value.y * 255 * (1 - flHealthFactor);
		int b = FullHealthColor.Value.z * 255 * flHealthFactor + LowestHealthColor.Value.z * 255 * (1 - flHealthFactor);
		int a = FullHealthColor.Value.w * 255 * flHealthFactor + LowestHealthColor.Value.w * 255 * (1 - flHealthFactor);
		ImColor CurrentHealthColor = ImColor(r, g, b, a);
		float healthBarWidth = 2.0f;
		float healthBarDistance = 3.0f;
		D::pDrawListActive->AddRectFilled(ImVec2(vecBox.x - healthBarDistance - healthBarWidth - 1, vecBox.y - 1), ImVec2(vecBox.x - healthBarDistance + 1, vecBox.w + 1), ImColor(0, 0, 0, 175));
		float barTopY = vecBox.w - (vecBox.w - vecBox.y) * flHealthFactor;
		D::pDrawListActive->AddRectFilled(ImVec2(vecBox.x - healthBarDistance - healthBarWidth, barTopY), ImVec2(vecBox.x - healthBarDistance, vecBox.w), CurrentHealthColor);
		//context.AddComponent(new CBarComponent(false, SIDE_LEFT, vecBox, flHealthFactor, Vars.overlayHealthBar));
		int iPlayerHealth = pPlayerPawn->GetHealth();
		if (iPlayerHealth < 100 && C_GET(bool, Vars.bHealthText))
		{
			ImVec2 HealthTextSize = ImGui::CalcTextSize(std::to_string(iPlayerHealth).c_str());
			D::pDrawListActive->AddText(FONT::pMenu[0], FONT::pMenu[0]->FontSize - 2, ImVec2(vecBox.x - healthBarDistance - healthBarWidth - 1 - HealthTextSize.x / 2, barTopY - HealthTextSize.y / 2 + 1.0f), ImColor(0, 0, 0), std::to_string(iPlayerHealth).c_str());
			D::pDrawListActive->AddText(FONT::pMenu[0], FONT::pMenu[0]->FontSize - 2, ImVec2(vecBox.x - healthBarDistance - healthBarWidth - 1 - HealthTextSize.x / 2, barTopY - HealthTextSize.y / 2), ImColor(255, 255, 255), std::to_string(iPlayerHealth).c_str());
		}
	}

	if (const auto& armorOverlayConfig = C_GET(BarOverlayVar_t, Vars.overlayArmorBar); armorOverlayConfig.bEnable)
	{
		const float flArmorFactor = pPlayerPawn->GetArmorValue() / 100.f;
		context.AddComponent(new CBarComponent(false, SIDE_BOTTOM, vecBox, flArmorFactor, Vars.overlayArmorBar));
	}

	if (C_GET(bool, Vars.bEnemyWeaponEsp))
	{
		for (int i = 0; i == 0; i++)
		{
			auto pWeaponServices = pPlayerPawn->GetWeaponServices();
			if (!pWeaponServices)
				break;
			CBaseHandle hActiveWeapon = pWeaponServices->GetActiveWeapon();
			C_CSWeaponBase* pWeapon = I::GameResourceService->pGameEntitySystem->Get<C_CSWeaponBase>(hActiveWeapon);
			if (!pWeapon)
				break;
			auto pWeaponVData = reinterpret_cast<CCSWeaponBaseVData*>(pWeapon->GetVData());
			if (!pWeaponVData)
				break;
			const std::string prefix = "weapon_";
			std::string truncatedName = pWeaponVData->GetName();;
			if (truncatedName.find(prefix) == 0) {
				truncatedName = truncatedName.substr(prefix.length());
			}
			size_t underscorePos = truncatedName.find('_');
			if (underscorePos != std::string::npos) {
				truncatedName = truncatedName.substr(0, underscorePos);
			}

			ImVec2 vecTextSize = ImGui::CalcTextSize(truncatedName.c_str());
			float flBoxWidth = abs(vecBox.z - vecBox.x);
			OVERLAY::AddText(truncatedName.c_str(), ImVec2(vecBox.z - flBoxWidth / 2 - vecTextSize.x / 2, vecBox.w /* + vecTextSize.y / 2*/), ImColor(255, 255, 255));
		}	
	}

	if (C_GET(unsigned int, Vars.arrESPFlags))
	{
		int activeFlagCount = 0;
		unsigned int Array = C_GET(unsigned int, Vars.arrESPFlags);
		// Assuming 'Array' is an unsigned int bitmask representing flags, you can iterate over its bits like this:
		//	"Defusing",
		//	"Armor",
		//	"Scoped",
		//	"Kit",
		//	"Reloading"
		//	"Ping"
		for (int i = 0; i < 32; ++i) // 32 bits in unsigned int
		{
			if (Array & (1u << i))
			{
				if (i == 0)
				{
					if (!pPlayerPawn->IsDefusing())
						continue;
					OVERLAY::AddTextFlag("Defusing", ImVec2(vecBox.z + 3, vecBox.y + activeFlagCount * 10), ImColor(122, 184, 255));
					activeFlagCount += 1;
				}
				if (i == 1)
				{
					if (pPlayerPawn->GetArmorValue() < 0)
						continue;
					OVERLAY::AddTextFlag("HK", ImVec2(vecBox.z + 3, vecBox.y + activeFlagCount * 10), ImColor(122, 184, 255)); // Needs fix for no ct
					activeFlagCount += 1;
				}
				if (i == 2)
				{
					auto pWeaponServices = pPlayerPawn->GetWeaponServices();
					if (!pWeaponServices)
						continue;
					CBaseHandle hWeapon = pWeaponServices->GetActiveWeapon();
					auto pWeapon = I::GameResourceService->pGameEntitySystem->Get<C_CSWeaponBase>(hWeapon);
					if (!pWeapon)
						continue;
					auto pWeaponVData = pWeapon->GetWeaponVData();
					if (!pWeaponVData)
						continue;
					if (pWeaponVData->GetWeaponType() != WEAPONTYPE_SNIPER_RIFLE)
						continue;
					auto pActiveWeaponBaseGun = I::GameResourceService->pGameEntitySystem->Get<C_CSWeaponBaseGun>(pWeapon->GetRefEHandle());
					if (!pActiveWeaponBaseGun)
						continue;
					if (pActiveWeaponBaseGun->GetZoomLevel() == 0)
						continue;
					OVERLAY::AddTextFlag("Scoped", ImVec2(vecBox.z + 3, vecBox.y + activeFlagCount * 10), ImColor(122, 184, 255));
					activeFlagCount += 1;
				}
				if (i == 3)
				{
					if (!pPlayer->IsPawnHasDefuser())
						continue;
					OVERLAY::AddTextFlag("Kit", ImVec2(vecBox.z + 3, vecBox.y + activeFlagCount * 10), ImColor(122, 184, 255));
					activeFlagCount += 1;
				}
				if (i == 4)
				{
					auto pWeaponServices = pPlayerPawn->GetWeaponServices();
					if (!pWeaponServices)
						continue;
					CBaseHandle hWeapon = pWeaponServices->GetActiveWeapon();
					auto pWeapon = I::GameResourceService->pGameEntitySystem->Get<C_CSWeaponBase>(hWeapon);
					if (!pWeapon)
						continue;
					if (!pWeapon->IsInReload())
						continue;
					OVERLAY::AddTextFlag("Reloading", ImVec2(vecBox.z + 3, vecBox.y + activeFlagCount * 10), ImColor(255, 163, 51));
					activeFlagCount += 1;
				}
				if (i == 5)
				{
					std::uint32_t nPing = pPlayer->GetPing();
					auto szPing = std::to_string(nPing).c_str();
					OVERLAY::AddTextFlag(szPing, ImVec2(vecBox.z + 3, vecBox.y + activeFlagCount * 10), ImColor(255, 163, 51));
					activeFlagCount += 1;
				}
				/*
				if (i == 2)
				{
					OVERLAY::AddTextFlag("Planting", ImVec2(vecBox.z + 3, vecBox.y + activeFlagCount * 10), ImColor(255, 51, 51));
					activeFlagCount += 1;
				}
				if (i == 3)
				{
					OVERLAY::AddTextFlag("Bomb", ImVec2(vecBox.z + 3, vecBox.y + activeFlagCount * 10), ImColor(255, 51, 51));
					activeFlagCount += 1;
				}
				*/
			}
		}
	}

	// render all the context
	context.Render(D::pDrawListActive, vecBox);
}

void F::VISUALS::OVERLAY::FOVCircle()
{
	if (!C_GET(bool, Vars.bLegitbot) || !C_GET(bool, Vars.bLegitbotFOV) || C_GET(int, Vars.nAimMode) == 0)
		return;

	ImVec2 DisplaySize = ImGui::GetIO().DisplaySize;
	D::pDrawListActive->AddCircle(ImVec2(DisplaySize.x / 2, DisplaySize.y / 2), C_GET(float, Vars.flAimRange) * 6, ImColor(255, 255, 255));
}

void F::VISUALS::OVERLAY::ScopeOverlay()
{
	if (!C_GET(bool, Vars.bRemoveScope))
		return;
	if (!I::Engine->IsConnected() || !I::Engine->IsInGame())
		return;
	C_CSPlayerPawn* pLocal = SDK::LocalPawn;
	if (!pLocal)
		return;
	auto pWeaponServices = pLocal->GetWeaponServices();
	if (!pWeaponServices)
		return;
	CBaseHandle hActiveWeapon = pWeaponServices->GetActiveWeapon();
	auto pWeapon = I::GameResourceService->pGameEntitySystem->Get<C_CSWeaponBase>(hActiveWeapon);
	if (!pWeapon)
		return;
	CCSWeaponBaseVData* pWeaponVData = reinterpret_cast<CCSWeaponBaseVData *>(pWeapon->GetVData());
	if (!pWeaponVData)
		return;
	if (pWeaponVData->GetWeaponType() != WEAPONTYPE_SNIPER_RIFLE)
		return;
	auto pActiveWeaponBaseGun = I::GameResourceService->pGameEntitySystem->Get<C_CSWeaponBaseGun>(pWeapon->GetRefEHandle());
	if (!pActiveWeaponBaseGun)
		return;
	if (pActiveWeaponBaseGun->GetZoomLevel() == 0)
		return;

	ImVec2 DisplaySize = ImGui::GetIO().DisplaySize;
	D::pDrawListActive->AddLine(ImVec2(0, DisplaySize.y / 2), ImVec2(DisplaySize.x, DisplaySize.y / 2), ImColor(0, 0, 0)); // Horizontal Line
	D::pDrawListActive->AddLine(ImVec2(DisplaySize.x / 2, 0), ImVec2(DisplaySize.x / 2, DisplaySize.y), ImColor(0, 0, 0));
}

bool OVERLAY::CanWallbang(float& dmg, CCSPlayerController* pLocalController)
{
	if (!pLocalController || !I::Engine->IsConnected() || !I::Engine->IsInGame())
		return false;
	C_CSPlayerPawn* pLocalPawn = I::GameResourceService->pGameEntitySystem->Get<C_CSPlayerPawn>(pLocalController->GetPawnHandle());
	if (!pLocalPawn || pLocalPawn->GetHealth() <= 0)
		return false;

	// Get weapon information
	CCSPlayer_WeaponServices* pWeaponService = pLocalPawn->GetWeaponServices();
	if (!pWeaponService)
		return false;

	CBaseHandle hWeaponHandle = pWeaponService->GetActiveWeapon();
	C_CSWeaponBase* pWeapon = I::GameResourceService->pGameEntitySystem->Get<C_CSWeaponBase>(hWeaponHandle);
	if (!pWeapon)
		return false;

	CCSWeaponBaseVData* pWeaponVData = pWeapon->GetWeaponVData();
	if (!pWeaponVData)
		return false;

	return false;
}

void OVERLAY::SpreadCircle(CCSPlayerController* pLocal)
{
	if (!C_GET(bool, Vars.bSpreadCircle))
		return;
	if (!I::Engine->IsConnected() || !I::Engine->IsInGame())
		return;
	if (!pLocal->IsPawnAlive())
		return;
	C_CSPlayerPawn* pLocalPawn = I::GameResourceService->pGameEntitySystem->Get<C_CSPlayerPawn>(pLocal->GetPawnHandle());
	if (!pLocalPawn || pLocalPawn->GetHealth() <= 0)
		return;
	ImVec2 vecDisplaySize = ImGui::GetIO().DisplaySize;
		
	CCSPlayer_WeaponServices* pWeaponService = pLocalPawn->GetWeaponServices();
	if (!pWeaponService)
		return;

	CBaseHandle hWeaponHandle = pLocalPawn->GetWeaponServices()->GetActiveWeapon();
	C_CSWeaponBase* pWeapon = I::GameResourceService->pGameEntitySystem->Get<C_CSWeaponBase>(hWeaponHandle);
	if (!pWeapon)
		return;
	auto pWeaponVData = pWeapon->GetWeaponVData();
	if (!pWeaponVData)
		return;
	if (pWeaponVData->GetWeaponType() == WEAPONTYPE_KNIFE || pWeaponVData->GetWeaponType() == WEAPONTYPE_GRENADE || pWeaponVData->GetWeaponType() == WEAPONTYPE_C4 || pWeaponVData->GetWeaponType() == WEAPONTYPE_EQUIPMENT)
		return;

	float flSpread = pWeapon->GetSpread();
	float flInaccuracy = pWeapon->GetInaccuracy();
	float flCombinedSpread = sqrtf(flSpread * flSpread + flInaccuracy * flInaccuracy);

	float fov = 90.0f; // Default FOV
	if (C_GET(bool, Vars.bFovChange))
		fov = C_GET(float, Vars.flFov);
	float fSpreadDistance = ((flInaccuracy + flSpread) * 320.0f / tanf(M_DEG2RAD(fov) / 2));
	//fSpreadDistance = (int)(fSpreadDistance / 480.0f);

	//D::pDrawListActive->AddCircleFilled(ImVec2(vecDisplaySize.x / 2, vecDisplaySize.y / 2), fSpreadDistance, ImColor(255, 255, 255));

	auto Color = C_GET(Color_t, Vars.colSpreadCircle);
	ImColor ColorCenter = ImColor(Color.r, Color.g, Color.b, 0);
	ImColor ColorBorder = ImColor(Color.r, Color.g, Color.b, Color.a);

	float radius = fSpreadDistance;
	const int steps = 40; // More steps = smoother gradient
	if (MENU::bMainWindowOpened)
		return;
	for (int i = 1; i <= steps; ++i)
	{
		float t = (float)i / steps;
		float radius = fSpreadDistance * t;
		int alpha = static_cast<int>(Color.a * t); // Fade in alpha from 0 to Color.a
		ImColor col(Color.r, Color.g, Color.b, alpha);
		D::pDrawListActive->AddCircle(ImVec2(vecDisplaySize.x / 2, vecDisplaySize.y / 2), radius, col, 20);
	}
}


void OVERLAY::PenetrationCrosshair(CCSPlayerController* pLocal)
{
	if (!C_GET(bool, Vars.bAutoWallCrosshair))
		return;
	if (!I::Engine->IsConnected() || !I::Engine->IsInGame())
		return;
	C_CSPlayerPawn* pLocalPawn = I::GameResourceService->pGameEntitySystem->Get<C_CSPlayerPawn>(pLocal->GetPawnHandle());
	if (!pLocalPawn || pLocalPawn->GetHealth() <= 0)
		return;
	ImVec2 vecDisplaySize = ImGui::GetIO().DisplaySize;

	CCSPlayer_WeaponServices* pWeaponService = pLocalPawn->GetWeaponServices();
	if (!pWeaponService)
		return;

	CBaseHandle hWeaponHandle = pLocalPawn->GetWeaponServices()->GetActiveWeapon();
	C_CSWeaponBase* pWeapon = I::GameResourceService->pGameEntitySystem->Get<C_CSWeaponBase>(hWeaponHandle);
	if (!pWeapon)
		return;
	auto pWeaponVData = pWeapon->GetWeaponVData();
	if (!pWeaponVData)
		return;
	if (pWeaponVData->GetWeaponType() == WEAPONTYPE_KNIFE || pWeaponVData->GetWeaponType() == WEAPONTYPE_GRENADE || pWeaponVData->GetWeaponType() == WEAPONTYPE_C4 || pWeaponVData->GetWeaponType() == WEAPONTYPE_EQUIPMENT)
		return;

	float damage = 0.0f;
	if (CanWallbang(damage, pLocal))
		D::pDrawListActive->AddCircleFilled(ImVec2(vecDisplaySize.x / 2, vecDisplaySize.y / 2), 2.0f, ImColor(0, 255, 0));
	else
		D::pDrawListActive->AddCircleFilled(ImVec2(vecDisplaySize.x / 2, vecDisplaySize.y / 2), 2.0f, ImColor(255, 0, 0));
}
