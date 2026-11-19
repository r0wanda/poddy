#include <vector>
#include <locale>
#include <memory>
#include <iostream>
#include <util.hpp>
#include <poddy.hpp>
#include <podmod.hpp>

int main(int argc, char **argv) {
	std::setlocale(LC_ALL, "");
	if (!PodUtil::startConfig()) {
		return 1;
	}
	std::vector<PodModWrapper> modules = { WRAPPODMOD(PoddyMain) };
	Poddy poddy(modules);
	return 0;
}
