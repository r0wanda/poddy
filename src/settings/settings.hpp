#pragma once
#include <string>
#include <memory>
#include <fstream>
#include <filesystem>
#include <nohmann/json.hpp>
#include "../podmod/podmod.hpp"

// abstract class that handles settings
/*template <typename T>
class Tracker {
public:
	PodSettingsBase(const T& val) : value(val), written(false) {};
	const T& get() const {
		return value;
	}
	operator const T&() const {
		return value;
	}
	void set(const T& val) {
		if (value != val) {

			value = val;
			changed = true;
		}
	}
	const T* operator->() const noexcept {
		return &value;
	}
	T* operator->() noexcept {
		changed = true;
		return &value;
	}

	void write() {
		changed = false;
	}
private:
	T value;
	bool changed = false;
};*/

template <typename S>
class PodSettings : public IndepPodMod {
public:
	PodSettings(std::string _name): PodMod(_name), written(false) {
		settingsPath = configPath / name / ".json";
		loadConfig();
	}
protected:
	S settings;
	std::filesystem::path settingsPath;
	void writeConfig() {
		nlohmann::json j = settings;
		std::ofstream strm(settingsPath);
		strm << j;
		strm.close();
	}
	void loadConfig(S strct) {
		try {
			std::ifstream strm(settingsPath);
			nlohmann::json j;
			strm >> j;
			strm.close();
			settings = j.get<S>();
		} catch (...) {
			// TODO: err handling
			settings = S();
		}
	}
};

// module to edit settings
class SettingsIface : public PodMod {
public:
private:
	std::vector<std::shared_ptr<PodSettings>> insts;
};
