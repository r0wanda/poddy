#include "poddy.hpp"
#include <ranges>
#include <vector>
#include <string>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <finddev.hpp>

using namespace ftxui;

PoddyMain::PoddyMain(PoddyPtr pod): PodSettingsMod("iPod", pod) {
	loadMainMenu();
	content = Container::Horizontal({
		Renderer(controls, [this] {
			return hbox({ controls->Render(), separator() });
		}),
		Renderer(controls, [this] {
			return hbox({ mainMenu->Render(), separator() });
		}),
		info
	});
}

MenuEntryOption PoddyMain::menuEntryOpt(GpodBasePtr item) {
	MenuEntryOption option;
	option.transform = [this, &item](EntryState state) {
		state.label = (state.active ? "> " : "  ") + state.label;
		Element e = text(state.label);
		if (state.focused) {
			e = e | inverted;
			currentItem = item;
		}
		if (state.active) {
			e = e | bold;
		}
		return e;
	};
	return option;
}

void PoddyMain::loadMainMenu() {
	// selectors
	static int menuToggleSel = 0;
	static int menuSel;
	static int menuSortSel = 0;
	menuSel = 0;
	menuSortSel = 0;

	// menu settings
	static const std::vector<std::string> tabVals{
		"Albums",
		"Artists",
		"Playlists",
		"Tracks"
	};

	// menu settings components
	static Component mainMenuToggle = Toggle(&tabVals, &menuToggleSel);
	std::vector<Component> menuEnts;
	GpodSortContainerPtr menuItems;

	switch (menuToggleSel) {
	case 0:
		menuItems = gpod->albums;
		break;
	case 1:
		menuItems = gpod->albums;
		break;
	case 2:
		menuItems = gpod->playlists;
		break;
	default:
		menuItems = gpod->tracks;
	}

	// sort menu
	const std::vector<std::string> sortKeys = menuItems->sortKeys();
	Component menuSortDD = Dropdown(&sortKeys, &menuSortSel);
	Component menuToggleCont = Container::Horizontal({
		mainMenuToggle, menuSortDD
	});
	menuEnts.push_back(Renderer(menuToggleCont, [&menuToggleCont] {
		return vbox({
			menuToggleCont->Render(),
			separator()
		});
	}));

	std::vector<GpodBasePtr> items = menuItems->get(sortKeys.at(menuSortSel));
	for (GpodBasePtr &base : items) {
		menuEnts.push_back(MenuEntry(base->name, menuEntryOpt(base)));
	}
	auto menu = Container::Vertical(menuEnts, &menuSel);
	mainMenu = Renderer(menu, [&menu] {
		return menu->Render() | frame;
	});
}


Poddy::Poddy(std::vector<PodModPtr> mods): tui(std::make_shared<Tui>(mods)), findd(std::make_shared<FindDev>(tui)), modules(mods) {
	findIpod();
}

void Poddy::findIpod() {
	tui->popup(findd->popup);
}
