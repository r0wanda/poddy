#pragma once
#include <string>
#include <filesystem>
#include <gpod/itdb.h>
#include <unordered_set>
#include <unordered_map>

typedef std::unordered_map<std::string, std::pair<unsigned short, unsigned long>> CacheITdb;
class Cache {
public:
	unsigned long initdbts;
	std::string lfmsk;
	CacheITdb initdb;
	CacheITdb db;
	std::unordered_set<std::string> ignored;
    Cache(const std::filesystem::path &path, const std::filesystem::path &ign);
	void read();
	void dbset(ITdb *db, std::string title, unsigned short plays, unsigned long ts);
	std::pair<unsigned short, unsigned long> dbget(ITdb *db, Itdb_Track *tr);
	void write();
private:
    std::filesystem::path path;
	std::filesystem::path ignPath;
	unsigned long unixtime();
};
