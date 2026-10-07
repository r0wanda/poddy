#include "poddy.hpp"
#include <vector>
#include <string>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

PoddyMain::PoddyMain(): PodSettings("iPod") {
	loadMainMenu();
	content = Container::Horizontal({
		controls->Render(),
		Renderer([] { return separator(); });
		mainMenu->Render(),
		Renderer([] { return separator(); });
		info->Render()
	});
}
void PoddyMain::loadMainMenu() {
	static int menuToggleSel = 0;
	static const std::vector<std::string> tabVals{
		"Albums",
		"Artists",
		"Playlists",
		"Tracks"
	};
	static Toggle mainMenuToggle = Toggle(&tabVals, &menuToggleSel);
	static Component container = Container::tab();
	std::vector<GpodBase> menuEnts;
	switch (menuToggleSel) {
	case 0:
		menuEnts = std::vector<GpodBase>(gpod->albums.begin(), gpod->albums.end());
		break;
	case 1:
		menuEnts = std::vector<GpodBase>(gpod->artists.begin(), gpod->artists.end());
		break;
	case 2:
		menuEnts = std::vector<GpodBase>(gpod->playlists.begin(), gpod->playlists.end());
		break;
	case 3:
		menuEnts = std::vector<GpodBase>(gpod->tracks.begin(), gpod->tracks.end());
		break;
	}
	auto menu = 
}


Poddy::Poddy(std::vector<PodMod> mods): modules(mods), tui(new Tui(modules)), findd(tui) {
	findIpod();
}

void Poddy::findIpod() {
	tui->popup(findd.popup);
}
