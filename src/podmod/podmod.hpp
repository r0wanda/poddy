#pragma once
#include <string>
#include <filesystem>
#include <ftxui/component/component.hpp>
#include <ftxui/component/app.hpp>
#include <ftxui/dom/elements.hpp>
#include "../poddy.hpp"

class IndepPodMod {
public:
	std::string name;
	IndepPodMod(std::string _name);
protected:
	std::filesystem::path configPath;
	std::filesystem::path cachePath;
};

class PodMod : public IndepPodMod {
public:
	ftxui::Component content;
	PodMod(std::string _name, Poddy *pod);
protected:
	Poddy *poddy;
};
