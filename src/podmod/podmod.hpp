#pragma once
#include <string>
#include <filesystem>
#include <ftxui/component/component.hpp>
#include <ftxui/component/app.hpp>
#include <ftxui/dom/elements.hpp>
#include "../poddy.hpp"

class PodMod {
public:
	ftxui::Component content;
	std::string name;
	PodMod(std::string _name, Poddy *pod);
protected:
	std::filesystem::path configPath;
	std::filesystem::path cachePath;
	Poddy *poddy;
};
