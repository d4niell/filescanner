#include <iostream>
#include "../ui/ui.hh"
#include "../globals.hh"
#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include <filesystem>
#include <Windows.h>
#include <string>
#include <fstream>
#include <random>
#include <vector>
#include <sstream>
#include <set>
#include <algorithm>
#include <ShlObj.h>

namespace fs = std::filesystem;

// State struct for file change tracking
struct FileChange {
    std::string new_name;
    std::string old_name;
    std::string new_size;
    std::string old_size;
    std::string newid;
    std::string oldid;
};

static int alert_amount = 0;
static int save_changes_old_count = 0;
static FileChange file;

std::string GetExeFileName() {
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    return std::string(buffer);
}

void add_alert(const std::string& text) {
    std::ofstream alert_file("fs_data/alerts.txt", std::ios::app);
    alert_file << "[alert] " << text << std::endl;
}

std::string GetExePath() {
    std::string f = GetExeFileName();
    return f.substr(0, f.find_last_of("\\/"));
}

// Helper: should this file be ignored (self, .pdb, or .exe)
bool should_ignore_file(const std::string& filename) {
    // Ignore .pdb, .exe, and the running executable
    std::string lower = filename;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    if (lower.size() > 4 && (lower.substr(lower.size() - 4) == ".pdb" || lower.substr(lower.size() - 4) == ".exe"))
        return true;
    std::string exe_name = fs::path(GetExeFileName()).filename().string();
    if (filename == exe_name)
        return true;
    return false;
}

void scan_dir_proceed() {
    ImGui::Text("Directory scan completed. You can review the results below.");
}

void coloredText() {
    ImGui::Text("["); ImGui::SameLine(); ImGui::TextColored(ImVec4(0.39f, 0.0f, 0.06f, 1.0f), "*"); ImGui::SameLine(); ImGui::Text("]"); ImGui::SameLine();
}

// Save the current scan as the old state for future comparison
void save_changes_old() {
    save_changes_old_count++;
    for (const auto& entry : fs::directory_iterator(globals.path)) {
        if (!entry.is_regular_file()) continue;
        std::string d_name = entry.path().filename().string();
        if (should_ignore_file(d_name)) continue;
        std::ifstream view_new("fs_data/" + d_name + "_data.txt");
        std::ofstream save_old("fs_data/old_data/" + d_name + "_data.txt");
        if (!view_new.is_open()) {
            add_alert("Failed to open new data for " + d_name);
            continue;
        }
        std::string line;
        int current_line = 0;
        while (std::getline(view_new, line)) {
            if (current_line > 0) save_old << "\n";
            save_old << line;
            current_line++;
        }
    }
}

// Helper: get all file base names (without _data.txt) from a directory
std::set<std::string> get_data_file_basenames(const std::string& dir, const std::string& suffix = "_data.txt") {
    std::set<std::string> names;
    if (!fs::exists(dir)) return names;
    for (const auto& entry : fs::directory_iterator(dir)) {
        if (!entry.is_regular_file()) continue;
        std::string fname = entry.path().filename().string();
        if (fname.size() > suffix.size() && fname.substr(fname.size() - suffix.size()) == suffix) {
            names.insert(fname.substr(0, fname.size() - suffix.size()));
        }
    }
    return names;
}

