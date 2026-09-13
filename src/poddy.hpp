#pragma once
#include <vector>
#include "tui/tui.hpp"
#include "gpod/gpod.hpp"
#include "podmod/podmod.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/app.hpp>
#include <ftxui/dom/elements.hpp>

class PoddyMain extends PodMod {
public:
	Poddy()
};

class Poddy {
public:
	Tui tui;
	Gpod gpod;
	std::vector<PodMod> modules;
	Poddy() {
		 
	}
	void findIpod() {
		tui.popup();
	}
};
