#include "poddy.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

PoddyMain::PoddyMain() {
	name = "iPod";
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

Poddy::Poddy(std::vector<PodMod> mods) {
	modules = mods;
	tui = new Tui(modules);
	gpod = new Gpod();
}

void Poddy::findIpod() {
	tui->popup();
}
