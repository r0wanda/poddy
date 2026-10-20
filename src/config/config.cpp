#include "config.hpp"
#include <iostream>

namespace fs = std::filesystem;

#ifdef IS_LINUX
#include <cstdlib>
bool startConfig() {
	static bool confReady = false;
	if (confReady) return true;
	if (std::getenv("XDG_CACHE_HOME") != NULL && std::getenv("XDG_CONFIG_HOME") != NULL) {
		cacheDir = fs::path(std::getenv("XDG_CACHE_HOME")) / "poddy";
		if (!fs::exists(cacheDir)) {
			fs::create_directory(cacheDir);
		}

		configDir = fs::path(std::getenv("XDG_CONFIG_HOME")) / "poddy";
		if (!fs::exists(configDir)) {
			fs::create_directory(configDir);
		}
		confReady = true;
		return true;
	}

	// error 
	std::cerr << "$XDG_*_HOME environment variables not found" << std::endl;
	return false;
}

#endif
