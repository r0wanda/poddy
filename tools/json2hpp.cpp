#include <regex>
#include <string>
#include <sstream>
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
	std::ostringstream ss;
	ss << std::showbase << std::setfill('0') << std::setw(4) << std::hex;
	for (const char ch : dump) {
		ss << (int)ch << ",";
	}
	std::string out = ss.str() + "0x00";

	std::ofstream outf(std::string(argv[1]) + ".cpp");
	outf << "#include <nlohmann/json.hpp>\nconst unsigned char _" << name << "[] = {" << out << "};\nconst nlohmann::json " << name << " = nlohmann::json::parse(_" << name << ", _" << name << " + " << out.size() << ");\n";
	outf.close();
	return 0;
}
