#pragma once
#include <map>
#include <string>
#include <nlohmann/json.hpp>
#include "cache.hpp"
#include "../gpod/gpod.hpp"

struct Scrobble {
	std::shared_ptr<GpodTrack> track = nullptr;
	Scrobble *next = nullptr;
	unsigned long ts;
};

class Lastfm {
public:
	Lastfm(const char *key, const char *sec);
	nlohmann::json fetch(std::string method, std::map<std::string, std::string> opts);
	nlohmann::json post(std::string method, std::map<std::string, std::string> opts);
	nlohmann::json authfetch(std::string method, std::map<std::string, std::string> opts);
	void load(Cache *cache);
	void login();
	int scrobble(Scrobble *tracks, int len);
	std::string getUsername();
private:
	inline std::string scrobbleI(int i, std::string key);
	std::string md5(std::string sig);
	std::string encodeURIComponent(std::string decoded);
	std::string sk;
	std::string username;
	const std::string apikey;
	const std::string secret;
	const std::string userAgent;
	const std::string baseUrl;
};
