#include "imgui_addons.h"

#include <map>
#include <string>


using namespace ImGui;
using std::string;

ImVec4 ImAdd::HexToColorVec4(unsigned int hex_color, float alpha) {
    ImVec4 color;

    color.x = ((hex_color >> 16) & 0xFF) / 255.0f;
    color.y = ((hex_color >> 8) & 0xFF) / 255.0f;
    color.z = (hex_color & 0xFF) / 255.0f;
    color.w = alpha;

    return color;
}

void ImAdd::DoubleText(ImVec4 color1, ImVec4 color2, const char* label1, const char* label2)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(std::string(label1 + std::string(label2)).c_str());
    const ImVec2 label1_size = CalcTextSize(label1, NULL, true);
    const ImVec2 label2_size = CalcTextSize(label2, NULL, true);

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = CalcItemSize(ImVec2(-0.1f, g.FontSize), label1_size.x + label2_size.x, g.FontSize);

    const ImRect total_bb(pos, pos + size);
    ItemSize(total_bb);
    if (!ItemAdd(total_bb, id)) {
        return;
    }

    window->DrawList->AddText(pos, GetColorU32(color1), label1);
    window->DrawList->AddText(pos + ImVec2(size.x - ImGui::CalcTextSize(label2).x, 0), GetColorU32(color2), label2);
}

void ImAdd::SeparatorText(const char* label, float thickness)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = CalcTextSize(label, NULL, true);

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = CalcItemSize(ImVec2(-0.1f, g.FontSize), label_size.x, g.FontSize);

    const ImRect total_bb(pos, pos + size);
    ItemSize(total_bb);
    if (!ItemAdd(total_bb, id)) {
        return;
    }

    window->DrawList->AddText(pos, GetColorU32(ImGuiCol_TextDisabled), label);

    if (thickness > 0)
        window->DrawList->AddLine(pos + ImVec2(label_size.x + style.ItemInnerSpacing.x, size.y / 2), pos + ImVec2(size.x, size.y / 2), GetColorU32(ImGuiCol_Border), thickness);
}

void ImAdd::VSeparator(float margin, float thickness)
{
    if (thickness <= 0)
        return;

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = CalcItemSize(ImVec2(thickness, -0.1f), thickness, thickness);

    const ImRect bb(pos, pos + size);
    const ImRect bb_rect(pos + ImVec2(0, margin), pos + size - ImVec2(0, margin));

    ItemSize(ImVec2(thickness, 0.0f));
    if (!ItemAdd(bb, 0))
        return;

    window->DrawList->AddRectFilled(bb_rect.Min, bb_rect.Max, GetColorU32(ImGuiCol_Border));
}

bool ImAdd::RadioFrame(const char* label, int* v, int radio_id, const ImVec2& size_arg)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = CalcTextSize(label, NULL, true);

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

    const ImRect total_bb(pos, pos + size);
    ItemSize(size, style.FramePadding.y);
    if (!ItemAdd(total_bb, id))
        return false;

    bool hovered, held, active;
    bool pressed = ButtonBehavior(total_bb, id, &hovered, &held);
    active = *v == radio_id;
    if (pressed) {
        *v = radio_id;
    }

    bool has_border = style.FrameBorderSize > 0;

    // Colors
    ImVec4 colFrameMain = GetStyleColorVec4(ImGuiCol_SliderGrab);
    ImVec4 colFrameNull = colFrameMain; colFrameNull.w = 0.0f;
    ImVec4 colFrame = (active ? colFrameMain : colFrameNull);

    ImVec4 colBorderMain = GetStyleColorVec4(ImGuiCol_Border);
    ImVec4 colBorderNull = colBorderMain; colBorderNull.w = 0.0f;
    ImVec4 colBorder = (active ? colBorderMain : colBorderNull);

    ImVec4 colLabel = GetStyleColorVec4(active ? ImGuiCol_WindowBg : (hovered && !held) ? ImGuiCol_Text : ImGuiCol_TextDisabled);

    // Animation
    struct stColors_State {
        ImVec4 Frame;
        ImVec4 Border;
        ImVec4 Label;
    };

    static std::map<ImGuiID, stColors_State> anim;
    auto it_anim = anim.find(id);

    if (it_anim == anim.end())
    {
        anim.insert({ id, stColors_State() });
        it_anim = anim.find(id);

        it_anim->second.Frame = colFrame;
        it_anim->second.Border = colBorder;
        it_anim->second.Label = colLabel;
    }

    it_anim->second.Frame = ImLerp(it_anim->second.Frame, colFrame, 1.0f / IMADD_ANIMATIONS_SPEED * ImGui::GetIO().DeltaTime);
    if (has_border) {    // good for preformance
        it_anim->second.Border = ImLerp(it_anim->second.Border, colBorder, 1.0f / IMADD_ANIMATIONS_SPEED * ImGui::GetIO().DeltaTime);
    }
    it_anim->second.Label = ImLerp(it_anim->second.Label, colLabel, 1.0f / IMADD_ANIMATIONS_SPEED * ImGui::GetIO().DeltaTime);

    // Render
    RenderNavHighlight(total_bb, id);

    window->DrawList->AddRectFilled(total_bb.Min, total_bb.Max, GetColorU32(it_anim->second.Frame), style.FrameRounding);
    if (has_border) {
        window->DrawList->AddRect(total_bb.Min, total_bb.Max, GetColorU32(it_anim->second.Border), style.FrameRounding, 0, style.FrameBorderSize);
    }

    window->DrawList->AddText(pos + style.FramePadding, GetColorU32(it_anim->second.Label), label);

    return pressed;
}

