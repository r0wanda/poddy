#include "config.hpp"

namespace fs = std::filesystem;

#ifdef IS_LINUX
#include <cstdlib>
bool startConfig() {
	if (std::getenv("XDG_CACHE_HOME") != NULL) {
		cacheDir = fs::path(std::getenv("XDG_CACHE_HOME")) / "poddy";
		if (!fs::exists(cacheDir)) {
			fs::create_directory(cacheDir);
		}
	} else return false;
	if (std::getenv("XDG_CONFIG_HOME") != NULL) {
		configDir = fs::path(std::getenv("XDG_CONFIG_HOME")) / "poddy";
		if (!fs::exists(configDir)) {
			fs::create_directory(configDir);
		}
	} else return false;
	return true;
}
#endif
