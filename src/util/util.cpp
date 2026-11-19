#include "util.hpp"
#include <iostream>

namespace fs = std::filesystem;

/**
 * generally based on https://codeberg.org/dirs/directories-rs
 */

#ifdef IS_LINUX
#include <cstdlib>

bool PodUtil::startConfig() {
	static bool confReady = false;
	if (confReady) return true;

	const char *home = std::getenv("HOME");
	const char *cache = std::getenv("XDG_CACHE_HOME");
	const char *config = std::getenv("XDG_CONFIG_HOME");
	// error
	if (!home || !cache || !config) {
		std::cerr << "$XDG_*_HOME or $HOME environment variables not found" << std::endl;
		return false;
	}
	homeDir = home;

	const char *music = std::getenv("XDG_MUSIC_DIR");
	if (music) musicDir = fs::path(music);
	else musicDir = homeDir;

	cacheDir = fs::path(cache) / "poddy";
	if (!fs::exists(cacheDir)) {
		fs::create_directory(cacheDir);
	}

	configDir = fs::path(config) / "poddy";
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
bool PodUtil::startConfig() {
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

bool PodUtil::startConfig() {
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

using namespace PodUtil;

ErrQ::ErrQ(const char *str): msg(str), fatal(false) {}
ErrQ::ErrQ(std::string str): msg(str), fatal(false) {}
ErrQ::ErrQ(GError *&err): msg(std::to_string(err->code) + err->message), fatal(false) {
	g_error_free(err);
	err = nullptr;
}
ErrQ::ErrQ(const char *str, bool isFatal): msg(str), fatal(isFatal) {}
ErrQ::ErrQ(std::string str, bool isFatal): msg(str), fatal(isFatal) {}
ErrQ::ErrQ(GError *&err, bool isFatal): msg(std::to_string(err->code) + err->message), fatal(isFatal) {
	g_error_free(err);
	err = nullptr;
}
ErrQ::ErrQ(const ErrQ &err): errq(err.errq), msg(err.msg), fatal(err.fatal) {}

ErrorQueue::ErrorQueue(std::string nm): name(nm) {}
ErrorQueue::ErrorQueue(std::string nm, ErrorQueuePtr padre): name(nm), parent(padre) {}

void ErrorQueue::error(ErrQ err) {
	if (!err.errq) err.errq = getPtr();
	if (parent) parent->error(err);
	else queue.push_back(err);
}
void ErrorQueue::error(ErrQ err, bool fatal) {
	err.fatal = fatal;
	if (!err.errq) err.errq = getPtr();
	if (parent) parent->error(err);
	else queue.push_back(err);
}
void ErrorQueue::warn(ErrQ err) {
	err.warning = true;
	error(err);
}
void ErrorQueue::setParent(std::shared_ptr<ErrorQueue> errq) {
	parent = errq;
	if (!queue.empty()) {
		for (ErrQ &err : queue) {
			parent->error(err);
		}
	}
}

