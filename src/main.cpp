#include <vector>
#include <locale>
#include "poddy.hpp"
#include "podmod/podmod.hpp"

int main(int argc, char **argv) {
	std::setlocale(LC_ALL, "");
	g_type_init();
	std::vector<PodMod> modules = { PoddyMain() };
	Poddy poddy(modules);
	return 0;
}
