#pragma once
#include <vector>
#include "tui/tui.hpp"
#include "gpod/gpod.hpp"
#include "podmod/podmod.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

class PoddyMain : public PodMod {
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
	Gpod *gpod;
	std::vector<PodMod> modules;
	Poddy(std::vector<PodMod> mods);
	void findIpod();
};
