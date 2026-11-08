#include "tui.hpp"
#include <poddy.hpp>
#include <podmod.hpp>
#include <vector>
#include <string>
#include <ftxui/component/component.hpp>
#include <ftxui/component/app.hpp>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

TuiErrorQueue::TuiErrorQueue(std::shared_ptr<Tui> tuiPtr): ErrorQueue::ErrorQueue("poddy"), tui(tuiPtr) {}
void TuiErrorQueue::error(PodUtil::ErrQ err) {
	if (!err.errq) err.errq = getPtr();
	tui->dispError(err);
}

Tui::Tui(std::shared_ptr<Poddy> pod): app(nullptr), tabsC(), errq(std::make_shared<TuiErrorQueue>(getPtr())), poddy(pod), modules(poddy->modules), curTab(0) {
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
void Tui::dispError(PodUtil::ErrQ err) {
	Render(*app, vbox({
		text(err.errq->name + " error") | hcenter,
		text(err.msg) | size(HEIGHT, EQUAL, 5) | size(WIDTH, EQUAL, err.msg.size() + 2) | border,
		text(std::string("press anything to ") + (err.fatal ? "exit" : "dismiss"))
	}) | size(HEIGHT, GREATER_THAN, 5) | size(WIDTH, GREATER_THAN, err.msg.size() + 2) | border | center);
	app->Print();
	if (err.fatal) exit(1);
}

PodModPtr Tui::getCurTab() {
	return modules.at(curTab);
}
