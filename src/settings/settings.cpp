#include "settings.hpp"

namespace fs = std::filesystem;

PodSettings::PodSettings(std::string _name): IndepPodMod(_name) {
	settingsPath = configPath / name / ".json";
	if (!fs::exists(settingsPath)) {}
}

PodSettingsMod::PodSettingsMod(std::string _name, std::shared_ptr<Poddy> pod): PodSettings(_name), PodMod(_name, pod) {}
