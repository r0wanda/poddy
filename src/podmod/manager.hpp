#pragma once
#include <vector>
#include <string>
#include <memory>
#include <filesystem>
#include <podmod.hpp>
#include <settings.hpp>
#ifdef IS_LINUX
#include <dlfcn.h>
#endif

class Poddy;

struct ModManagerSettings {
	std::vector<std::string> manifests;
	std::vector<std::string> installed;
};
//NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ModManagerSettings, manifests, installed)

/*
struct PodModWrap {
	virtual PodModPtr get(std::shared_ptr<Poddy> pod) = 0;
};
#define WRAPPODMOD(name, base, ...) class name : public PodModWrap { virtual PodModPtr get(std::shared_ptr<Poddy> pod) { return std::make_shared<base>(pod, __VA_ARGS__); } };
*/

typedef void (*dypodmod_t)(Poddy*);
typedef void (*dypodmodclose_t)(PodMod*);
class DyPodMod;
// TODO change def of podmodptr
class DyPodModDeleter {
public:
	DyPodModDeleter(DyPodMod *mod) {
		dymod = mod;
	}
	void operator()([[ maybe_unused ]] PodMod *pod) {
		dymod->free();
	}
private:
	DyPodMod *dymod;
};
class DyPodMod {
public:
	DyPodMod(std::string path) {
		handle = dlopen(path.c_str(), RTLD_LAZY);
		if (!handle) {

		}
		dlerror();
		init = (dypodmod_t)dlsym(handle, "create_object");
		close = (dypodmodclose_t)dlsym(handle, "free_object");
	}
	PodModPtr wrap(std::shared_ptr<Poddy> pod) {
		std::shared_ptr<PodMod, DyPodModDeleter> ptr
	}
	void free() {
		close(rawPtr);
		dlclose(handle);
	}
	~DyPodMod() {
		close(rawPtr);
	}
private:
	dypodmod_t init;
	dypodmodclose_t close;
	PodMod *rawPtr;
	PodModPtr sharedPtr;
	void *handle;
}

class ModManager : public PodSettingsMod {
public:
	ModManager
};
