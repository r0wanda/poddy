#pragma once
#include <vector>
#include <string>
#include <filesystem>
#include "podmod.hpp"
#include "../settings/settings.hpp"
#ifdef IS_LINUX
#include <dlfcn.h>
#endif

struct ModManagerSettings {
	std::vector<std::string> manifests;
	std::vector<std::string> installed;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ModManagerSettings, manifests, installed)

class DyPodMod {
public:
	DyPodMod(std::string path) {
		handle = dlopen(path.c_str(), RTLD_LAZY);
		if (!handle) //err
		dlerror();
		init = dlsym(handle, "create_object");
	}
private:
	int (*init)(Poddy*);
	PodMod *rawPtr;
	void *handle;
}

class ModManager : public PodSettings {
public:
	ModManager
};
