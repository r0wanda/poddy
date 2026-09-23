#include "podmod.hpp"
#include "../config.hpp"
namespace fs = std::filesystem;

PodMod::PodMod(std::string _name): name(_name) {
	if (!confReady) startConfig();
	configPath = configDir / name;
	cachePath = cacheDir / name;
	if (!fs::exists(configPath)) fs::create_directory(configPath);
	if (!fs::exists(cachePath)) fs::create_directory(cachePath);
}
