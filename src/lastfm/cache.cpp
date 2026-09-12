#include "cache.hpp"
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <filesystem>
#include <gpod/itdb.h>

namespace fs = std::filesystem;

Cache::Cache(): initdbts(0) {
	// TODO: cross-platform
	const char *home = std::getenv("XDG_CONFIG_HOME");
	path = fs::path(home) / ".lastpod";
	ignPath = fs::path(home) / ".lastpodignore";
	read();
}

void Cache::read() {
	initdb = ITdb();
	db = ITdb();
	std::ifstream strm(path);
	if (!strm.is_open()) return;
	std::string idbts;
	std::getline(strm, idbts);
	std::getline(strm, lfmsk);
	initdbts = std::stoul(idbts);
	std::string title;
	std::string plays;
	std::string ts;
	while (std::getline(strm, title) && std::getline(strm, plays) && std::getline(strm, ts)) {
		//std::cout << title << ": " << plays << " @ " << ts << std::endl;
		dbset(&initdb, title, static_cast<unsigned short>(std::stoul(plays)), std::stoul(ts));
	}
	strm.close();
	std::ifstream ignstrm(ignPath);
	if (!ignstrm.is_open()) return;
	std::string ign;
	while (std::getline(ignstrm, ign)) {
		if (ign.empty()) continue;
		ignored.insert(ign);
	}
}
void Cache::dbset(ITdb *db, std::string title, unsigned short plays, unsigned long ts) {
	(*db)[title] = std::pair<unsigned short, unsigned long>(plays, ts);
}
std::pair<unsigned short, unsigned long> Cache::dbget(ITdb *db, Itdb_Track *tr) {
	std::pair<unsigned short, unsigned long> data;
	if (db->contains(tr->title) && !ignored.contains(tr->artist)) {
		data = db->at(tr->title);
	} else {
		data = std::pair(0, 0);
	}
	return data;
}
void Cache::write() {
	std::fstream strm(path, std::fstream::out | std::fstream::trunc);
	strm << unixtime() << "\n";
	strm << lfmsk << "\n";
	for (auto &it : db) {
		strm << it.first << "\n";
		strm << it.second.first << "\n";
		strm << it.second.second << "\n";
	}
	strm.close();
}

unsigned long Cache::unixtime() {
	const auto t = std::chrono::system_clock::now();
	return std::chrono::duration_cast<std::chrono::seconds>(t.time_since_epoch()).count();
}
