#include "../tui/tui.hpp"
#include "../podmod/podmod.hpp"
#include <vector>
#include <string>
#include <ftxui/component/component.hpp>
#include <ftxui/component/app.hpp>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

Tui::Tui(std::vector<PodMod> _tabs): app(nullptr), tabsC(), modules(_tabs), curTab(0) {
	/*std::vector<std::string> names;
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
	});*/
	app = new App(App::FullscreenAlternateScreen());
  	//app->Loop(renderer);
}
void Tui::popup(ftxui::Component pop) {
	auto renderer = Renderer(pop, [&] {
    	return vbox({
        	filler(),
        	hbox({
            	filler(),
        		pop->Render() | border,
            	filler(),
        	}),
        	filler(),
    	});
	});
	app->Loop(renderer);
	//std::getchar();
}
void Tui::error(std::string err, bool fatal) {
	auto renderer = Renderer(*app, vbox({
		text("Error") | hcenter,
		text(err) | size(HEIGHT, EQUAL, 5) | size(WIDTH, EQUAL, err.size() + 2) | border,
		text("Press anything to " + (fatal ? "exit" : "dismiss"))
	}) | size(HEIGHT, GREATER_THAN, 5) | size(WIDTH, GREATER_THAN, err.size() + 2) | border | center);
	app->Print();
	if (fatal) exit(1);
}
PodMod Tui::getCurTab() {
	return modules[curTab];
}
