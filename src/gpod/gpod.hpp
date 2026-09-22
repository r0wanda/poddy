#pragma once
#include <map>
#include <string>
#include <vector>
#include <memory>
#include <glib.h>
#include <functional>
#include <gpod/itdb.h>
#include <taglib/tag.h>
#include <taglib/fileref.h>

class GpodArtist;
class GpodAlbum;
class GpodTrack;
class Gpod;

class GpodArtwork : public std::enable_shared_from_this<GpodArtwork> {
public:
	Itdb_Artwork *art = nullptr;
	void write(GpodTrack *tr);
	std::shared_ptr<GpodArtwork> getPtr();
};
class GpodTrack : public std::enable_shared_from_this<GpodTrack> {
public:
	Gpod *gpod = nullptr;
	Itdb_Track *track = nullptr;
	GpodArtist *artist = nullptr;
	GpodAlbum *album = nullptr;
	std::string title;
	unsigned short playcount;
	unsigned short recentPlaycount;
	unsigned long ts;
	GpodTrack(Itdb_Track *tr, Gpod *gp);
	GpodTrack(TagLib::FileRef ref, Gpod *gp);
};
class GpodAlbum: public std::enable_shared_from_this<GpodAlbum> {
public:
	GpodArtist *artist = nullptr;
	std::vector<GpodTrack*> tracks;
	GpodAlbum(std::string n);
};
class GpodArtist: public std::enable_shared_from_this<GpodArtist> {
public:
	std::map<std::string, GpodAlbum*> albums;
	std::vector<GpodTrack*> tracks;
	std::string name;
	GpodArtist(std::string n);
};
class GpodPlaylist: public std::enable_shared_from_this<GpodPlaylist> {
public:

	std::vector<GpodTrack*> tracks;
};

class Gpod {
public:
	Itdb_iTunesDB *itdb = nullptr;
	std::vector<GpodTrack*> tracks;
	std::map<std::string, GpodArtist*> artists;
	std::map<std::string, GpodAlbum*> albums;
	Gpod(std::string dbPath, std::function<void(GError*, bool)> errHandle);
	void process(std::function<void(int)> perCb);
	~Gpod();
	friend class GpodTrack;
	friend class GpodAlbum;
	friend class GpodArtist;
	friend class GpodPlaylist;
protected:
	void throwG(bool fatal);
	std::string path;
	std::function<void(GError*, bool)> errHandle;
	GError *err;
};
