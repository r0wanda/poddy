#include "manager.hpp"
#include <tui.hpp>

DyPodMod::DyPodMod(std::string path, std::shared_ptr<Tui> tui): errq(std::make_shared<PodUtil::ErrorQueue>(path, tui->errq)) {
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
	return PodModPtr(rawPtr, [this](PodMod *mod) -> void {
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

ModManager(std::shared_ptr<Tui> tui);

void ModManager::loadExternal(std::string path) [

]
