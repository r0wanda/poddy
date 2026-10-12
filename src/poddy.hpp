#pragma once
#include <vector>
#include <tui.hpp>
#include <gpod.hpp>
#include <finddev.hpp>
#include <podmod.hpp>
#include <settings.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

/**
 * @brief ipod interface window
 * 
 */
class PoddyMain : public PodSettings {
public:
	PoddyMain();
	void loadMainMenu();
private:
	int selected;
	ftxui::Component controls;

	ftxui::Component mainMenu;
	void menuEntryOpt();
	GpodBasePtr currentItem;

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