bool ImAdd::RadioFrameIcon(ImTextureID icon_texture, const char* label, int* v, int radio_id, const ImVec4& gradient_top, const ImVec4& gradient_bottom, const ImVec2& size_arg)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = CalcTextSize(label, NULL, true);
    bool has_icon = icon_texture != nullptr;

    const ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = CalcItemSize(size_arg, g.FontSize + style.CellPadding.x * 2 + style.CellPadding.y * 2, g.FontSize + style.CellPadding.x * 2 + style.CellPadding.y * 2);

    const ImRect total_bb(pos, pos + size);
    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, id))
    {
        IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
        return false;
    }

    bool hovered, held;
    bool pressed = ButtonBehavior(total_bb, id, &hovered, &held);
    if (pressed)
    {
        *v = radio_id;
        MarkItemEdited(id);
    }
    bool active = *v == radio_id;

    // Colors
    ImVec4 colFrameMain = GetStyleColorVec4(ImGuiCol_SliderGrab);
    ImVec4 colFrameNull = colFrameMain; colFrameNull.w = 0.0f;
    ImVec4 colFrame = (active ? colFrameMain : colFrameNull);

    ImVec4 colLabel = GetStyleColorVec4(active ? ImGuiCol_WindowBg : ImGuiCol_Text);
    //ImVec4 colLabel = GetStyleColorVec4(ImGuiCol_WindowBg);
    //ImVec4 colLabel = GetStyleColorVec4(ImGuiCol_Text);

    // Animation
    struct stColors_State {
        ImVec4 Frame;
        ImVec4 Label;
    };

    static std::map<ImGuiID, stColors_State> anim;
    auto it_anim = anim.find(id);

    if (it_anim == anim.end())
    {
        anim.insert({ id, stColors_State() });
        it_anim = anim.find(id);

        it_anim->second.Frame = colFrame;
        it_anim->second.Label = colLabel;
    }

    it_anim->second.Frame = ImLerp(it_anim->second.Frame, colFrame, 1.0f / IMADD_ANIMATIONS_SPEED * ImGui::GetIO().DeltaTime);
    it_anim->second.Label = ImLerp(it_anim->second.Label, colLabel, 1.0f / IMADD_ANIMATIONS_SPEED * ImGui::GetIO().DeltaTime);

    RenderNavHighlight(total_bb, id);

    window->DrawList->AddRectFilled(total_bb.Min, total_bb.Max, GetColorU32(it_anim->second.Frame), style.FrameRounding);
    window->DrawList->AddRectFilledMultiColorRounded(total_bb.Min + ImVec2(style.CellPadding.y * 2, style.CellPadding.x), total_bb.Min + ImVec2(size.y - style.CellPadding.x *  2 + style.CellPadding.y * 2, size.y - style.CellPadding.x), ImGui::GetColorU32(gradient_top), ImGui::GetColorU32(gradient_top), ImGui::GetColorU32(gradient_bottom), ImGui::GetColorU32(gradient_bottom), style.TabRounding);

    if (has_icon) {
        window->DrawList->AddImage(icon_texture, pos + ImVec2(style.CellPadding.y * 3, style.CellPadding.x + style.CellPadding.y), pos + ImVec2(style.CellPadding.y * 3, style.CellPadding.x + style.CellPadding.y) + ImVec2(g.FontSize, g.FontSize));
    }

    if (label_size.x > 0.0f) {
        PushStyleColor(ImGuiCol_Text, it_anim->second.Label);
        RenderText(pos + ImVec2(size.y - style.CellPadding.x * 2 + style.CellPadding.y * 4, size.y / 2 - label_size.y / 2), label);
        PopStyleColor();
    }

    return pressed;
}

