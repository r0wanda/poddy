#include "podmod.hpp"
#include <poddy.hpp>
#include <config.hpp>
#include <tui.hpp>
#include <poddy.hpp>
namespace fs = std::filesystem;

IndepPodMod::IndepPodMod(std::string _name, std::shared_ptr<Poddy> pod): name(_name),
	errq(std::make_shared<PodUtil::ErrorQueue>(name, pod->errq)), poddy(pod), gpod(poddy->gpod), tui(poddy->tui) {
	startConfig();
	configPath = configDir / name;
	cachePath = cacheDir / name;
	if (!fs::exists(configPath)) fs::create_directory(configPath);
	if (!fs::exists(cachePath)) fs::create_directory(cachePath);
}


PodMod::PodMod(std::string _name, PoddyPtr pod): IndepPodMod::IndepPodMod(_name, pod) {}
