#include "ui.hh"
#include "../globals.hh"
#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include "../fs/Source.h"
#include <filesystem>


void ui::render() {

    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_WindowBg] = ImVec4(36.0f / 255.0f, 37.0f / 255.0f, 42.0f / 255.0f, 1.0f); // Example: change window background
    style.FrameRounding = 10.0f;
    style.Colors[ImGuiCol_Button] = ImVec4(0.2f, 0.5f, 0.7f, 1.0f);
    if (!globals.active) return;
    ImGui::SetNextWindowPos(ImVec2(window_pos.x, window_pos.y), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(window_size.x, window_size.y));
    ImGui::SetNextWindowBgAlpha(1.0f);
    ImGui::Begin(window_title, &globals.active, window_flags);
    {
                if (globals.scan_dir_proceed != true) {
                    if (globals.scan_a_directory_btn != true) {
                        ImGui::Text("This Tool is still in it's early state.");
                        ImGui::Text("Your path:"); ImGui::SameLine(); ImGui::TextColored(ImVec4(255, 0, 0, 255), globals.path.c_str());
                        if (ImGui::Button("Scan")) {
                            std::filesystem::remove_all("fs_data");
                            Sleep(1000);
                            globals.scan_a_directory_btn = true;
                        }
                    }
                    else
                        scan_dir();
                }
                else
                    scan_dir_proceed();
    }
    ImGui::End();
}

void ui::init(LPDIRECT3DDEVICE9 device) {
    dev = device;
	
    // colors
    ImGui::StyleColorsDark();

	if (window_pos.x == 0) {
		RECT screen_rect{};
		GetWindowRect(GetDesktopWindow(), &screen_rect);
		screen_res = ImVec2(float(screen_rect.right), float(screen_rect.bottom));
		window_pos = (screen_res - window_size) * 0.5f;

		// init images here
	}
}