bool ImAdd::Button(const char* label, const ImVec2& size_arg, ImGuiButtonFlags button_flags, ImDrawFlags draw_flags)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = CalcTextSize(label, NULL, true);

    ImVec2 pos = window->DC.CursorPos;
    if ((button_flags & ImGuiButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrLineTextBaseOffset) // Try to vertically align buttons that are smaller/have no padding so that text baseline matches (bit hacky, since it shouldn't be a flag)
        pos.y += window->DC.CurrLineTextBaseOffset - style.FramePadding.y;
    ImVec2 size = CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

    const ImRect bb(pos, pos + size);
    ItemSize(size, style.FramePadding.y);
    if (!ItemAdd(bb, id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held, button_flags);
    bool has_border = style.FrameBorderSize > 0;

    // Colors
    ImVec4 colFrame = GetStyleColorVec4((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);

    // Animations
    struct stColors_State {
        ImVec4 Frame;
    };

    static std::map<ImGuiID, stColors_State> anim;
    auto it_anim = anim.find(id);

    if (it_anim == anim.end())
    {
        anim.insert({ id, stColors_State() });
        it_anim = anim.find(id);

        it_anim->second.Frame = colFrame;
    }

    it_anim->second.Frame = ImLerp(it_anim->second.Frame, colFrame, 1.0f / IMADD_ANIMATIONS_SPEED * GetIO().DeltaTime);

    // Render
    RenderNavHighlight(bb, id);

    window->DrawList->AddRectFilled(bb.Min, bb.Max, GetColorU32(it_anim->second.Frame), style.FrameRounding, draw_flags);
    if (has_border)
    {
        window->DrawList->AddRect(bb.Min, bb.Max, GetColorU32(ImGuiCol_Border), style.FrameRounding, draw_flags, style.FrameBorderSize);
    }

    RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);

    return pressed;
}

bool ImAdd::ButtonCircle(const char* str_id, float radius)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(str_id);

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = CalcItemSize(ImVec2(radius, radius) * 2, g.FontSize, g.FontSize);

    const ImRect bb(pos, pos + size);
    ItemSize(size, style.FramePadding.y);
    if (!ItemAdd(bb, id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held);
    bool has_border = style.FrameBorderSize > 0;

    // Colors
    ImVec4 colFrame = GetStyleColorVec4((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);

    // Animations
    struct stColors_State {
        ImVec4 Frame;
    };

    static std::map<ImGuiID, stColors_State> anim;
    auto it_anim = anim.find(id);

    if (it_anim == anim.end())
    {
        anim.insert({ id, stColors_State() });
        it_anim = anim.find(id);

        it_anim->second.Frame = colFrame;
    }

    it_anim->second.Frame = ImLerp(it_anim->second.Frame, colFrame, 1.0f / IMADD_ANIMATIONS_SPEED * GetIO().DeltaTime);

    // Render
    RenderNavHighlight(bb, id);

    window->DrawList->AddCircleFilled(bb.Min + ImVec2(radius, radius), radius, GetColorU32(it_anim->second.Frame));
    if (has_border)
    {
        window->DrawList->AddCircle(bb.Min + ImVec2(radius, radius), radius, GetColorU32(ImGuiCol_Border), 0, style.FrameBorderSize);
    }

    return pressed;
}

bool ImAdd::CheckBox(const char* label, bool* v)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = CalcTextSize(label, NULL, true);

    const float square_sz = GetFontSize() + style.CellPadding.y * 2.0f;
    const ImVec2 pos = window->DC.CursorPos;
    const ImRect total_bb(pos, pos + ImVec2(square_sz + (label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f), label_size.y + style.CellPadding.y * 2.0f));
    ItemSize(total_bb, style.CellPadding.y);

    if (!ItemAdd(total_bb, id))
    {
        return false;
    }

    bool has_border = style.FrameBorderSize > 0;
    bool checked = *v;
    bool hovered, held;
    bool pressed = ButtonBehavior(total_bb, id, &hovered, &held);

    if (pressed) {
        checked = !checked;
    }

    if (*v != checked)
    {
        *v = checked;
        pressed = true; // return value
        MarkItemEdited(id);
    }

    const ImRect check_bb(pos, pos + ImVec2(square_sz, square_sz));

    // Colors
    ImVec4 colFrame = GetStyleColorVec4(checked ? ImGuiCol_SliderGrab : (held && hovered) ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);

    ImVec4 colBorderMain = GetStyleColorVec4(ImGuiCol_Border);
    ImVec4 colBorderNull = colBorderMain; colBorderNull.w = 0.0f;
    ImVec4 colBorder = (checked ? colBorderNull : colBorderMain);

    ImVec4 colCheckMarkMain = GetStyleColorVec4(ImGuiCol_CheckMark);
    ImVec4 colCheckMarkNull = colCheckMarkMain; colCheckMarkNull.w = 0.0f;
    ImVec4 colCheckMark = (checked ? colCheckMarkMain : colCheckMarkNull);

    // Animations
    struct stColors_State {
        ImVec4 Frame;
        ImVec4 Border;
        ImVec4 CheckMark;
    };

    static std::map<ImGuiID, stColors_State> anim;
    auto it_anim = anim.find(id);

    if (it_anim == anim.end())
    {
        anim.insert({ id, stColors_State() });
        it_anim = anim.find(id);

        it_anim->second.Frame = colFrame;
        it_anim->second.Border = colBorder;
        it_anim->second.CheckMark = colCheckMark;
    }

    it_anim->second.Frame = ImLerp(it_anim->second.Frame, colFrame, 1.0f / IMADD_ANIMATIONS_SPEED * GetIO().DeltaTime);
    it_anim->second.Border = ImLerp(it_anim->second.Border, colBorder, 1.0f / IMADD_ANIMATIONS_SPEED * GetIO().DeltaTime);
    it_anim->second.CheckMark = ImLerp(it_anim->second.CheckMark, colCheckMark, 1.0f / IMADD_ANIMATIONS_SPEED * GetIO().DeltaTime);

    RenderNavHighlight(total_bb, id);

    window->DrawList->AddRectFilled(check_bb.Min, check_bb.Max, GetColorU32(it_anim->second.Frame), style.FrameRounding);
    if (has_border)
    {
        window->DrawList->AddRect(check_bb.Min, check_bb.Max, GetColorU32(it_anim->second.Border), style.FrameRounding, 0, style.FrameBorderSize);
    }

    const float pad = ImMax(1.0f, IM_TRUNC(square_sz / 4.0f));
    RenderCheckMark(window->DrawList, check_bb.Min + ImVec2(pad, pad), GetColorU32(it_anim->second.CheckMark), square_sz - pad * 2.0f);

    const ImVec2 label_pos = ImVec2(check_bb.Max.x + style.ItemInnerSpacing.x, check_bb.Min.y + style.CellPadding.y);
    if (label_size.x > 0.0f) {
        RenderText(label_pos, label);
    }

    return pressed;
}

bool ImAdd::ToggleButtonClassic(const char* label, bool* v)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);

    const ImVec2 label_size = CalcTextSize(label, NULL, true);

    float height = g.FontSize;
    float width = height * 2;
    float rounding = height / 2;

    float grab_radius = (height / 2) - 1;
    float grab_rounding = grab_radius / 2;

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = ImVec2(label_size.x > 0 ? label_size.x + style.ItemInnerSpacing.x + width : width, height);

    const ImRect bb(pos, pos + size);
    ItemSize(size);
    if (!ItemAdd(bb, id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held);
    if (pressed)
        *v = !*v;

    // Colors
    ImVec4 colFrame = GetStyleColorVec4(*v ? ImGuiCol_SliderGrab : ImGuiCol_FrameBg);
    ImVec4 colBorder = GetStyleColorVec4(*v ? ImGuiCol_SliderGrabActive : ImGuiCol_Border);
    ImVec4 colGrab = GetStyleColorVec4(*v ? ImGuiCol_Text : ImGuiCol_TextDisabled);

    // Animations
    struct stColors_State {
        ImVec4 Frame;
        ImVec4 Border;
        ImVec4 Grab;
        float GrabPos;
    };

    static std::map<ImGuiID, stColors_State> anim;
    auto it_anim = anim.find(id);

    if (it_anim == anim.end())
    {
        anim.insert({ id, stColors_State() });
        it_anim = anim.find(id);

        it_anim->second.Frame = colFrame;
        it_anim->second.Border = colBorder;
        it_anim->second.Grab = colGrab;
        it_anim->second.GrabPos = *v ? 1.0f : 0.0f;
    }

    it_anim->second.Frame = ImLerp(it_anim->second.Frame, colFrame, 1.0f / IMADD_ANIMATIONS_SPEED * ImGui::GetIO().DeltaTime);
    it_anim->second.Border = ImLerp(it_anim->second.Border, colBorder, 1.0f / IMADD_ANIMATIONS_SPEED * ImGui::GetIO().DeltaTime);
    it_anim->second.Grab = ImLerp(it_anim->second.Grab, colGrab, 1.0f / IMADD_ANIMATIONS_SPEED * ImGui::GetIO().DeltaTime);
    it_anim->second.GrabPos = ImLerp<float>(it_anim->second.GrabPos, *v ? (width - rounding) : rounding, 1.0f / IMADD_ANIMATIONS_SPEED * ImGui::GetIO().DeltaTime);

    // Render
    RenderNavHighlight(bb, id);

    window->DrawList->AddRectFilled(pos, pos + ImVec2(width, size.y), GetColorU32(it_anim->second.Frame), rounding);
    if (style.FrameBorderSize)
        window->DrawList->AddRect(pos, pos + ImVec2(width, size.y), GetColorU32(it_anim->second.Border), rounding, 0, style.FrameBorderSize);

    float grab_pos = *v ? (width - rounding * 2) : 0;

    window->DrawList->AddCircleFilled(pos + ImVec2(it_anim->second.GrabPos, rounding), grab_radius, GetColorU32(it_anim->second.Grab));

    if (label_size.x > 0)
    {
        RenderText(pos + ImVec2(width + style.ItemInnerSpacing.x, 0), label);
    }

    return pressed;
}

