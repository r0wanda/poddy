#pragma once
#include <string>
#include <nohmann/json.hpp>
#include "../podmod/podmod.hpp"

// abstract class that handles settings
class PodSettings : public PodMod {
public:
	PodSettings(std::string _name);
protected:
	template <typename S>
	void loadConfig(S strct) {
		
	}
};

// module to edit settings
class SettingsIface : public PodMod {
public:
private:
	std::vector<std::shared_ptr<PodSettings>> insts;
};
