#pragma once
#include <memory>
#include <ftxui/component/component.hpp>
#include <ftxui/component/app.hpp>
#include <ftxui/dom/elements.hpp>
#include "../util.hpp"

class PodMod;

/**
 * extensible tui framework to manage and displat modules
 */
class Tui : PodUtil::GetPtr<Tui> {
public:
	ftxui::App *app;
	ftxui::Component renderer;
	ftxui::Component container;
	ftxui::Components tabsC;
	std::vector<std::shared_ptr<PodMod>> modules;
	Tui(std::vector<std::shared_ptr<PodMod>> _tabs);
	void popup(ftxui::Component pop);
	void error(std::string err);
	void error(std::string err, bool fatal);
	std::shared_ptr<PodMod> getCurTab();
	PodUtil::ErrorQueuePtr errq;
private:
	int curTab;
	ftxui::Component tabmenu;
	ftxui::Component toggle;
};

typedef std::shared_ptr<Tui> TuiPtr;
