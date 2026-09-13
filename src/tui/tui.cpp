#include "tui.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/app.hpp>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

Tui::Tui(Components _tabs): curTab(0) {
	app = App::FullscreenAlternateScreen();
	tabmenu = Container::Tab(_tabs, &curTab);
	
	container = Container::Vertical({

	})
	renderer = 
  	app.Loop(tabmenu);
}
void popup() {

}
