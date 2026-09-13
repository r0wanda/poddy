#pragma once
#include 
#include <ftxui/component/component.hpp>
#include <ftxui/component/app.hpp>
#include <ftxui/dom/elements.hpp>

class Tui {
public:
	ftxui::App app;
	ftxui::ComponentDecorator renderer;
	ftxui::Component container;
	ftxui::Components tabs;
	Tui(std::vector<PodMod> _tabs);
	void popup();
private:
	int curTab;
	ftxui::Component tabmenu;
};
