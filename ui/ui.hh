#pragma once
#include <d3d9.h>
#include "../imgui/imgui.h"
#include <string>

namespace ui {
	void init(LPDIRECT3DDEVICE9);
	void render();
}

namespace ui {
	inline LPDIRECT3DDEVICE9 dev;
	inline const char* window_title = "File Scanner v2.0.1 alpha";
}

namespace ui {
	inline ImVec2 screen_res{ 000, 000 };
	inline ImVec2 window_pos{ 100, 120 };
	inline ImVec2 window_size{ 500, 500 };
	inline DWORD  window_flags = ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
}

// Expose directory dialog for UI
std::string select_directory_dialog(const std::string& initial_path = "");