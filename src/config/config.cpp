#include "config.hpp"
#include <iostream>

namespace fs = std::filesystem;

/**
 * generally based on https://codeberg.org/dirs/directories-rs
 */

#ifdef IS_LINUX
#include <cstdlib>

bool startConfig() {
	static bool confReady = false;
	if (confReady) return true;

	// error
	if (std::getenv("XDG_CACHE_HOME") == NULL || std::getenv("XDG_CONFIG_HOME") == NULL) {
		std::cerr << "$XDG_*_HOME environment variables not found" << std::endl;
		return false;
	}

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

#elifdef IS_WINDOWS
#include <windows.h>
#include <shlobj.h>

// https://stackoverflow.com/a/62314965
bool startConfig() {
	static bool confReady = false;
	if (confReady) return true;

    PWSTR configTmp;
	PWSTR cacheTmp;
    HRESULT configRes = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &configTmp);
	HRESULT cacheRes = SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &cacheTmp);

    if (configRes != S_OK) {
        CoTaskMemFree(configTmp);
		std::cerr << "FOLDERID_RoamingAppData not found" << std::endl;
    }
	if (cacheRes != S_OK) {
        CoTaskMemFree(cacheTmp);
		std::cerr << "FOLDERID_RoamingAppData not found" << std::endl;
    }
	if (configRes != S_OK || cacheRes != S_OK) return false;

    configDir = fs::path(configTmp) / "poddy";
	CoTaskMemFree(configTmp);
	if (!fs::exists(configDir)) {
		fs::create_directory(configDir);
	}
	cacheDir = fs::path(cacheTmp) / "poddy";
	CoTaskMemFree(cacheTmp);
	if (!fs::exists(cacheDir)) {
		fs::create_directory(cacheDir);
	}

	confReady = true;
    return true;
}

#elifdef IS_MACOS
#include <cstdlib>

bool startConfig() {
	static bool confReady = false;
	if (confReady) return true;

	fs::path home = std::getenv("HOME");

	if (home == NULL) {
		std::cerr << "HOME directory not found" << std::endl;
		return false;
	}
	cacheDir = home / "Library" / "Caches" / "poddy";
	if (!fs::exists(cacheDir)) {
		fs::create_directory(cacheDir);
	}

	configDir = home / "Library" / "Application Support" / "poddy";
	if (!fs::exists(configDir)) {
		fs::create_directory(configDir);
	}

	confReady = true;
	return true;
}
#endif
