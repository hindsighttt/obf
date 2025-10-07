#pragma once

#include "../../imgui.h"
#include "../../imgui_internal.h"
#include "../../../../cstrike/sdk/datatypes/color.h"

#include <vector>

// Configuration
#define IMADD_ANIMATIONS_SPEED 0.05f // Second

namespace ImAdd
{
    // Helpers
    ImVec4  HexToColorVec4(unsigned int hex_color, float alpha);

    // DoubleText
    void    DoubleText(ImVec4 color1, ImVec4 color2, const char* label1, const char* label2);

    // Separator
    void    SeparatorText(const char* label, float thickness = 1.0f);
    void    VSeparator(float margin = 0.0f, float thickness = 1.0f);

    // RadioFrame
    bool    RadioFrame(const char* label, int* v, int radio_id, const ImVec2& size = ImVec2(0, 0));
	bool    RadioFrameIcon(ImTextureID icon_texture, const char* label, int* v, int radio_id, const ImVec4& gradient_top, const ImVec4& gradient_bottom, const ImVec2& size_arg = ImVec2(0, 0));

    // Button
    bool    Button(const char* label, const ImVec2& size_arg = ImVec2(0, 0), ImGuiButtonFlags button_flags = 0, ImDrawFlags draw_flags = 0);
	bool	ButtonCircle(const char* str_id, float radius);

    // Toggle
    bool    CheckBox(const char* label, bool* v);
	bool    ToggleButtonClassic(const char* label, bool* v);
	bool    ToggleButton(const char* label, bool* v, float width = -0.1f);

	// Custom Child
	void    BeginChild(const char* str_id, const ImVec2& size = ImVec2(0, 0), bool border = true);
	void    EndChild();

	// Inputs
	bool    InputText(ImTextureID icon_texture, const char* label, char* buf, size_t buf_size, ImGuiInputTextFlags flags = 0);

	// Label
	bool    Label(const char* label, const ImVec2& size = ImVec2(0, 0), const ImVec2& text_align = ImVec2(0, 0));

    // Slider
    bool    SliderScalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format = NULL, ImGuiSliderFlags flags = 0);
    bool    SliderFloat(const char* label, float* v, float v_min, float v_max, const char* format = "%.1f", ImGuiSliderFlags flags = 0);
    bool    SliderAngle(const char* label, float* v_rad, float v_degrees_min = -360.0f, float v_degrees_max = +360.0f, const char* format = "%.0f deg", ImGuiSliderFlags flags = 0);
    bool    SliderInt(const char* label, int* v, int v_min, int v_max, const char* format = "%d", ImGuiSliderFlags flags = 0);

    bool    Selectable(const char* label, bool selected = false, const ImVec2& size_arg = ImVec2(0, 0));
	bool    SelectableLabel(ImDrawList* drawlist, const char* label, bool selected = false, const ImVec2& size_arg = ImVec2(0, 0));
	bool    Combo(const char* str_id, int* current_item, const char* items_separated_by_zeros);
	bool    Combo(const char* str_id, int* current_item, const char* items_separated_by_zeros, float width);
    bool    MultiCombo(const char* label, std::vector<int>* selected, std::vector<const char*> items, float width = -0.1f);

    bool	KeyBind(const char* str_id, int* k, float width);
	bool    ColorEdit4(const char* str_id, float col[4]);
	bool	ColorEdit4(const char* str_id, Color_t* col);
}

const char* const szKeyNames[] =
{
	"Unknown",
	"LBUTTON",
	"RBUTTON",
	"CANCEL",
	"MBUTTON",
	"XBUTTON1",
	"XBUTTON2",
	"Unknown",
	"BACK",
	"TAB",
	"Unknown",
	"Unknown",
	"CLEAR",
	"RETURN",
	"Unknown",
	"Unknown",
	"SHIFT",
	"CONTROL",
	"MENU",
	"PAUSE",
	"CAPITAL",
	"KANA",
	"Unknown",
	"JUNJA",
	"FINAL",
	"KANJI",
	"Unknown",
	"ESCAPE",
	"CONVERT",
	"NONCONVERT",
	"ACCEPT",
	"MODECHANGE",
	"SPACE",
	"PRIOR",
	"NEXT",
	"END",
	"HOME",
	"LEFT",
	"UP",
	"RIGHT",
	"DOWN",
	"SELECT",
	"PRINT",
	"EXECUTE",
	"SNAPSHOT",
	"INSERT",
	"DELETE",
	"HELP",
	"NUMPAD 0",
	"NUMPAD 1",
	"NUMPAD 2",
	"NUMPAD 3",
	"NUMPAD 4",
	"NUMPAD 5",
	"NUMPAD 6",
	"NUMPAD 7",
	"NUMPAD 8",
	"NUMPAD 9",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"KEY A",
	"KEY B",
	"KEY C",
	"KEY D",
	"KEY E",
	"KEY F",
	"KEY G",
	"KEY H",
	"KEY I",
	"KEY J",
	"KEY K",
	"KEY L",
	"KEY M",
	"KEY N",
	"KEY O",
	"KEY P",
	"KEY Q",
	"KEY R",
	"KEY S",
	"KEY T",
	"KEY U",
	"KEY V",
	"KEY W",
	"KEY X",
	"KEY Y",
	"KEY Z",
	"LWIN",
	"RWIN",
	"APPS",
	"Unknown",
	"SLEEP",
	"NUMPAD 0",
	"NUMPAD 1",
	"NUMPAD 2",
	"NUMPAD 3",
	"NUMPAD 4",
	"NUMPAD 5",
	"NUMPAD 6",
	"NUMPAD 7",
	"NUMPAD 8",
	"NUMPAD 9",
	"MULTIPLY",
	"ADD",
	"SEPARATOR",
	"SUBTRACT",
	"DECIMAL",
	"DIVIDE",
	"F1",
	"F2",
	"F3",
	"F4",
	"F5",
	"F6",
	"F7",
	"F8",
	"F9",
	"F10",
	"F11",
	"F12",
	"F13",
	"F14",
	"F15",
	"F16",
	"F17",
	"F18",
	"F19",
	"F20",
	"F21",
	"F22",
	"F23",
	"F24",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"NUMLOCK",
	"SCROLL",
	"OEM_NEC_EQUAL",
	"OEM_FJ_MASSHOU",
	"OEM_FJ_TOUROKU",
	"OEM_FJ_LOYA",
	"OEM_FJ_ROYA",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"LSHIFT",
	"RSHIFT",
	"LCONTROL",
	"RCONTROL",
	"LMENU",
	"RMENU"
};
