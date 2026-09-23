#include <vector>
#include <locale>
#include <iostream>
#include "config.hpp"
#include "poddy.hpp"
#include "podmod/podmod.hpp"

int main(int argc, char **argv) {
	std::setlocale(LC_ALL, "");
	if (!startConfig()) {
		return 1;
	}
	std::vector<PodMod> modules = { /*PoddyMain()*/ };
	Poddy poddy(modules);
	return 0;
}
