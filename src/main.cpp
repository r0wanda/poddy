#include <vector>
#include <locale>
#include <memory>
#include <iostream>
#include <config.hpp>
#include <poddy.hpp>
#include <podmod.hpp>

int main(int argc, char **argv) {
	std::setlocale(LC_ALL, "");
	if (!startConfig()) {
		return 1;
	}
	std::vector<PodModPtr> modules = { std::make_shared<PoddyMain>() };
	Poddy poddy(modules);
	return 0;
}
