#pragma once
#include <vector>
#include "tui/tui.hpp"
#include "gpod/gpod.hpp"
#include "finddev/finddev.hpp"
#include "podmod/podmod.hpp"
#include "settings/settings.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

class PoddyMain : public PodSettings {
public:
	PoddyMain();
private:
	int selected;
	ftxui::Component controls;
	ftxui::Component mainMenu;
	ftxui::Component info;
};

class Poddy {
public:
	Tui *tui;
	Gpod *gpod = nullptr;
	FindDev findd;
	std::vector<PodMod> modules;
	Poddy(std::vector<PodMod> mods);
	void findIpod();
};
