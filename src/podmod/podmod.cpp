#include "podmod.hpp"
#include <poddy.hpp>
#include <tui.hpp>
#include <poddy.hpp>
#include <util.hpp>
namespace fs = std::filesystem;

IndepPodMod::IndepPodMod(std::string _name, PoddyPtr pod): name(_name),
	errq(std::make_shared<PodUtil::ErrorQueue>(name, pod->errq)), poddy(pod), gpod(poddy->gpod), tui(poddy->tui) {
	configPath = PodUtil::configDir / name;
	cachePath = PodUtil::cacheDir / name;
	if (!fs::exists(configPath)) fs::create_directory(configPath);
	if (!fs::exists(cachePath)) fs::create_directory(cachePath);
}

PodMod::PodMod() {}
PodMod::~PodMod() {}
//PodMod::PodMod(std::string _name, PoddyPtr pod): IndepPodMod::IndepPodMod(_name, pod) {}
