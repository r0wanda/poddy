#pragma once
#include <string>
#include <filesystem>
#include <nohmann/json.hpp>
#include "../podmod/podmod.hpp"

// abstract class that handles settings
template <typename T>
class PodSettingsBase {
public:
	PodSettingsBase(const &T val) : settings(val), written(false) {};
	const T& get() const {
		return settings;
	}
	operator const T&() const {
		return settings;
	}
	void set(const T& nVal) {
		value =
	}
private:
	T settings;
	bool written = false;
};

template <typename S>
class PodSettings : public PodMod {
public:
	PodSettings(std::string _name): PodMod(_name), written(false) {
		settingsPath = _
		loadConfig();
	}
protected:
	S settings;
	std::filesystem::path settingsPath;
	void writeConfig() {}
	void loadConfig(S strct) {
		settings = 
	}
};

// module to edit settings
class SettingsIface : public PodMod {
public:
private:
	std::vector<std::shared_ptr<PodSettings>> insts;
};
