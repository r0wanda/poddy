#pragma once
#include <string>
#include <memory>
#include <fstream>
#include <concepts>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <nlohmann/json-schema.hpp>
#include <podmod.hpp>
#include <semver.hpp>
#include <ftxui/component/component.hpp>

template <struct T>
concept JsonStruct = std::is_constructable_v<T, const nlohmann::json>

/**
 * @brief PodSettings WITHOUT window
 * 
 */
class PodSettings : public virtual IndepPodMod {
public:
	ftxui::Component edit;
	PodSettings();
	virtual ~PodSettings() = 0;
protected:
	std::filesystem::path settingsPath;
	nlohmann::json_schema::json_validator schema;
	void validate();
	void initDefault();
	nlohmann::json config;
	void writeConfig();
	void loadConfig();
};
typedef std::shared_ptr<PodSettings> PodSettingsPtr;

/**
 * @brief PodSettings WITH window
 * 
 */
class PodSettingsMod : public PodSettings, public PodMod {
protected:
	PodSettingsMod();
	virtual ~PodSettingsMod() = 0;
};
typedef std::shared_ptr<PodSettingsMod> PodSettingsModPtr;

/**
 * @brief struct representing module manifest
 * 
 */
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
	PodManifestMod(const nlohmann::json &manif);
	virtual ~PodManifestMod() = 0;
};
typedef std::shared_ptr<PodManifestMod> PodManifestModPtr;

// module to edit settings
class SettingsIface : public PodMod {
public:
private:
	std::vector<PodSettings> insts;
};
typedef std::shared_ptr<SettingsIface> SettingsIfacePtr;
