#pragma once
#include <vector>
#include <tui.hpp>
#include <util.hpp>
#include <gpod.hpp>
#include <podmod.hpp>
#include <manager.hpp>
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

class Poddy : public PodUtil::GetPtr<Poddy> {
public:
	TuiPtr tui;
	GpodPtr gpod;
	std::shared_ptr<FindDev> findd;
	ModManagerPtr modules;
	PodUtil::ErrorQueuePtr errq;
	Poddy(std::vector<PodModWrapper> mods);
	void findIpod();
};

typedef std::shared_ptr<Poddy> PoddyPtr;
