#include "settings.hpp"

namespace fs = std::filesystem;

PodSettings::PodSettings(std::string _name, std::shared_ptr<Poddy> pod): IndepPodMod::IndepPodMod(_name, pod) {
	settingsPath = configPath / name / ".json";
	if (!fs::exists(settingsPath)) {}
}

//PodSettingsMod::PodSettingsMod(std::string _name, std::shared_ptr<Poddy> pod): IndepPodMod::IndepPodMod(_name, pod) {}

const nlohmann::json_schema::json_validator PodManifestT::schema{manifestSchema};
PodManifestT::PodManifestT(const nlohmann::json &manif) {
	schema.validate(manif);
	url = manif.at("url");
	std::string ver = manif.at("version");
	version = semver::from_string(ver);
	zip = manif.at("zip");
	website = manif.at("website");
	name = manif.at("name");
	description = manif.at("description");
	before = manif.at("before");
}

PodManifestMod::PodManifestMod(std::string _name, std::shared_ptr<Poddy> pod, const nlohmann::json &manif): PodSettingsMod::PodSettingsMod(_name, pod), manifest(manif) {}
