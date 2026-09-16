#pragma once
#include <map>
#include <string>
#include <vector>
#include <GLib.h>
#include <functional>
#include <gpod/itdb.h>

class GpodArtist;
class GpodAlbum;

class GpodTrack {
public:
	Itdb_Track *track;
	GpodArtist *artist;
	GpodAlbum *album;
	std::string title;
	unsigned short playcount;
	unsigned short recentPlaycount;
	GpodTrack(Itdb_Track *tr);
};
class GpodAlbum {
public:
	GpodArtist *artist;
	std::vector<GpodTrack*> tracks;
	GpodAlbum(std::string n);
};
class GpodArtist {
public:
	std::map<std::string, GpodAlbum*> albums;
	std::vector<GpodTrack*> tracks;
	std::string name;
	GpodArtist(std::string n);
};
class GpodPlaylist {
public:

	std::vector<GpodTrack*> tracks;
}

class Gpod {
public:
	Itdb_iTunesDB *itdb;
	std::vector<GpodTrack*> tracks;
	std::map<std::string, GpodArtist*> artist;
	std::map<std::string, GpodAlbum*> albums;
	Gpod(std::string dbPath, std::function<void(GError*, bool)> errHandle);
	void process(std::function<void(int)> perCb);
	~Gpod();
private:
	void throwG(bool fatal);
	std::string path;
	std::function<void(GError*, bool)> errHandler;
	GError *err;
};
