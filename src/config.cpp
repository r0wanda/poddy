#include "config.hpp"
#include <iostream>

namespace fs = std::filesystem;

#ifdef IS_LINUX
#include <cstdlib>
bool startConfig() {
	if (std::getenv("XDG_CACHE_HOME") != NULL) {
		cacheDir = fs::path(std::getenv("XDG_CACHE_HOME")) / "poddy";
		if (!fs::exists(cacheDir)) {
			fs::create_directory(cacheDir);
		}
	} else goto sConfigE;
	if (std::getenv("XDG_CONFIG_HOME") != NULL) {
		configDir = fs::path(std::getenv("XDG_CONFIG_HOME")) / "poddy";
		if (!fs::exists(configDir)) {
			fs::create_directory(configDir);
		}
	} else goto sConfigE;
	confReady = true;
	return true;
	sConfigE:;
	std::cerr << "$XDG_*_HOME environment variables not found" << std::endl;
	return false;
}

#endif
