#include "IsaacRepentance.h"
#include "ImGuiFeatures/LogViewer.h"

#define MINI_CASE_SENSITIVE
#include "ini.h"

#include <filesystem>

inline int defstoi(const std::string& str, int default) {
	try {
		return std::stoi(str);
	}
	catch (...) {
	}
	return default;
}

struct REPENTOGONOptions {
	void Init() {
		optionsPath = std::string((char*)&g_SaveDataPath) + "Repentogon/options.ini";
		std::string directory = optionsPath.substr(0, optionsPath.find_last_of("\\/"));

		if (!CreateDirectory(directory.c_str(), NULL)) {
			if (GetLastError() != ERROR_ALREADY_EXISTS) {
				logViewer.AddLog("[REPENTOGON]", "Error creating Repentogon Save directory\n");
				return;
			}
		}
		mINI::INIFile iniFile(optionsPath);

		if (!std::filesystem::exists(optionsPath)) {
			printf("INI doesn't exist, creating\n");
			
			ini["VanillaTweaks"]["BetterVoidGeneration"] = "1";
			ini["VanillaTweaks"]["HushPanicStateFix"] = "1";
			ini["VanillaTweaks"]["KeyMasterDealChance"] = "1";
			ini["VanillaTweaks"]["PreventModUpdates"] = "0";
			ini["internal"]["DidModReset"] = "0";

			iniFile.generate(ini, true);
		}

		iniFile.read(ini);

		betterVoidGeneration = defstoi(ini["VanillaTweaks"]["BetterVoidGeneration"],1);
		hushPanicStateFix = defstoi(ini["VanillaTweaks"]["HushPanicStateFix"],1);
		keyMasterDealChance = defstoi(ini["VanillaTweaks"]["KeyMasterDealChance"],1);
		preventModUpdates = defstoi(ini["VanillaTweaks"]["PreventModUpdates"],0);

		printf("Loaded REPENTOGON INI\n");
	}

	bool GetBool(char* category, char* field) {
		mINI::INIFile iniFile(optionsPath);
		return ini[category][field] == "1" ? true : false;
	}

	bool Write(char* category, char* field, char* value) {
		mINI::INIFile iniFile(optionsPath);
		ini[category][field] = value;
		return iniFile.write(ini);
	}

	mINI::INIStructure ini;
	bool betterVoidGeneration;
	bool hushPanicStateFix;
	bool keyMasterDealChance;
	bool preventModUpdates;
	std::string optionsPath;
};

extern REPENTOGONOptions repentogonOptions;