#include <vector>
#include "poddy.hpp"
#include "podmod/podmod.hpp"

int main(int argc, char **argv) {
	std::vector<PodMod> modules = { PoddyMain() };
	Poddy poddy(modules);
	return 0;
}
