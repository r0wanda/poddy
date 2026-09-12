#pragma once
#include <string>
#include <filesystem>
#include <gpod/itdb.h>
#include <unordered_set>
#include <unordered_map>

typedef  ITdb;
class Cache {
	typedef std::unordered_map<std::string, std::pair<unsigned short, unsigned long>> ITdb;
public:
	unsigned long initdbts;
	std::string lfmsk;
	ITdb initdb;
	ITdb db;
	std::unordered_set<std::string> ignored;
    Cache();
	void read();
	void dbset(ITdb *db, std::string title, unsigned short plays, unsigned long ts);
	std::pair<unsigned short, unsigned long> dbget(ITdb *db, Itdb_Track *tr);
	void write();
private:
    std::filesystem::path path;
	std::filesystem::path ignPath;
	unsigned long unixtime();
};
