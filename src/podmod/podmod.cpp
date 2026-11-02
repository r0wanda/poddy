#include "podmod.hpp"
#include <poddy.hpp>
#include <config.hpp>
#include <tui.hpp>
namespace fs = std::filesystem;

IndepPodMod::IndepPodMod(std::string _name): name(_name) {
	startConfig();
	configPath = configDir / name;
	cachePath = cacheDir / name;
	if (!fs::exists(configPath)) fs::create_directory(configPath);
	if (!fs::exists(cachePath)) fs::create_directory(cachePath);
}


PodMod::PodMod(std::string _name, PoddyPtr pod): IndepPodMod::IndepPodMod(_name), poddy(pod) {}
