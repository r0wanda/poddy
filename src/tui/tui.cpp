#include "../tui/tui.hpp"
#include "../podmod/podmod.hpp"
#include <vector>
#include <string>
#include <ftxui/component/component.hpp>
#include <ftxui/component/app.hpp>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

Tui::Tui(std::vector<PodMod> _tabs): curTab(0), tabsC(), modules(_tabs), app(nullptr) {
	std::vector<std::string> names;
	for (const PodMod &pm : modules) {
		tabsC.push_back(pm.content);
		names.push_back(pm.name);
	}
	tabmenu = Container::Tab(tabsC, &curTab);
	toggle = Toggle(names, &curTab);
	
	container = Container::Vertical({
		toggle,
		tabmenu
	});
	renderer = Renderer(container, [&] {
		return vbox({
			toggle->Render(),
			separator(),
			container->Render()
		});
	});
	app = new App(App::FullscreenAlternateScreen());
  	app->Loop(renderer);
}
void Tui::popup() {

}
PodMod Tui::getCurTab() {
	return modules[curTab];
}
