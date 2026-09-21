#include <vector>
#include <locale>
#include <iostream>
#include "config.hpp"
#include "poddy.hpp"
#include "podmod/podmod.hpp"

int main(int argc, char **argv) {
	std::setlocale(LC_ALL, "");
	if (!startConfig()) {
		#ifdef IS_LINUX
		std::cerr << "$XDG_*_HOME environment variables not found" << std::endl;
		#endif
	}
	std::vector<PodMod> modules = { /*PoddyMain()*/ };
	Poddy poddy(modules);
	return 0;
}
