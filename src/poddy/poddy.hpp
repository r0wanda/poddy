#pragma once
#include <vector>
#include <tui.hpp>
#include <gpod.hpp>
#include <podmod.hpp>
#include <settings.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

class Poddy;
class FindDev;

/**
 * @brief ipod interface window
 * 
 */
class PoddyMain : public PodSettingsMod {
public:
	PoddyMain(std::shared_ptr<Poddy> pod);
	
	void loadMainMenu();
private:
	int selected;
	ftxui::Component controls;

	ftxui::Component mainMenu;
	ftxui::MenuEntryOption menuEntryOpt(GpodBasePtr menuItems);
	GpodBasePtr currentItem;

	ftxui::Component info;
};

class Poddy {
public:
	TuiPtr tui;
	GpodPtr gpod;
	std::shared_ptr<FindDev> findd;
	std::vector<PodModPtr> modules;
	Poddy(std::vector<PodModPtr> mods);
	void findIpod();
};

typedef std::shared_ptr<Poddy> PoddyPtr;
