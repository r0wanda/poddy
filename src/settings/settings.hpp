#pragma once
#include <string>
#include <memory>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <nlohmann/json-schema.hpp>
#include <podmod.hpp>
#include <semver.hpp>
#include <ftxui/component/component.hpp>

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
class PodSettings : public virtual IndepPodMod {
public:
	ftxui::Component edit;
	PodSettings(std::string _name, std::shared_ptr<Poddy> pod);
	virtual ~PodSettings() = default;
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
typedef std::shared_ptr<PodSettings> PodSettingsPtr;

/**
 * @brief PodSettings WITH window
 * 
 */
class PodSettingsMod : public PodSettings, public PodMod {
public:
	PodSettingsMod(std::string _name, std::shared_ptr<Poddy> pod);
};
typedef std::shared_ptr<PodSettingsMod> PodSettingsModPtr;

extern const nlohmann::json manifestSchema;
struct PodManifestT {
	static const nlohmann::json_schema::json_validator schema;
	PodManifestT(const nlohmann::json &manif);
	std::string url;
	semver::version<uint32_t, uint32_t, uint32_t> version;
	std::string zip;
	std::string website;
	std::string name;
	std::string description;
	std::string before;
};
class PodManifestMod : public PodSettingsMod {
public:
	const PodManifestT manifest;
	PodManifestMod(std::string _name, std::shared_ptr<Poddy> pod, const nlohmann::json &manif);
};
typedef std::shared_ptr<PodManifestMod> PodManifestModPtr;

// module to edit settings
class SettingsIface : public PodMod {
public:
private:
	std::vector<PodSettings> insts;
};
typedef std::shared_ptr<SettingsIface> SettingsIfacePtr;
