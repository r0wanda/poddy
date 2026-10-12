#include "settings.hpp"

PodSettings::PodSettings(std::string _name): IndepPodMod(_name) {
	settingsPath = configPath / name / ".json";
}
