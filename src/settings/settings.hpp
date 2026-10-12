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

class PodSettings : public IndepPodMod {
public:
	PodSettings(std::string _name);
protected:
	std::filesystem::path settingsPath;
	template <typename T>
	void writeConfig(const T &strct) {
		nlohmann::json j = strct;
		std::ofstream strm(settingsPath);
		strm << j;
		strm.close();
	}
	template <typename T>
	T loadConfig() {
		try {
			std::ifstream strm(settingsPath);
			nlohmann::json j;
			strm >> j;
			strm.close();
			return j.get<T>();
		} catch (...) {
			// TODO: err handling
			return T();
		}
	}
};

// module to edit settings
class SettingsIface : public PodMod {
public:
private:
	std::vector<PodSettings> insts;
};
