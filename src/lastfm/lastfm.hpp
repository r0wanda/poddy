#pragma once
#include <map>
#include <string>
#include <nlohmann/json.hpp>
#include "cache.hpp"
#include "../gpod/gpod.hpp"
#include "../settings/settings.hpp"

class Lastfm : public PodSettings {
public:
	Lastfm(const char *key, const char *sec);
	void load(Cache *cache);
	void login();
	int scrobble(Scrobble *tracks, int len);
	std::string getUsername();
private:
	nlohmann::json fetch(std::string method, std::map<std::string, std::string> opts);
	nlohmann::json post(std::string method, std::map<std::string, std::string> opts);
	nlohmann::json authfetch(std::string method, std::map<std::string, std::string> opts);
	inline std::string scrobbleI(int i, std::string key);
	std::string md5(std::string sig);
	std::string encodeURIComponent(std::string decoded);
	std::string sk;
	std::string username;
	Cache cache;
	const std::string apikey;
	const std::string secret;
	const std::string userAgent;
	const std::string baseUrl;
};
