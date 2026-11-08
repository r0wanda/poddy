#pragma once
#include <string>
#include <memory>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <podmod.hpp>

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

/**
 * @brief PodSettings WITHOUT window
 * 
 */
class PodSettings : public IndepPodMod {
public:
	PodSettings(std::string _name);
protected:
	std::filesystem::path settingsPath;
	nlohmann::json config;
	void writeConfig() {
		std::ofstream strm(settingsPath);
		strm << config;
		strm.close();
	}
	void loadConfig() {
		std::ifstream strm(settingsPath);
		nlohmann::json j;
		strm >> j;
		strm.close();
		config = j;
	}
};

/**
 * @brief PodSettings WITH window
 * 
 */
class PodSettingsMod : public PodSettings, public PodMod {
public:
	PodSettingsMod(std::string _name, std::shared_ptr<Poddy> pod);
};

// module to edit settings
class SettingsIface : public PodMod {
public:
private:
	std::vector<PodSettings> insts;
};
