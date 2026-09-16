#include <string>
#include <string.h>
#include <iostream>
#include <gpod/itdb.h>
#include <curlpp/cURLpp.hpp>
#include "lastfm.hpp"
#include "cache.hpp"
#include "../gpod/gpod.hpp"
#define GERROR(e, fatal) if (e) { if (e->message) std::cerr << e->message; g_error_free(e); e = NULL; if (fatal) return 1; }

struct LastFmSettings {
	bool useCache = true;
	std::vector<std::string> ignoredArtists;
};

int lfm(Gpod *gpod) {
	cURLpp::initialize();
	//dotenv::init();
	std::string inp;

	Lastfm lfm(std::getenv("LASTFM_API_KEY"), std::getenv("LASTFM_API_SECRET"));
	Cache cache;
	lfm.load(&cache);

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

	std::cout << "tracks: " << gpod.tracks.size() << std::endl;
	//GList *it;
	Scrobble *first = nullptr;
	Scrobble *prev = nullptr;
	int n = 0;
	int total = 0;
	int ign = 0;
	
	for (const GpodTrack &tr : gpod.tracks) {
		if (tr->playcount < 1) continue;

		std::string title(tr->title);
		unsigned short playcount = tr->playcount;
		unsigned long ts = static_cast<unsigned long>(tr->time_played);
		cache.dbset(&cache.db, title, playcount, ts);
		auto cData = cache.dbget(&cache.initdb, tr);

		if (ts <= cData.second) continue;
		int plays = playcount - cData.first;
		for (int i = 0; i < plays; i++) {
			if (n == 48) {
				ign += lfm.scrobble(first, 48);

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
