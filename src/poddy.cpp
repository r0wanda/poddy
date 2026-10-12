#include "poddy.hpp"
#include <span>
#include <vector>
#include <string>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

PoddyMain::PoddyMain(): PodSettings("iPod") {
	loadMainMenu();
	content = Container::Horizontal({
		controls->Render(),
		Renderer([] { return separator(); }),
		mainMenu->Render(),
		Renderer([] { return separator(); }),
		info->Render()
	});
}

MenuEntryOption PoddyMain::menuEntryOpt(int i, const std::vector<GpodBasePtr> &menuItems) {
	MenuEntryOption option;
	option.transform = [this](EntryState state) {
		state.label = (state.active ? "> " : "  ") + state.label;
		Element e = text(state.label);
		if (state.focused) {
			e = e | inverted;
			currentItem = menuItems.at(i + 2);
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
	static int menuSortSel;
	menuSel = 0;
	menuSortSel = 0;

	// menu settings
	static constexpr std::vector<std::string> tabVals{
		"Albums",
		"Artists",
		"Playlists",
		"Tracks"
	};
	// backwards alphabetical
	static bool azBackwards = false;
	static constexpr std::span<const std::string> _sortVals{"Artist", "Name", "Release Date", "Recently Played"};
	std::span<const std::string> sortVals;

	// menu settings components
	static Toggle mainMenuToggle = Toggle(&tabVals, &menuToggleSel);
	static Component container = Container::Tab();
	std::vector<MenuEntry> menuEnts;
	std::vector<GpodBasePtr> menuItems;

	switch (menuToggleSel) {
	case 0:
		menuItems = std::vector<GpodBasePtr>(gpod->albums.begin(), gpod->albums.end());
		sortVals = _sortVals;
		break;
	case 1:
		menuItems = std::vector<GpodBasePtr>(gpod->artists.begin(), gpod->artists.end());
		sortVals = _sortVals.subspan(1);
		break;
	case 2:
		menuItems = std::vector<GpodBasePtr>(gpod->playlists.begin(), gpod->playlists.end());
		sortVals = _sortVals.subspan(1);
		break;
	case 3:
		menuItems = std::vector<GpodBasePtr>(gpod->tracks.begin(), gpod->tracks.end());
		break;
	}
	auto menuSortDD = Dropdown(&sortVals, )
	auto menuToggleCont = Container::Horizontal({
		mainMenuToggle,
	});
	menuEnts.push_back();
	menuEnts.push_back(Renderer([] { return separator(); }));

	for (GpodBasePtr &base : menuItems) {
		menuEnts.push_back(MenuEntry(base->name, menuEntryOpt));
	}
	auto menu = Container::Vertical(menuEnts, &menuSel);
	mainMenu = Renderer(menu, [&] {
		return menu->Render() | frame;
	});
}


Poddy::Poddy(std::vector<PodMod> mods): modules(mods), tui(new Tui(modules)), findd(tui) {
	findIpod();
}

void Poddy::findIpod() {
	tui->popup(findd.popup);
}
