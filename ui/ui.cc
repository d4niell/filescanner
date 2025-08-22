#include "ui.hh"
#include "../globals.hh"
#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include "../fs/Source.h"
#include <filesystem>

extern std::string select_directory_dialog(const std::string& initial_path);

void ui::render() {
    // Modern, vibrant theme
    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.15f, 0.20f, 1.0f); // Deep blue-gray
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.12f, 0.18f, 1.0f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.18f, 0.22f, 0.32f, 1.0f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.18f, 0.52f, 0.36f, 1.0f); // Green
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.22f, 0.66f, 0.44f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.16f, 0.40f, 0.28f, 1.0f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.18f, 0.52f, 0.36f, 1.0f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.22f, 0.66f, 0.44f, 1.0f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.16f, 0.40f, 0.28f, 1.0f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.18f, 0.24f, 1.0f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.18f, 0.22f, 0.32f, 1.0f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.22f, 0.66f, 0.44f, 1.0f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.22f, 0.66f, 0.44f, 1.0f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.22f, 0.66f, 0.44f, 1.0f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.18f, 0.52f, 0.36f, 1.0f);
    style.Colors[ImGuiCol_Separator] = ImVec4(0.10f, 0.12f, 0.18f, 1.0f);
    style.Colors[ImGuiCol_Text] = ImVec4(0.95f, 0.97f, 1.0f, 1.0f); // Bright text
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.55f, 0.60f, 1.0f);
    style.FrameRounding = 8.0f;
    style.WindowRounding = 8.0f;
    style.FrameBorderSize = 1.0f;
    style.WindowBorderSize = 1.0f;
    if (!globals.active) return;
    ImGui::SetNextWindowPos(ImVec2(window_pos.x, window_pos.y), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(window_size.x, window_size.y));
    ImGui::SetNextWindowBgAlpha(1.0f);
    ImGui::Begin(window_title, &globals.active, window_flags);
    {
        if (ImGui::Button("Select Directory to Scan")) {
            std::string selected = select_directory_dialog(globals.path);
            if (!selected.empty()) {
                globals.path = selected;
            }
        }
        ImGui::SameLine();
        ImGui::Text("Current: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(0.22f, 0.66f, 0.44f, 1.0f), globals.path.c_str());
        if (globals.scan_dir_proceed != true) {
            if (globals.scan_a_directory_btn != true) {
                ImGui::Text("This Tool is still in its early state.");
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
    ImGui::StyleColorsDark();
    if (window_pos.x == 0) {
        RECT screen_rect{};
        GetWindowRect(GetDesktopWindow(), &screen_rect);
        screen_res = ImVec2(float(screen_rect.right), float(screen_rect.bottom));
        window_pos = (screen_res - window_size) * 0.5f;
        // init images here
    }
}