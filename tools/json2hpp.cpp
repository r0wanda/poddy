#include <regex>
#include <string>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

int main(int argc, char **argv) {
	if (argc < 3) {
		std::cerr << "not enough args\nusage: json2hpp <name> <file>" << std::endl;
		return 1;
	}
	std::ifstream file(argv[2]);
	std::string name(argv[1]);
	nlohmann::json j;
	file >> j;
	file.close();
	std::string dump = j.dump();
	dump = std::regex_replace(dump, std::regex("\\\""), "\\\"");
	std::ofstream outf(std::string(argv[1]) + ".cpp");
	outf << "#include <nlohmann/json.hpp>\nusing namespace nlohmann::literals;\nconst nlohmann::json " << name << " = \"" << dump << "\"_json;\n";
	outf.close();
	return 0;
}
