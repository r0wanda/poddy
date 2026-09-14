#pragma once
#include <string>
#include <vector>
#include <GLib.h>
#include <functional>
#include <gpod/itdb.h>

class GpodTrack {
public:
	Itdb_Track *track;
	std::string title;
	GpodTrack(Itdb_Track *tr);
};
class GpodAlbum {

};

class Gpod {
public:
	Itdb_iTunesDB *itdb;
	std::vector<GpodTrack> tracks;
	Gpod(std::string dbPath, std::function<void(GError*, bool)> errHandle);
	void process(std::function<void(int)> perCb);
	~Gpod();
private:
	void throwG(bool fatal);
	std::string path;
	std::function<void(GError*, bool)> errHandler;
	GError *err;
};