bool ImAdd::ToggleButton(const char* label, bool* v, float spacing_arg)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);

    const ImVec2 label_size = CalcTextSize(label, NULL, true);

    float height = g.FontSize;
    float width = float(int(height * 1.8));
    float rounding = height / 2;

    float spacing = ImGui::CalcItemSize(ImVec2(spacing_arg > 0 ? spacing_arg : -0.1f, 0), label_size.x > 0 ? (label_size.x + style.ItemInnerSpacing.x) : 0, 0).x;

    float grab_radius = height / 2 - 2;
    float grab_rounding = grab_radius / 2;

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = ImVec2(label_size.x > 0 ? label_size.x + style.ItemInnerSpacing.x + width : width, height);

    const ImRect bb(pos, pos + ImVec2(spacing, size.y));
    ItemSize(size);
    if (!ItemAdd(bb, id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held);
    if (pressed)
        *v = !*v;

    // Colors - using ImGuiCol_SliderGrab for active state like in ToggleButtonClassic
    ImVec4 colFrame = GetStyleColorVec4(*v ? ImGuiCol_SliderGrab : ImGuiCol_FrameBgHovered);
    ImVec4 colBorder = GetStyleColorVec4(*v ? ImGuiCol_SliderGrabActive : ImGuiCol_Border);
    ImVec4 colGrab = GetStyleColorVec4(*v ? ImGuiCol_WindowBg : ImGuiCol_WindowBg);

    // Animations
    struct stColors_State {
        ImVec4 Frame;
        ImVec4 Border;
        ImVec4 Grab;
        float GrabPos;
    };

    static std::map<ImGuiID, stColors_State> anim;
    auto it_anim = anim.find(id);

    if (it_anim == anim.end())
    {
        anim.insert({ id, stColors_State() });
        it_anim = anim.find(id);

        it_anim->second.Frame = colFrame;
        it_anim->second.Border = colBorder;
        it_anim->second.Grab = colGrab;
        it_anim->second.GrabPos = *v ? 1.0f : 0.0f;
    }

    it_anim->second.Frame = ImLerp(it_anim->second.Frame, colFrame, 1.0f / IMADD_ANIMATIONS_SPEED * ImGui::GetIO().DeltaTime);
    it_anim->second.Border = ImLerp(it_anim->second.Border, colBorder, 1.0f / IMADD_ANIMATIONS_SPEED * ImGui::GetIO().DeltaTime);
    it_anim->second.Grab = ImLerp(it_anim->second.Grab, colGrab, 1.0f / IMADD_ANIMATIONS_SPEED * ImGui::GetIO().DeltaTime);
    it_anim->second.GrabPos = ImLerp<float>(it_anim->second.GrabPos, *v ? (width - rounding) : rounding, 1.0f / IMADD_ANIMATIONS_SPEED * ImGui::GetIO().DeltaTime);

    // Render
    RenderNavHighlight(bb, id);

    window->DrawList->AddRectFilled(pos + ImVec2(spacing - width, 0), pos + ImVec2(spacing - width, 0) + ImVec2(width, size.y), GetColorU32(it_anim->second.Frame), rounding);
    if (style.FrameBorderSize)
        window->DrawList->AddRect(pos + ImVec2(spacing - width, 0), pos + ImVec2(spacing - width, 0) + ImVec2(width, size.y), GetColorU32(it_anim->second.Border), rounding, 0, style.FrameBorderSize);

    float grab_pos = *v ? (width - rounding * 2) : 0;

    window->DrawList->AddCircleFilled(pos + ImVec2(spacing - width, 0) + ImVec2(it_anim->second.GrabPos, rounding), grab_radius, GetColorU32(it_anim->second.Grab));

    if (label_size.x > 0)
    {
        RenderText(pos, label);
    }

    return pressed;
}

void ImAdd::BeginChild(const char* str_id, const ImVec2& size, bool border)
{
    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;

    PushStyleVar(ImGuiStyleVar_WindowPadding, style.FramePadding);
    ImGui::BeginChild(str_id, size, border ? ImGuiChildFlags_Border : ImGuiChildFlags_AlwaysUseWindowPadding);
    PushStyleVar(ImGuiStyleVar_ItemSpacing, style.FramePadding);
}

void ImAdd::EndChild()
{
    PopStyleVar();
    ImGui::EndChild();
    PopStyleVar();
}

