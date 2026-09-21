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

Poddy::Poddy(std::vector<PodMod> mods): findd() {
	modules = mods;
	tui = new Tui(modules);
	findIpod();
}

void Poddy::findIpod() {
	tui->popup(findd.popup);
}