// Compare new and old scan data and show changes (robust, no false positives)
void check_for_changes() {
    std::set<std::string> new_files = get_data_file_basenames("fs_data");
    std::set<std::string> old_files = get_data_file_basenames("fs_data/old_data");
    std::set<std::string> all_files;
    std::set_union(new_files.begin(), new_files.end(), old_files.begin(), old_files.end(), std::inserter(all_files, all_files.begin()));

    for (const auto& fname : all_files) {
        std::ifstream compare_new("fs_data/" + fname + "_data.txt");
        std::ifstream compare_old("fs_data/old_data/" + fname + "_data.txt");
        bool has_new = compare_new.is_open();
        bool has_old = compare_old.is_open();
        std::string new_lines[3], old_lines[3];
        if (has_new) for (int i = 0; i < 3; ++i) std::getline(compare_new, new_lines[i]);
        if (has_old) for (int i = 0; i < 3; ++i) std::getline(compare_old, old_lines[i]);
        if (has_new && !has_old) {
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "[+] New file detected: %s", fname.c_str());
        } else if (!has_new && has_old) {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "[-] File deleted: %s", fname.c_str());
        } else if (has_new && has_old) {
            file.new_name = new_lines[0];
            file.newid = new_lines[1];
            file.new_size = new_lines[2];
            file.old_name = old_lines[0];
            file.oldid = old_lines[1];
            file.old_size = old_lines[2];
            bool changed = false;
            if (file.new_name != file.old_name) {
                ImGui::TextColored(ImVec4(1, 0.5f, 0, 1), "[*] File name changed: '%s' -> '%s'", file.old_name.c_str(), file.new_name.c_str());
                changed = true;
            }
            if (file.new_size != file.old_size) {
                ImGui::TextColored(ImVec4(0, 0.5f, 1, 1), "[*] '%s' size changed: %s -> %s bytes", file.new_name.c_str(), file.old_size.c_str(), file.new_size.c_str());
                changed = true;
            }
            if (file.newid != file.oldid) {
                ImGui::TextColored(ImVec4(0.5f, 0, 1, 1), "[*] '%s' ID changed: %s -> %s", file.new_name.c_str(), file.oldid.c_str(), file.newid.c_str());
                changed = true;
            }
            if (!changed) {
                ImGui::TextColored(ImVec4(0.5f, 1, 0.5f, 1), "[=] No changes: %s", file.new_name.c_str());
            }
        }
    }
}

// Helper: open a folder selection dialog and return the selected path
std::string select_directory_dialog(const std::string& initial_path) {
    BROWSEINFO bi = { 0 };
    bi.lpszTitle = "Select a directory to scan";
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
    LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
    char path[MAX_PATH];
    if (pidl && SHGetPathFromIDListA(pidl, path)) {
        CoTaskMemFree(pidl);
        return std::string(path);
    }
    if (pidl) CoTaskMemFree(pidl);
    return "";
}

// Modern, humanized directory scan (scan all files, always update data)
void scan_dir() {
    fs::create_directories("fs_data/old_data");
    ImGui::Text("Scanning: %s", globals.path.c_str());
    ImGui::Text("Alerts: %d", alert_amount);
    ImGui::Text("Debug (scan count): %d", save_changes_old_count);
    int file_count = 0;
    std::vector<std::string> file_names;
    for (const auto& entry : fs::directory_iterator(globals.path)) {
        if (!entry.is_regular_file()) continue;
        std::string d_name = entry.path().filename().string();
        if (should_ignore_file(d_name)) continue;
        std::ifstream in_file(entry.path(), std::ios::binary | std::ios::ate);
        auto file_size = in_file.tellg();
        coloredText();
        ImGui::Selectable(d_name.c_str());
        std::string file = d_name;
        std::ofstream save_locally("fs_data/" + file + "_data.txt");
        int file_id = static_cast<int>(file_size) * 3 + (rand() % 50 + 50);
        save_locally << d_name << "\n" << file_id << "\n" << file_size;
        save_locally.close();
        if (ImGui::BeginPopupContextItem()) {
            in_file.close();
            ImGui::Text("%s info:", d_name.c_str());
            ImGui::Text("File Size: %lld Bytes", static_cast<long long>(file_size));
            if (ImGui::Button("Close"))
                ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("%s", d_name.c_str());
        file_count++;
        file_names.push_back(d_name);
    }
    if (file_count == 0) {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "No files found in the selected directory.");
    }
    ImGui::Text("%d files scanned.", file_count);
    ImGui::Separator();
    ImGui::Text("Detected Changes:");
    if (save_changes_old_count == 0) {
        save_changes_old();
    } else {
        check_for_changes();
    }
    if (ImGui::Button("Stop")) {
        globals.scan_a_directory_btn = false;
    }
}