#pragma once
#include <vector>
#include "tui/tui.hpp"
#include "gpod/gpod.hpp"
#include "podmod/podmod.hpp"

class Poddy {
public:
	Tui tui;
	Gpod gpod;
	std::vector<PodMod> modules;
	Poddy() {
		 
	}
	void findIpod() {
		tui.popup
	}
};
