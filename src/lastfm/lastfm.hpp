#pragma once
#include <map>
#include <string>
#include <nlohmann/json.hpp>
#include "cache.hpp"
#include <gpod.hpp>
#include <settings.hpp>

struct LastfmResponse {
	unsigned int ignored = 0;
	unsigned int accepted = 0;
	LastfmResponse operator+(const LastfmResponse &res) {
		return LastfmResponse{ignored + res.ignored, accepted + res.accepted};
	}
};

class Lastfm : public PodSettingsMod {
public:
	Lastfm(const char *key, const char *sec);
	void scrobbleDb();
	void load(Cache *cache);
	void login();
	std::string getUsername();
private:
	nlohmann::json fetch(std::string method, std::map<std::string, std::string> opts);
	nlohmann::json post(std::string method, std::map<std::string, std::string> opts);
	nlohmann::json authfetch(std::string method, std::map<std::string, std::string> opts);
	LastfmResponse scrobble(std::vector<GpodTrackPtr> tracks, std::vector<unsigned long> tss);
	inline std::string scrobbleI(int i, std::string key) const;
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
