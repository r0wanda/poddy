#pragma once
#include "../podmod/podmod.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/app.hpp>
#include <ftxui/dom/elements.hpp>

/**
 * extensible tui framework to manage and displat modules
 */
class Tui {
public:
	ftxui::App *app;
	ftxui::Component renderer;
	ftxui::Component container;
	ftxui::Components tabsC;
	std::vector<PodMod> modules;
	Tui(std::vector<PodMod> _tabs);
	void popup(ftxui::Component pop);
	PodMod getCurTab();
private:
	int curTab;
	ftxui::Component tabmenu;
	ftxui::Component toggle;
};
