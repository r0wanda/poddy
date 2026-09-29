#pragma once
#include <vector>
#include <string>
#include <filesystem>
#include "../settings/settings.hpp"

struct ModManagerSettings {
	std::vector<std::string> manifests;
	std::vector<std::string> installed;
};

class ModManager : public PodSettings {
public:
	ModManager
};