bool ImAdd::InputText(ImTextureID icon_texture, const char* label, char* buf, size_t buf_size, ImGuiInputTextFlags flags)
{
    IM_ASSERT(!(flags & ImGuiInputTextFlags_Multiline)); // call InputTextMultiline()

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;

    ImVec2 pos = GetCursorScreenPos();
    bool has_icon = icon_texture != nullptr;

    ImGui::BeginChild(label, ImVec2(CalcItemWidth(), GetFrameHeight()), ImGuiChildFlags_None, ImGuiWindowFlags_NoBackground);

    window->DrawList->AddRectFilled(pos, pos + GetWindowSize(), GetColorU32(ImGuiCol_FrameBg), style.FrameRounding);
    if (has_icon) {
        window->DrawList->AddImage(icon_texture, pos + style.FramePadding, pos + ImVec2(GetWindowHeight(), GetWindowHeight()) - style.FramePadding, ImVec2(0, 0), ImVec2(1, 1), GetColorU32(ImGuiCol_Text));
    }

    PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));
    PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);
    PushItemWidth(has_icon ? (GetWindowWidth() - GetFontSize() - style.FramePadding.x) : GetWindowWidth());
    if (has_icon) {
        SetCursorScreenPos(pos + ImVec2(GetFontSize() + style.FramePadding.x, 0));
    }
    bool input = InputTextEx(string("##" + string(label)).c_str(), NULL, buf, (int)buf_size, ImVec2(0, 0), flags);
    PopItemWidth();
    PopStyleVar();
    PopStyleColor();

    if (has_icon) {
        GetForegroundDrawList()->AddRectFilledMultiColor(pos + ImVec2(GetFontSize() + style.FramePadding.x, 0), pos + ImVec2(GetWindowHeight(), GetWindowHeight()), GetColorU32(ImGuiCol_FrameBg), GetColorU32(ImGuiCol_FrameBg, 0.0f), GetColorU32(ImGuiCol_FrameBg, 0.0f), GetColorU32(ImGuiCol_FrameBg));
    }

    if (!IsItemActive() && strlen(buf) == 0) {
        window->DrawList->AddText(pos + ImVec2(has_icon ? GetWindowHeight() : style.FramePadding.x, style.FramePadding.y), GetColorU32(ImGuiCol_TextDisabled), label);
    }

    ImGui::EndChild();

    return input;
}

bool ImAdd::Label(const char* label, const ImVec2& size_arg, const ImVec2& text_align)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = CalcTextSize(label, NULL, true);

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = CalcItemSize(size_arg, label_size.x, label_size.y);

    const ImRect bb(pos, pos + size);
    ItemSize(size);
    if (!ItemAdd(bb, id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held);

    RenderNavHighlight(bb, id);
    //    RenderFrame(bb.Min, bb.Max, IM_COL32(255, 0, 0, 255), false);
    RenderTextClipped(bb.Min, bb.Max, label, NULL, &label_size, text_align, &bb);

    return pressed;
}

bool ImAdd::SliderScalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const float width = CalcItemWidth();
    const ImVec2 pos = window->DC.CursorPos;

    const ImVec2 label_size = CalcTextSize(label, NULL, true);
    const bool has_label = label_size.x > 0;
    const float label_height = has_label ? g.FontSize + style.ItemInnerSpacing.y : 0.0f;
    const float slider_height = 10;

    const ImRect frame_bb(pos + ImVec2(0, label_height), pos + ImVec2(width, slider_height + label_height));
    const ImRect total_bb(pos, pos + ImVec2(width, slider_height + label_height));

    ItemSize(total_bb);
    if (!ItemAdd(total_bb, id, &frame_bb, 0))
        return false;

    if (format == NULL)
        format = DataTypeGetInfo(data_type)->PrintFmt;

    const bool hovered = ItemHoverable(frame_bb, id, g.LastItemData.InFlags);
    const bool clicked = hovered && IsMouseClicked(0, ImGuiInputFlags_None, id);
    const bool held = g.ActiveId == id;
    const bool make_active = (clicked || g.NavActivateId == id);
    if (make_active)
    {
        SetActiveID(id, window);
        SetFocusID(id, window);
        FocusWindow(window);
        g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
    }

    // Colors
    ImVec4 colFrame = GetStyleColorVec4(held ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
    ImVec4 colLine = GetStyleColorVec4(held ? ImGuiCol_SliderGrabActive : ImGuiCol_SliderGrab);

    // Animations
    struct stColors_State {
        ImVec4 Frame;
        ImVec4 Line;
    };

    static std::map<ImGuiID, stColors_State> anim;
    auto it_anim = anim.find(id);

    if (it_anim == anim.end())
    {
        anim.insert({ id, stColors_State() });
        it_anim = anim.find(id);

        it_anim->second.Frame = colFrame;
        it_anim->second.Line = colLine;
    }

    it_anim->second.Frame = ImLerp(it_anim->second.Frame, colFrame, 1.0f / IMADD_ANIMATIONS_SPEED * GetIO().DeltaTime);
    it_anim->second.Line = ImLerp(it_anim->second.Line, colLine, 1.0f / IMADD_ANIMATIONS_SPEED * GetIO().DeltaTime);

    // Slider behavior
    ImRect grab_bb;
    const bool value_changed = SliderBehavior(frame_bb, id, data_type, p_data, p_min, p_max, format, flags, &grab_bb);
    if (value_changed)
        MarkItemEdited(id);

    grab_bb.Min.x = grab_bb.Max.x - style.GrabMinSize;

    // Draw frame
    RenderNavHighlight(frame_bb, id);
    RenderFrame(frame_bb.Min, frame_bb.Max, GetColorU32(it_anim->second.Frame), true, g.Style.FrameRounding);

    // Render grab
    if (grab_bb.Max.x > grab_bb.Min.x) {
        window->DrawList->AddRectFilled(frame_bb.Min, ImVec2(grab_bb.Max.x + (grab_bb.Min.y - frame_bb.Min.y), frame_bb.Max.y), GetColorU32(it_anim->second.Line), style.FrameRounding);
    }

    char value_buf[64];
    const char* value_buf_end = value_buf + DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, p_data, format);

    if (has_label) {
        window->DrawList->AddText(total_bb.Min, GetColorU32(ImGuiCol_Text), label);
        window->DrawList->AddText(total_bb.Min + ImVec2(width - CalcTextSize(value_buf).x, 0), GetColorU32(ImGuiCol_TextDisabled), value_buf);
    }

    return value_changed;
}

bool ImAdd::SliderFloat(const char* label, float* v, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
    return SliderScalar(label, ImGuiDataType_Float, v, &v_min, &v_max, format, flags);
}

bool ImAdd::SliderAngle(const char* label, float* v_rad, float v_degrees_min, float v_degrees_max, const char* format, ImGuiSliderFlags flags)
{
    if (format == NULL)
        format = "%.0f deg";
    float v_deg = (*v_rad) * 360.0f / (2 * IM_PI);
    bool value_changed = SliderFloat(label, &v_deg, v_degrees_min, v_degrees_max, format, flags);
    *v_rad = v_deg * (2 * IM_PI) / 360.0f;
    return value_changed;
}

