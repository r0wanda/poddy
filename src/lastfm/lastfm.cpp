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
#include <gpod.hpp>
#define GERROR(e, fatal) if (e) { if (e->message) std::cerr << e->message; g_error_free(e); e = NULL; if (fatal) return 1; }

struct LastFmSettings {
	std::string apiKey;
	std::string apiSecret;
	bool useCache = false;
	std::vector<std::string> ignoredArtists;
};

LastfmIgnList::LastfmIgnList(const nlohmann::json &list) {
	
}

LastfmIgnore::LastfmIgnore(const nlohmann::json &sch) {
	// TODO: patch
	schema.validate(sch);
	
}

Lastfm::Lastfm(const char *key, const char *sec):
	apikey(key), secret(sec),
	userAgent("poddy (https://github.com/r0wanda/poddy)"),
	baseUrl("https://ws.audioscrobbler.com/2.0/"),
	cache(cachePath / "lastpod.db", configPath / "ignore.json") {
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

LastfmResponse Lastfm::scrobble(const std::vector<const GpodTrackPtr> &tracks, const std::vector<unsigned long> &tss) {
	std::map<std::string, std::string> body;
	if (tracks.size() != tss.size()) {
		errq->error("invalid scrobble batch");
		return LastfmResponse{tracks.size(), 0};
	}
	LastfmResponse report;
	int i = 0;
	for (const GpodTrackPtr &sc : tracks) {
		if (i == 50) {
			errq->warn("too many tracks in one batch");
			report.ignored += sc.size() - 50;
			break;
		}
		body[scrobbleI(i, "artist")] = std::string(sc->artist->name);
		body[scrobbleI(i, "track")] = std::string(sc->title);
		body[scrobbleI(i, "timestamp")] = std::to_string(tss.at(i));
		body[scrobbleI(i, "album")] = std::string(sc->album->title);
		body[scrobbleI(i, "duration")] = std::to_string(sc->track->tracklen / 1000);
		prev = sc;
		sc = sc->next;
		i++;
	}

	nlohmann::json res = post("track.scrobble", body);
	//std::cerr << res << std::endl;
	if (res.contains("error")) {
		report.ignored = tracks.size();
	} else {
		report.ignored += res["scrobbles"]["@attr"]["ignored"];
		report.accepted += res["scrobbles"]["@attr"]["accepted"];
	}
	
	return report;
}

std::string Lastfm::getUsername() {
	return username;
}

std::string Lastfm::scrobbleI(int i, std::string key) const {
	std::string str = key + "[" + std::to_string(i) + "]";
	return str;
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

std::string Lastfm::encodeURIComponent(std::string decoded) {
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

void Lastfm::scrobbleDb() {
	//std::string inp;

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

	//std::cout << "tracks: " << gpod->tracks.size() << std::endl;
	LastfmResponse report;
	
	std::vector<const GpodTrackPtr> scrobbles;
	std::vector<unsigned long> tss;
	for (const GpodTrackPtr &gtr : gpod.tracks) {
		GpodTrackPlayData tr = gtr->playData;
		if (tr->playcount < 1) continue;

		unsigned short playcount = tr.playcount;
		cache.dbset(&cache.db, gtr->title, tr.playcount, ts);
		auto cData = cache.dbget(&cache.initdb, tr);

		if (ts <= cData.second) continue;
		int plays = playcount - cData.first;
		for (int i = 0; i < plays; i++) {
			if (scrobbles.size() == 48) {
				report += scrobble(scrobbles, tss);
				tss.clear();
				scrobbles.clear();
			}
			// add 60 seconds between repeated tracks bc itdb doesnt track all timestamps for plays
			tss.push_back(tr.ts + i * 60);
			scrobbles.push_back(gtr);
		}
	}
	report += scrobble(scrobbles, tss);
	
	//std::cout << "scrobbled " << (total + n) << " songs" << std::endl;
	//std::cout << "ignored " << ign << " songs" << std::endl;

	cache.write();
	cURLpp::terminate();
	return 0;
}
