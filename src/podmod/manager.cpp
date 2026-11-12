#include "manager.hpp"
#include <algorithm>
#include <tui.hpp>

DyPodMod::DyPodMod(std::string path, std::shared_ptr<Tui> tui): errq(std::make_shared<PodUtil::ErrorQueue>(path, tui->errq)) {
	// init handle, wait to actually load symbols
	handle = dlopen(path.c_str(), RTLD_LAZY);
	char *err = dlerror();
	if (!handle || err) {
		if (err) errq->error(err);
		else errq->error("couldn't load " + path);
		return;
	}
}

PodModPtr DyPodMod::get(std::shared_ptr<Poddy> pod) {
	if (sharedPtr) return sharedPtr;
	if (!handle) return PodModPtr();

	init = (dypodmod_t)dlsym(handle, "create_object");
	char *err = dlerror();
	if (err) {
		errq->error(err);
		return PodModPtr();
	}
	close = (dypodmodclose_t)dlsym(handle, "free_object");
	err = dlerror();
	if (err) {
		errq->error(err);
		return PodModPtr();
	}

	rawPtr = init(pod.get());

	// no make_shared
	return PodModPtr(rawPtr, [this]([[ maybe_unused ]] PodMod *mod) -> void {
		// mod should be the current instance
		free();
	});
}

void DyPodMod::free() {
	if (!rawPtr) return;
	sharedPtr.reset();
	close(rawPtr);
	rawPtr = nullptr;
	init = nullptr;
	close = nullptr;
	dlclose(handle);
	handle = nullptr;
}
DyPodMod::~DyPodMod() {
	free();
}

ModManager::ModManager(std::shared_ptr<Poddy> pod, SettingsIfacePtr siface): PodSettings::PodSettings("mod manager", pod), settings(siface) {
	tabs = decltype(tabs)([this](const std::string &a, const std::string &b) -> bool {
		// sort tabs by preset order
		return std::find(order.begin(), order.end(), a) < std::find(order.begin(), order.end(), b);
	});
}

void ModManager::addTab(PodModPtr tab) {
	// default: use cached order
	auto orderIt = std::find(order.begin(), order.end(), tab->name);
	if (orderIt == order.end()) {
		// fallback: use order specified by manifest
		PodManifestModPtr mod = tab->getPtr<PodManifestMod>();
		if (mod) {
			orderIt = std::find(order.begin(), order.end(), mod->manifest.before);
		}
		// fallback: insert before settings tab
		if (!mod || orderIt == order.end()) {
			orderIt = std::find(order.begin(), order.end(), settings->name);
			// secret fallback: insert at end if settings isnt found for some reason
		}
	}
	// order determines map sort
	order.insert(orderIt, tab->name);
	tabs.insert({ tab->name, tab });
}

void ModManager::loadExternal(std::string path) {
	DyPodModPtr dym = std::make_shared<DyPodMod>(path, tui);
	externs.push_back(dym);
	addTab(dym->get(poddy));
}