bool ImAdd::SliderInt(const char* label, int* v, int v_min, int v_max, const char* format, ImGuiSliderFlags flags)
{
    return SliderScalar(label, ImGuiDataType_S32, v, &v_min, &v_max, format, flags);
}

bool ImAdd::Selectable(const char* label, bool selected, const ImVec2& size_arg)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = CalcTextSize(label, NULL, true);

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

    const ImRect bb(pos, pos + size);
    ItemSize(size, style.FramePadding.y);
    if (!ItemAdd(bb, id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held);
    float borderSize = style.FrameBorderSize;

    // Colors
    ImVec4 colFrameMain = GetStyleColorVec4((hovered && !selected) ? held ? ImGuiCol_HeaderActive : ImGuiCol_HeaderHovered : ImGuiCol_Header);
    ImVec4 colFrameNull = colFrameMain; colFrameNull.w = 0.0f;
    ImVec4 colFrame = ((!hovered && !selected) ? colFrameNull : colFrameMain);

    ImVec4 colBorderMain = GetStyleColorVec4(ImGuiCol_Border);
    ImVec4 colBorderNull = colBorderMain; colBorderNull.w = 0.0f;
    ImVec4 colBorder = (selected ? colBorderMain : colBorderNull);

    // Animations
    struct stColors_State {
        ImVec4 Frame;
        ImVec4 Border;
    };

    static std::map<ImGuiID, stColors_State> anim;
    auto it_anim = anim.find(id);

    if (it_anim == anim.end())
    {
        anim.insert({ id, stColors_State() });
        it_anim = anim.find(id);

        it_anim->second.Frame = colFrame;
        it_anim->second.Border = colBorder;
    }

    it_anim->second.Frame = ImLerp(it_anim->second.Frame, colFrame, 1.0f / IMADD_ANIMATIONS_SPEED * ImGui::GetIO().DeltaTime);
    it_anim->second.Border = ImLerp(it_anim->second.Border, colBorder, 1.0f / IMADD_ANIMATIONS_SPEED * ImGui::GetIO().DeltaTime);

    // Render
    RenderNavHighlight(bb, id);

    window->DrawList->AddRectFilled(bb.Min, bb.Max, GetColorU32(it_anim->second.Frame), style.FrameRounding);

    if (borderSize > 0)
        window->DrawList->AddRect(bb.Min, bb.Max, GetColorU32(it_anim->second.Border), style.FrameRounding, 0, borderSize);

    RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);

    return pressed;
}

bool ImAdd::SelectableLabel(ImDrawList* drawlist, const char* label, bool selected, const ImVec2& size_arg)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = CalcTextSize(label, NULL, true);

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = CalcItemSize(size_arg, label_size.x, label_size.y);

    const ImRect bb(pos, pos + size);
    ItemSize(size);
    if (!ItemAdd(bb, id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held);
    bool has_border = style.FrameBorderSize > 0;

    // Colors
    ImVec4 colText = GetStyleColorVec4(selected ? ImGuiCol_SliderGrab : (hovered && !held) ? ImGuiCol_Text : ImGuiCol_TextDisabled);

    // Animations
    struct stColors_State {
        ImVec4 Text;
    };

    static std::map<ImGuiID, stColors_State> anim;
    auto it_anim = anim.find(id);

    if (it_anim == anim.end())
    {
        anim.insert({ id, stColors_State() });
        it_anim = anim.find(id);

        it_anim->second.Text = colText;
    }

    it_anim->second.Text = ImLerp(it_anim->second.Text, colText, 1.0f / IMADD_ANIMATIONS_SPEED * GetIO().DeltaTime);

    // Render
    RenderNavHighlight(bb, id);

    drawlist->AddText(pos, GetColorU32(it_anim->second.Text), label);

    return pressed;
}

const char* GetStringByIndex(const char* multiString, size_t index)
{
    if (!multiString) return nullptr;

    size_t currentIndex = 0;
    const char* ptr = multiString;

    while (*ptr) {
        if (currentIndex == index)
            return ptr;

        // Move to the next string
        ptr += std::strlen(ptr) + 1;
        currentIndex++;
    }

    // Index not found
    return nullptr;
}

size_t GetMultiStringItemCount(const char* multiString)
{
    const char* ptr = multiString;
    size_t count = 0;

    while (*ptr) {
        ++count;
        ptr += std::strlen(ptr) + 1;
    }

    return count;
}

