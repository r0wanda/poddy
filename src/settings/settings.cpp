#include "settings.hpp"

PodSettings::PodSettings(std::string _name): IndepPodMod(_name) {
	settingsPath = configPath / name / ".json";
}

PodSettingsMod::PodSettingsMod(std::string _name, std::shared_ptr<Poddy> pod): PodSettings(_name), PodMod(_name, pod) {}
