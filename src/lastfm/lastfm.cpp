#include <regex>
#include <string>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <string.h>
#include <iostream>
#include <gpod/itdb.h>
#include <openssl/evp.h>
#include <openssl/md5.h>
#include <curlpp/Easy.hpp>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Options.hpp>
#include "lastfm.hpp"
#include "cache.hpp"
#include "../gpod/gpod.hpp"
#define GERROR(e, fatal) if (e) { if (e->message) std::cerr << e->message; g_error_free(e); e = NULL; if (fatal) return 1; }

struct LastFmSettings {
	std::string apiKey;
	std::string apiSecret;
	bool useCache = false;
	std::vector<std::string> ignoredArtists;
};
Lastfm::Lastfm(const char *key, const char *sec):
	apikey(key), secret(sec),
	userAgent("poddy (https://github.com/r0wanda/poddy)"),
	baseUrl("https://ws.audioscrobbler.com/2.0/"),
	cache() {
	cURLpp::initialize();
};

nlohmann::json Lastfm::fetch(std::string method, std::map<std::string, std::string> opts) {
	std::list<std::string> headers;
	headers.push_back("User-Agent: " + userAgent);
	using namespace curlpp::Options;
	curlpp::Easy req;
	req.setOpt(new HttpHeader(headers));

	// construct url
	std::string url = baseUrl;
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

nlohmann::json Lastfm::post(std::string method, std::map<std::string, std::string> opts) {
	std::string url = baseUrl;
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

nlohmann::json Lastfm::authfetch(std::string method, std::map<std::string, std::string> opts) {
	opts["sk"] = sk;
	return fetch(method, opts);
}

void Lastfm::load() {
	if (!cache.lfmsk.empty()) {
		sk = cache.lfmsk;
		auto res = authfetch("user.getInfo", {});
		if (!res.contains("error")) {
			username = res["user"]["name"];
			return;
		} else {
			// TODO: handle errors
		}
	}
	//std::cout << "logging in" << std::endl;
	login();
	cache.lfmsk = sk;
	return;
}

void Lastfm::login() {
	auto tok = fetch("auth.getToken", {});
	std::string token = tok["token"];

	std::cout << "\"https://www.last.fm/api/auth/?api_key=" + apikey + "&token=" + token + "\"" << std::endl;
#ifdef IS_LINUX
	// TODO: options
	std::string cmd = "xdg-open \"https://www.last.fm/api/auth/?api_key=" + apikey + "&token=" + token + "\"";
	std::system(cmd.c_str());
#endif
	// TODO: tui 
	//std::cout << "press enter when authentication is complete";
	//std::cin.ignore();

	nlohmann::json session = fetch("auth.getSession", {{"token", token}});
	sk = session["session"]["key"];
	std::cout << "logged in as " << session["session"]["name"] << std::endl;
}

int scrobble(std::vector<GpodTrackPtr> tracks) {
	std::map<std::string, std::string> body;
	int i = 0;
	for (GpodTrackPtr &sc : tracks) {
		if (i == 50) {
			std::cerr << "too many tracks in one batch" << std::endl;
			break;
		}
		body[scrobbleI(i, "artist")] = std::string(sc->artist->name);
		body[scrobbleI(i, "track")] = std::string(sc->title);
		body[scrobbleI(i, "timestamp")] = std::to_string(sc->ts);
		body[scrobbleI(i, "album")] = std::string(sc->album->title);
		body[scrobbleI(i, "duration")] = std::to_string(sc->track->tracklen / 1000);
		prev = sc;
		sc = sc->next;
		i++;
	}
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

std::string Lastfm::getUsername() {
	return username;
}

std::string Lastfm::scrobbleI(int i, std::string key) {
	std::ostringstream sstrm;
	sstrm << key << "[" << i << "]";
	return sstrm.str();
}

std::string Lastfm::md5(std::string sig) {
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

std::string encodeURIComponent(std::string decoded) {
	// https://gist.github.com/arthurafarias/56fec2cd49a32f374c02d1df2b6c350f
    std::ostringstream oss;
    std::regex r("[!'\\(\\)*-.0-9A-Za-z_~]");

    for (char &c : decoded)
    {
        if (std::regex_match((std::string){c}, r))
        {
            oss << c;
        }
        else
        {
            oss << "%" << std::uppercase << std::hex << (0xff & c);
        }
    }
    return oss.str();
}

int lfm(Gpod *gpod) {
	std::string inp;

	//Lastfm lfm(std::getenv("LASTFM_API_KEY"), std::getenv("LASTFM_API_SECRET"));

	/*GError *err = nullptr;
	Itdb_iTunesDB *itdb;
	if (inp.empty()) {
		if (argc < 2) {
			std::cerr << "usage: " << g_path_get_basename(argv[0]) << " <mountpoint>" << std::endl;
			return 1;
		}
		inp = argv[1];
	}

	itdb = itdb_parse(inp.data(), &err);
	GERROR(err, true);*/

	std::cout << "tracks: " << gpod->tracks.size() << std::endl;
	int n = 0;
	int total = 0;
	int ign = 0;
	
	std::vector<GpodTrackPtr> scrobbles;
	for (const GpodTrackPtr &tr : gpod.tracks) {
		if (tr->playcount < 1) continue;

		unsigned short playcount = tr->playcount;
		cache.dbset(&cache.db, tr->title, tr->playcount, ts);
		auto cData = cache.dbget(&cache.initdb, tr);

		if (ts <= cData.second) continue;
		int plays = playcount - cData.first;
		for (int i = 0; i < plays; i++) {
			if (n == 48) {
				ign += lfm.scrobble(scrobbles, 48);

				first = nullptr;
				prev = nullptr;
				total += n;
				n = 0;
			}
			Scrobble *sc = (Scrobble*)malloc(sizeof(Scrobble));
			sc->track = tr;
			// add 60 seconds between repeated tracks bc itdb doesnt track all timestamps for plays
			sc->ts = tr->time_played + i * 60;
			if (first == nullptr) {
				first = sc;
				prev = sc;
			} else {
				prev->next = sc;
				prev = sc;
			}
			n++;
		}
		//std::cout << tr->title << ": " << tr->playcount << " @ " << tr->time_played << std::endl;
	}
	ign += lfm.scrobble(first, n);
	
	//std::cout << "scrobbled " << (total + n) << " songs" << std::endl;
	//std::cout << "ignored " << ign << " songs" << std::endl;

	cache.write();
	itdb_free(itdb);
	cURLpp::terminate();
	return 0;
}
