#include "settings.hpp"
#include <stdexcept>

namespace fs = std::filesystem;
using namespace nlohmann::literals;

PodSettings::PodSettings() {
	settingsPath = configPath / name / ".json";
	if (!fs::exists(settingsPath)) {}
}

void PodSettings::validate() {
	loadConfig();
	try {
		schema.validate(config);
	} catch (const std::exception &err) {
		
	}
}
void PodSettings::initDefault() {

}

void PodSettings::writeConfig() {
	std::ofstream strm(settingsPath);
	strm << config;
	strm.close();
}
void PodSettings::loadConfig() {
	std::ifstream strm(settingsPath);
	config = "{}"_json;
	strm >> config;
	strm.close();
}
PodSettings::~PodSettings() {}

PodSettingsMod::PodSettingsMod() {}
PodSettingsMod::~PodSettingsMod() {}
//PodSettingsMod::PodSettingsMod(std::string _name, std::shared_ptr<Poddy> pod): IndepPodMod::IndepPodMod(_name, pod), PodSettings::PodSettings(_name, pod), PodMod::PodMod(_name, pod) {}

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

PodManifestMod::PodManifestMod(const nlohmann::json &manif): manifest(manif) {}
PodManifestMod::~PodManifestMod() {}
