#include <iostream>
#include "../ui/ui.hh"
#include "../globals.hh"
#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include <filesystem>
#include "Dirent.h"
#include <Windows.h>
#include <string>
#include <fstream>
#include <random>
namespace fs = std::filesystem;
int times_checked = 0;
int i = 0;
int alert_amount = 0;
int save_changes_old_count = 0;
struct {
public:
	std::string new_name = "";
	std::string old_name = "";
	std::string old_size = "";
	std::string new_size = "";
	std::string oldid = "";
	std::string newid = "";
}file;

std::string GetExeFileName()
{
	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	return std::string(buffer);
}
void add_alert(std::string text) {
	std::ofstream alert_file;
	alert_file.open("fs_data//alerts.txt");
	alert_file << "[alert] " << text;
}
std::string GetExePath()
{
	std::string f = GetExeFileName();
	return f.substr(0, f.find_last_of("\\/"));
}

void scan_dir_proceed() {
	ImGui::Text("Directory Scan completed.");
}
void coloredText() {
	ImGui::Text("["); ImGui::SameLine(); ImGui::TextColored(ImVec4(100, 0, 15, 150), "*"); ImGui::SameLine(); ImGui::Text("]"); ImGui::SameLine();
}
void save_changes_old() {
	save_changes_old_count++;
	DIR* di;
	struct dirent* dir;
	di = opendir(globals.path.c_str()); //specify the directory name
	if (di)
	{
		while ((dir = readdir(di)) != NULL)
		{
			std::string d_name = dir->d_name;
			std::fstream view_new;
			std::ofstream save_old;
			view_new.open("fs_data//" + d_name + "_data.txt",std::ios::in);
			save_old.open("fs_data//old_data//" + d_name + "_data.txt");
			if (!view_new.is_open()) {
			//todo error msg
			}
			else {
				int current_line = 0;
				std::string line;       			
				while (getline(view_new, line))
				{
					if (current_line == 0) {
					//name line;
						save_old << line;
					}
					if (current_line == 1) {
						//uid line;
						save_old << "\n" << line;
					}
					if (current_line == 2) {
						//size line;
						save_old<< "\n" << line;
					}
					current_line++;
				}
				save_old.close();
			}
		}
	}

}
void view_changes() {

}
void check_for_changes() {
	std::ofstream add_alerts;
	add_alerts.open("fs_data//alerts.txt");
	DIR* di;
	struct dirent* dir;
	di = opendir(globals.path.c_str()); //specify the directory name
	if (di)
	{
		while ((dir = readdir(di)) != NULL)
		{
			std::string d_name = dir->d_name;
			std::fstream compare_new;
			std::fstream compare_old;
			compare_new.open("fs_data//" + d_name + "_data.txt");
			compare_old.open("fs_data//old_data//" + d_name + "_data.txt");	
					int current_line_old = 0;
					int current_line_new = 0;
					std::string new_line;
					std::string old_line;
					while (getline(compare_new, new_line)) {
						if (current_line_new == 0) { // name line
							file.new_name = new_line; //storing the variables
						}
						if (current_line_new == 2) { // size line
							file.new_size = new_line;
						}
						if (current_line_new == 1) { // uid line
							file.newid = new_line;
						}
						current_line_new++;
					}
					compare_new.close();
					while (getline(compare_old, old_line)) {
						if (current_line_old == 0) {
							file.old_name = old_line;
						}
						if (current_line_old == 2) {
							file.old_size = old_line;
						}
						if (current_line_old == 1) {
							file.oldid = old_line;
						}
						current_line_old++;
					}
					compare_old.close();
					if (file.new_name != file.old_name && file.new_size != file.old_size && file.newid != file.oldid) {
						ImGui::TextColored(ImVec4(255, 0, 0, 255), "[*] New file (%s)", file.new_name.c_str());
					}
					else {
						if (file.new_name != file.old_name) {
							ImGui::TextColored(ImVec4(255, 0, 0, 255), "[*] File Name changed from %s to %s", file.old_name.c_str(), file.new_name.c_str());
						}
					}
					if (file.new_size != file.old_size) {
						ImGui::TextColored(ImVec4(255, 0, 0, 255), "[*] (%s) File Size changed from %s Bytes to %s Bytes ", file.new_name.c_str(), file.old_size.c_str(), file.new_size.c_str());
					}
					if (file.newid != file.oldid) {
						ImGui::TextColored(ImVec4(255, 0, 0, 255), "[*] (%s) FileID changed from %s to %s", file.new_name.c_str(), file.oldid.c_str(), file.newid.c_str());
					}
		}
	}

}
void scan_dir() {
	std::ofstream save_locally;
	CreateDirectory("fs_data", NULL);
	CreateDirectory("fs_data//old_data", NULL);
	ImGui::Text("Scanning: %s", globals.path.c_str());
	ImGui::Text("%d times checked", times_checked);
	ImGui::Text("Alerts: %d", alert_amount);
	ImGui::Text("Debug: %d", save_changes_old_count);
    DIR* di;
    struct dirent* dir;
    di = opendir(globals.path.c_str()); //specify the directory name
    if (di)
    {
		
			while ((dir = readdir(di)) != NULL)
			{
				std::ifstream in_file(dir->d_name, std::ios::binary);
				in_file.seekg(0, std::ios::end);
				int file_size = in_file.tellg();

				coloredText();
				ImGui::Selectable(dir->d_name);


				std::string file = dir->d_name;
				save_locally.open("fs_data//" + file + "_data.txt");
				int file_id = file_size * 3 + rand() % 1 + 50;
				save_locally << dir->d_name << "\n" << file_id << "\n" << file_size;
				save_locally.close();
				if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
				{
					in_file.close();
					ImGui::Text("%s info:", dir->d_name);
					ImGui::Text("type: %d", dir->d_type);
					ImGui::Text("File Position: %ld", dir->d_off);
					ImGui::Text("File Size: %d Bytes", file_size);
					if (ImGui::Button("Close"))
						ImGui::CloseCurrentPopup();
					ImGui::EndPopup();

				}

				times_checked++;
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(dir->d_name);
				if (dir->d_type != 32768) {
					ImGui::SameLine();
					ImGui::TextColored(ImVec4(255, 0, 0, 255), "<- folder/unkown");
				}
			}


        closedir(di);
		ImGui::Text("==============CHANGES==============");
		if (save_changes_old_count == 0) {
			save_locally.close();
			save_changes_old();
		}
		else {
			save_locally.close();
			check_for_changes();
		}
    }
	
		if (ImGui::Button("Stop")) {
			globals.scan_a_directory_btn = false;
		}
}