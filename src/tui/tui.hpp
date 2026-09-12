#pragma once
#include <ftxui/component/component.hpp>
#include <ftxui/component/app.hpp>
#include <ftxui/dom/elements.hpp>

class Tui {
public:
	Tui() {
		app = App::TerminalOutput();
  		app.Loop(component);
	}
};
