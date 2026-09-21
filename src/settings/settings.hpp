#pragma once
#include "../podmod/podmod.h"

// abstract class that handles settings
class PodSettings : public PodMod {
public:
	PodSettings(std::string _name);
};

// module to edit settings
class SettingsIface : public PodMod {

private:
	std::vector<std::shared_ptr<PodSettings>> insts;
};
