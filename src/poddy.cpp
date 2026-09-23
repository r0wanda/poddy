#include "poddy.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

PoddyMain::PoddyMain(): PodSettings("iPod") {
	content = Renderer(mainMenu, [&] {
		return hbox({
			controls->Render(),
			separator(),
			mainMenu->Render(),
			separator(),
			info->Render()
		});
	});
}

Poddy::Poddy(std::vector<PodMod> mods): modules(mods), tui(new Tui(modules)), findd(tui) {
	findIpod();
}

void Poddy::findIpod() {
	tui->popup(findd.popup);
}