bool ImAdd::Combo(const char* str_id, int* current_item, const char* items_separated_by_zeros)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(str_id);

    const float width = CalcItemWidth();
    const float height = g.FontSize + style.CellPadding.y * 2;

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = ImVec2(width, height);

    const ImRect bb(pos, pos + size);
    ItemSize(size);
    if (!ItemAdd(bb, id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held);
    bool has_border = style.FrameBorderSize > 0;

    // Colors
    ImVec4 colFrame = GetStyleColorVec4((held && hovered) ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_WindowBg);

    ImVec4 colBorderMain = GetStyleColorVec4(ImGuiCol_Separator);
    ImVec4 colBorderNull = colBorderMain; colBorderNull.w = 0.0f;
    ImVec4 colBorder = ((!hovered) ? colBorderMain : colBorderNull);

    // Animations
    struct stColors_State {
        ImVec4 Frame;
        ImVec4 Border;
    };

    static std::map<ImGuiID, stColors_State> anim;
    auto it_anim = anim.find(id);

    if (it_anim == anim.end())
    {
        anim.insert({ id, stColors_State() });
        it_anim = anim.find(id);

        it_anim->second.Frame = colFrame;
        it_anim->second.Border = colBorder;
    }

    it_anim->second.Frame = ImLerp(it_anim->second.Frame, colFrame, 1.0f / IMADD_ANIMATIONS_SPEED * GetIO().DeltaTime);
    it_anim->second.Border = ImLerp(it_anim->second.Border, colBorder, 1.0f / IMADD_ANIMATIONS_SPEED * GetIO().DeltaTime);

    // Render
    RenderNavHighlight(bb, id);

    window->DrawList->AddRectFilled(bb.Min, bb.Max, GetColorU32(it_anim->second.Frame), style.FrameRounding);
    //if (has_border)
    {
        window->DrawList->AddRect(bb.Min, bb.Max, GetColorU32(it_anim->second.Border), style.FrameRounding, 0, style.FrameBorderSize);
    }

    string item_preview = GetStringByIndex(items_separated_by_zeros, *current_item);

    window->DrawList->AddText(pos + style.CellPadding, GetColorU32(ImGuiCol_Text), item_preview.c_str() == nullptr ? "*error item*" : item_preview.c_str());

    if (pressed)
    {
        OpenPopup(str_id);
    }

    PushStyleVar(ImGuiStyleVar_WindowPadding, style.CellPadding);
    PushStyleVar(ImGuiStyleVar_ItemSpacing, style.CellPadding);
    PushStyleVar(ImGuiStyleVar_FramePadding, style.CellPadding);
    PushStyleVar(ImGuiStyleVar_PopupRounding, style.FrameRounding);
    PushStyleVar(ImGuiStyleVar_PopupBorderSize, style.FrameBorderSize);
    if (BeginPopup(str_id, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground))
    {
        ImDrawList* pDrawList = GetForegroundDrawList();
        pDrawList->AddRectFilled(pos, pos + GetWindowSize(), GetColorU32(ImGuiCol_PopupBg), style.PopupRounding);

        const int ItemsCount = GetMultiStringItemCount(items_separated_by_zeros);
        const float PopupHeight =
            (ItemsCount * g.FontSize + style.FramePadding.y * 2) +  // Total frames
            ((ItemsCount - 1) * style.ItemSpacing.y) +              // Total spaces
            (style.WindowPadding.y * 2);                            // window padding

        SetWindowPos(pos, ImGuiCond_Always);
        //SetWindowSize(ImVec2(width, PopupHeight), ImGuiCond_Always);

        for (size_t i = 0; i < ItemsCount; i++)
        {
            string item_label = GetStringByIndex(items_separated_by_zeros, i);

            if (SelectableLabel(pDrawList, item_label.c_str(), i == *current_item, ImVec2(width - style.WindowPadding.x * 2, 0)))
            {
                *current_item = i;
                CloseCurrentPopup();
            }
        }

        EndPopup();
    }
    PopStyleVar(5);

    return pressed;
}

bool ImAdd::Combo(const char* str_id, int* current_item, const char* items_separated_by_zeros, float width)
{
    PushItemWidth(width <= 0 ? CalcItemWidth() : width);
    bool result = Combo(str_id, current_item, items_separated_by_zeros);
    PopItemWidth();
    return result;
}

bool ImAdd::MultiCombo(const char* label, std::vector<int>* selected, std::vector<const char*> items, float width)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;

    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = CalcTextSize(label, NULL, true);

    float height = g.FontSize + style.CellPadding.y * 2;

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = CalcItemSize(ImVec2(width, 0), label_size.x + style.CellPadding.x + height, height);

    const ImRect bb(pos, pos + size + ImVec2(0, style.ItemInnerSpacing.y + g.FontSize));
    ItemSize(size + ImVec2(0, g.FontSize + style.ItemInnerSpacing.y));
    if (!ItemAdd(bb, id))
        return false;

    bool hovered, held, popup_open = false;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held);
    if (pressed) popup_open = !popup_open;
    bool empty = true;

    std::string preview = "";

    int stop_at = -1;
    for (int i = 0; i < items.size(); i++)
    {
        if ((*selected)[i] == 1)
        {
            stop_at = i;
            empty = false;
        }
    }

    for (int i = 0; i < items.size(); i++)
    {
        if ((*selected)[i] == 1)
        {
            preview += items[i] + std::string(i == stop_at ? "" : ", ");
        }
    }

    // Colors
    ImVec4 colFrame = GetStyleColorVec4(hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
    ImVec4 colText = GetStyleColorVec4((popup_open || hovered) ? ImGuiCol_Text : ImGuiCol_TextDisabled);

    // Animations
    struct stColors_State {
        ImVec4 Frame;
        ImVec4 Text;
    };

    static std::map<ImGuiID, stColors_State> anim;
    auto it_anim = anim.find(id);

    if (it_anim == anim.end())
    {
        anim.insert({ id, stColors_State() });
        it_anim = anim.find(id);

        it_anim->second.Frame = colFrame;
        it_anim->second.Text = colText;
    }

    it_anim->second.Frame = ImLerp(it_anim->second.Frame, colFrame, 1.0f / IMADD_ANIMATIONS_SPEED * ImGui::GetIO().DeltaTime);
    it_anim->second.Text = ImLerp(it_anim->second.Text, colText, 1.0f / IMADD_ANIMATIONS_SPEED * ImGui::GetIO().DeltaTime);
    ImVec4 Grad = it_anim->second.Frame; Grad.w = 0;

    // Render
    RenderNavHighlight(bb, id);
    RenderFrame(bb.Min + ImVec2(0, style.ItemInnerSpacing.y + g.FontSize), bb.Max, GetColorU32(it_anim->second.Frame), false, style.FrameRounding);

    window->DrawList->AddText(pos, GetColorU32(ImGuiCol_Text), label);

    window->DrawList->PushClipRect(pos + ImVec2(0, g.FontSize + style.ItemInnerSpacing.y), pos + ImVec2(size.x - size.y, g.FontSize + style.ItemInnerSpacing.y + size.y));
    window->DrawList->AddText(pos + ImVec2(style.CellPadding.x, g.FontSize + style.ItemInnerSpacing.y + style.CellPadding.y), GetColorU32(ImGuiCol_TextDisabled), empty ? "unselected" : preview.c_str());
    window->DrawList->PopClipRect();

    window->DrawList->AddRectFilledMultiColor(bb.Min + ImVec2(size.x - size.y - style.CellPadding.x, g.FontSize + style.ItemInnerSpacing.y), bb.Min + ImVec2(size.x - size.y, size.y + g.FontSize + style.ItemInnerSpacing.y), GetColorU32(Grad), GetColorU32(it_anim->second.Frame), GetColorU32(it_anim->second.Frame), GetColorU32(Grad));

    RenderFrameBorder(bb.Min + ImVec2(0, style.ItemInnerSpacing.y + g.FontSize), bb.Max, style.FrameRounding);

    RenderArrow(window->DrawList, bb.Min + ImVec2(size.x - height, g.FontSize + style.ItemInnerSpacing.y) + style.CellPadding, GetColorU32(it_anim->second.Text), ImGuiDir_Down, 1.0f);

    if (popup_open)
    {
        OpenPopup(std::string("MultiCombo_" + std::string(label)).c_str());
    }
    PushStyleVar(ImGuiStyleVar_WindowPadding, style.CellPadding);
    PushStyleVar(ImGuiStyleVar_ItemSpacing, style.CellPadding);
    if (BeginPopup(std::string("MultiCombo_" + std::string(label)).c_str(), ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize))
    {
        SetWindowPos(pos + ImVec2(0, height + GetFontSize() + style.ItemInnerSpacing.y + style.WindowPadding.y));
        SetWindowSize(ImVec2(size.x, 0));

        for (int i = 0; i < items.size(); i++)
        {
            if (ImAdd::Selectable(items[i], (*selected)[i] == 1, ImVec2(size.x - style.WindowPadding.x * 2, 0)))
            {
                (*selected)[i] = (*selected)[i] == 1 ? 0 : 1;
            }
        }

        EndPopup();
    }
    PopStyleVar(2);

    return pressed;
}

