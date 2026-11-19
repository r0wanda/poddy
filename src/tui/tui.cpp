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

Tui::Tui(std::shared_ptr<Poddy> pod): app(nullptr), tabsC(), errq(std::make_shared<TuiErrorQueue>(getPtr())), poddy(pod), curTab(0) {
	tabNames.clear();
	for (auto const &[name, mod] : poddy->modules->tabs) {
		tabsC.push_back(mod->content);
		tabNames.push_back(name);
	}
	tabmenu = Container::Tab(tabsC, &curTab);
	toggle = Toggle(tabNames, &curTab);
	
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
	try {
		return poddy->modules->tabs.at(tabNames.at(curTab));
	} catch (...) {
		return PodModPtr();
	}
}
