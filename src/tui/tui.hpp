#pragma once
#include <memory>
#include <ftxui/component/component.hpp>
#include <ftxui/component/app.hpp>
#include <ftxui/dom/elements.hpp>
#include <util.hpp>

class PodMod;
class Tui;
class Poddy;

class TuiErrorQueue : public PodUtil::ErrorQueue {
public:
	TuiErrorQueue(std::shared_ptr<Tui> tuiPtr);
	void error(PodUtil::ErrQ err) override;
private:
	std::shared_ptr<Tui> tui;
};

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
	Tui(std::shared_ptr<Poddy> pod);
	void popup(ftxui::Component pop);
	void dispError(PodUtil::ErrQ err);
	std::shared_ptr<PodMod> getCurTab();
	PodUtil::ErrorQueuePtr errq;
private:
	std::shared_ptr<Poddy> poddy;
	int curTab;
	ftxui::Component tabmenu;
	ftxui::Component toggle;
};

typedef std::shared_ptr<Tui> TuiPtr;
