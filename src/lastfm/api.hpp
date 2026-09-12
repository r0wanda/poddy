#pragma once
#include <map>
#include <string>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <iostream>
#include <gpod/itdb.h>
#include <openssl/evp.h>
#include <openssl/md5.h>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <nlohmann/json.hpp>
#include "cache.hpp"
#include "encodeuricomponent.hpp"

struct Scrobble {
	Itdb_Track *track = nullptr;
	Scrobble *next = nullptr;
	unsigned long ts;
};

class Lastfm {
public:
	Lastfm(const char *key, const char *sec):
		apikey(key), secret(sec),
		userAgent("lastpod (https://github.com/r0wanda/lastpod)") {
	}
	nlohmann::json fetch(std::string method, std::map<std::string, std::string> opts) {
		std::list<std::string> headers;
		headers.push_back("User-Agent: " + userAgent);
		using namespace curlpp::Options;
		curlpp::Easy req;
		req.setOpt(new HttpHeader(headers));

		std::string url = "https://ws.audioscrobbler.com/2.0/";
		std::string sig = "";
		opts["api_key"] = apikey;
		opts["method"] = method;
		bool first = true;
		for (auto &opt : opts) {
			if (first) {
				url += "?";
				first = false;
			} else url += "&";
			url += opt.first + "=" + opt.second;
			sig += opt.first + opt.second;
		}
		url += "&format=json";
		sig += secret;
		url += "&api_sig=" + md5(sig);

		//std::cout << url << std::endl;
		req.setOpt(new Url(url));
		std::ostringstream sstrm;
		sstrm << req;

		nlohmann::json res = nlohmann::json::parse(sstrm.str());
		if (res.contains("error")) std::cerr << "error " << res["error"] << ": " << res["message"] << std::endl;
		return res;
	}
	nlohmann::json post(std::string method, std::map<std::string, std::string> opts) {
		std::string url = "https://ws.audioscrobbler.com/2.0/";
		std::string sig = "";
		opts["api_key"] = apikey;
		opts["method"] = method;
		opts["sk"] = sk;
		std::string body = "";
		bool first = true;
		for (auto &opt : opts) {
			//std::cout << opt.first << ":" << opt.second << std::endl;
			if (first) {
				//body += "?";
				first = false;
			} else body += "&";
			body += encodeURIComponent(opt.first) + "=" + encodeURIComponent(opt.second);
			sig += opt.first + opt.second;
		}
		sig += secret;
		body += "&api_sig=" + md5(sig);
		body += "&format=json";

		//std::cout << body << std::endl;

		std::list<std::string> headers;
		headers.push_back("User-Agent: " + userAgent);
		headers.push_back("Content-Type: application/x-www-form-urlencoded");
		headers.push_back("Content-Length: " + std::to_string(body.length()));
		using namespace curlpp::Options;
		curlpp::Easy req;
		req.setOpt(new HttpHeader(headers));
		req.setOpt(new Encoding("utf8"));
		req.setOpt(new PostFields(body));
		req.setOpt(new PostFieldSize(body.length()));
		req.setOpt(new Url(url));
		std::ostringstream sstrm;
		sstrm << req;

		//std::cout << sstrm.str();
		nlohmann::json res = nlohmann::json::parse(sstrm.str());
		if (res.contains("error")) std::cerr << "error " << res["error"] << ": " << res["message"] << std::endl;
		return res;
	}
	nlohmann::json authfetch(std::string method, std::map<std::string, std::string> opts) {
		opts["sk"] = sk;
		return fetch(method, opts);
	}
	void load(Cache *cache) {
		if (!cache->lfmsk.empty()) {
			sk = cache->lfmsk;
			auto res = authfetch("user.getInfo", {});
			if (!res.contains("error")) {
				std::cout << "logged in as " << res["user"]["name"] << std::endl;
				return;
			}
		}
		std::cout << "logging in" << std::endl;
		login();
		cache->lfmsk = sk;
		return;
	}
	void login() {
		auto tok = fetch("auth.getToken", {});
		std::string token = tok["token"];

		std::cout << "\"https://www.last.fm/api/auth/?api_key=" + apikey + "&token=" + token + "\"" << std::endl;
#ifndef RPI
		std::string cmd = "xdg-open \"https://www.last.fm/api/auth/?api_key=" + apikey + "&token=" + token + "\"";
		std::system(cmd.c_str());
#endif
		std::cout << "press enter when authentication is complete";
		std::cin.ignore();

		auto session = fetch("auth.getSession", {{"token", token}});
		//std::cout << session;
		sk = session["session"]["key"];
		std::cout << "logged in as " << session["session"]["name"] << std::endl;
	}
	int scrobble(Scrobble *tracks, int len) {
		std::map<std::string, std::string> body;
		int i = 0;
		Scrobble *prev = nullptr;
		Scrobble *sc = tracks;
		while (sc != nullptr && i < len) {
			if (i == 50) {
				std::cerr << "too many tracks in one batch" << std::endl;
				break;
			}
			free(prev);
			Itdb_Track *tr = sc->track;
			if (!tr) continue;
			body[scrobbleI(i, "artist")] = std::string(tr->artist);
			body[scrobbleI(i, "track")] = std::string(tr->title);
			body[scrobbleI(i, "timestamp")] = std::to_string(sc->ts);
			body[scrobbleI(i, "album")] = std::string(tr->album);
			body[scrobbleI(i, "duration")] = std::to_string(tr->tracklen / 1000);
			prev = sc;
			sc = sc->next;
			i++;
		}
		free(prev);
		nlohmann::json res = post("track.scrobble", body);
		std::cerr << res << std::endl;
		int ignored = 0;
		if (res.contains("error")) {
			ignored = len;
		} else {
			ignored = res["scrobbles"]["@attr"]["ignored"];
			std::cout << "accepted: " << res["scrobbles"]["@attr"]["accepted"] << std::endl;
		}
		std::cout << "ignored: " << ignored << std::endl;
		
		return ignored;
	}
private:
	inline std::string scrobbleI(int i, std::string key) {
		std::ostringstream sstrm;
		sstrm << key << "[" << i << "]";
		return sstrm.str();
	}
	std::string md5(std::string sig) {
		std::ostringstream ssig;
		unsigned char digest[MD5_DIGEST_LENGTH];
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
		EVP_Q_digest(NULL, "MD5", NULL, sig.c_str(), sig.size(), digest, NULL);
#else
		MD5_CTX ctx;
		MD5_Init(&ctx);
		MD5_Update(&ctx, sig.c_str(), sig.size());
		MD5_Final(digest, &ctx);
#endif
		for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
			ssig << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(digest[i]);
		}
		return ssig.str();
	}
	std::string sk;
	const std::string apikey;
	const std::string secret;
	const std::string userAgent;
};
