#pragma once
#include <unordered_map>
#include "fs/Source.h"
class c_globals {
public:
	bool active = true;
	bool scan_a_directory_btn = false;
	bool scan_current_directory = false;
	std::string path = GetExePath();
	bool scan_dir_proceed = false;
};

inline c_globals globals;