bool ImAdd::KeyBind(const char* str_id, int* k, float width)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    ImGuiIO& io = g.IO;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(str_id);

    ImVec2 pos = window->DC.CursorPos;
    float height = ImGui::GetFontSize();

    ImRect frame_bb(pos, pos + ImVec2(width, height + style.CellPadding.y * 2));
    ImRect total_bb(pos, frame_bb.Max);

    ImGui::ItemSize(total_bb);
    if (!ImGui::ItemAdd(total_bb, id))
        return false;

    const bool hovered = ImGui::ItemHoverable(frame_bb, id, 0);

    if (hovered)
    {
        ImGui::SetHoveredID(id);
        g.MouseCursor = ImGuiMouseCursor_Hand;
    }

    const bool user_clicked = hovered && io.MouseClicked[0];

    if (user_clicked)
    {
        if (g.ActiveId != id)
        {
            memset(io.MouseDown, 0, sizeof(io.MouseDown));
            memset(io.KeysDown, 0, sizeof(io.KeysDown));
            *k = 0;
        }
        ImGui::SetActiveID(id, window);
        ImGui::FocusWindow(window);
    }
    
    else if (io.MouseClicked[0])
    {
        if (g.ActiveId == id)
            ImGui::ClearActiveID();
    }
    

    bool value_changed = false;
    int key = *k;

    if (g.ActiveId == id)
    {
        if (!value_changed)
        {
            for (auto i = 0x08; i <= 0xA5; i++)
            {
                if (io.KeysDown[i])
                {
                    key = i;
                    value_changed = true;
                    ImGui::ClearActiveID();
                }
            }
        }

        if (ImGui::IsKeyPressed(ImGuiKey_Escape))
        {
            *k = 0;
            ImGui::ClearActiveID();
        }
        else *k = key;
    }

    window->DrawList->AddRectFilled(frame_bb.Min, frame_bb.Max, ImGui::GetColorU32(ImGuiCol_FrameBg), style.FrameRounding);

    const float border_size = g.Style.FrameBorderSize;
    if (border_size > 0.0f)
    {
        window->DrawList->AddRect(frame_bb.Min + ImVec2(1, 1), frame_bb.Max + ImVec2(1, 1), ImGui::GetColorU32(ImGuiCol_BorderShadow), style.FrameRounding, 0, border_size);
        window->DrawList->AddRect(frame_bb.Min, frame_bb.Max, ImGui::GetColorU32(ImGuiCol_Border), style.FrameRounding, 0, border_size);
    }

    ImGui::RenderNavHighlight(total_bb, id);

    char buf_display[32] = "None";

    if (*k != 0 && g.ActiveId != id)
        strcpy_s(buf_display, sizeof buf_display, szKeyNames[*k]);
    else if (g.ActiveId == id)
        strcpy_s(buf_display, sizeof buf_display, "...");

    ImVec2 buf_display_size = CalcTextSize(buf_display);

    window->DrawList->AddText(pos + ImVec2((frame_bb.Max.x - frame_bb.Min.x) / 2 - buf_display_size.x / 2, (GetFontSize() + style.CellPadding.y * 2) / 2 - buf_display_size.y / 2), GetColorU32(ImGuiCol_TextDisabled), buf_display);

    return value_changed;
}

bool ImAdd::ColorEdit4(const char* str_id, float col[4])
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImVec4 col_v4(col[0], col[1], col[2], col[3]);

    float height = ImGui::GetFontSize();
    float width = height * 2;

    ImGuiColorEditFlags flags = ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoSidePreview;

    //PushStyleColor(ImGuiCol_Border, ImVec4());
    PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
    bool result = ColorButton(str_id, col_v4, flags, ImVec2(width, height));
    PopStyleVar();
    //PopStyleColor();

    if (result)
    {
        OpenPopup(std::string(std::string(str_id) + "##Picker").c_str());
    }
    if (BeginPopup(std::string(std::string(str_id) + "##Picker").c_str()))
    {
        ColorPicker4(str_id, col, flags);
        EndPopup();
    }

    return result;
}

bool ImAdd::ColorEdit4(const char* str_id, Color_t *col)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImVec4 col_v4 = col->GetVec4();

    float height = ImGui::GetFontSize();
    float width = height * 2;

    ImGuiColorEditFlags flags = ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoSidePreview;

    //PushStyleColor(ImGuiCol_Border, ImVec4());
    PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
    bool result = ColorButton(str_id, col_v4, flags, ImVec2(width, height));
    PopStyleVar();
    //PopStyleColor();

    if (result)
    {
        OpenPopup(std::string(std::string(str_id) + "##Picker").c_str());
    }
    if (BeginPopup(std::string(std::string(str_id) + "##Picker").c_str()))
    {
        float arrColor[4];
        col->BaseAlpha(arrColor);
        if (ColorPicker4(str_id, &arrColor[0], flags))
            *col = Color_t::FromBase4(arrColor);
        EndPopup();
    }

    return result;
}