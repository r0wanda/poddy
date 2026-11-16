#pragma once
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <filesystem>
#include <util.hpp>
#include "podmod.hpp"
#include <settings.hpp>
#ifdef IS_LINUX
#include <dlfcn.h>
#endif

class Poddy;

struct ModManagerSettings {
	std::vector<std::string> manifests;
	std::vector<std::string> installed;
};

/*
struct PodModWrap {
	virtual PodModPtr get(std::shared_ptr<Poddy> pod) = 0;
};
#define WRAPPODMOD(name, base, ...) class name : public PodModWrap { virtual PodModPtr get(std::shared_ptr<Poddy> pod) { return std::make_shared<base>(pod, __VA_ARGS__); } };
*/

typedef PodMod* (*dypodmod_t)(Poddy*);
typedef void (*dypodmodclose_t)(PodMod*);

class DyPodMod : PodUtil::GetPtr<DyPodMod> {
public:
	DyPodMod(std::string path, std::shared_ptr<Tui> tui);
	PodModPtr get(std::shared_ptr<Poddy> pod);
	void free();
	~DyPodMod();
	PodUtil::ErrorQueuePtr errq;
private:
	dypodmod_t init;
	dypodmodclose_t close;
	PodMod *rawPtr;
	PodModPtr sharedPtr;
	void *handle;
};
typedef std::shared_ptr<DyPodMod> DyPodModPtr;

// def. in modmanager.json
extern const nlohmann::json modmanager;
typedef std::function<bool(const std::string&, const std::string&)> _PodModComp;
class ModManager : public PodSettings {
public:
	ModManager(std::shared_ptr<Poddy> pod, SettingsIfacePtr siface);
	void loadExternal(std::string path);
	void addTab(PodModPtr tab);
	std::map<std::string, PodModPtr, _PodModComp> tabs;
private:
	std::vector<std::string> order;
	SettingsIfacePtr settings;
	std::vector<DyPodModPtr> externs;
};
typedef std::shared_ptr<ModManager> ModManagerPtr;
