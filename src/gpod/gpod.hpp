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

class GpodBase : public std::enable_shared_from_this<GpodBase> {
public:
	virtual std::shared_ptr<GpodBase> getBasePtr();
}

class GpodArtwork : public GpodBase {
public:
	Itdb_Artwork *art = nullptr;
	void write(GpodTrack *tr);
	std::shared_ptr<GpodArtwork> getPtr();
};
typedef std::shared_ptr<GpodArtwork> GpodArtworkPtr;

class GpodTrack : public std::enable_shared_from_this<GpodTrack> {
public:
	Gpod *gpod = nullptr;
	Itdb_Track *track = nullptr;
	GpodArtistPtr artist;
	GpodAlbumPtr album;
	std::string title;
	unsigned short playcount;
	unsigned short recentPlaycount;
	unsigned long ts;
	GpodTrack(Itdb_Track *tr, Gpod *gp);
	GpodTrack(TagLib::FileRef ref, Gpod *gp);
	std::shared_ptr<GpodTrack> getPtr();
};
typedef std::shared_ptr<GpodTrack> GpodTrackPtr;

class GpodAlbum : public std::enable_shared_from_this<GpodAlbum> {
public:
	GpodArtistPtr artist;
	std::vector<GpodTrackPtr> tracks;
	GpodAlbum(std::string n);
	std::string title;
	std::shared_ptr<GpodAlbum> getPtr();
};
typedef std::shared_ptr<GpodAlbum> GpodAlbumPtr;

class GpodArtist : public std::enable_shared_from_this<GpodArtist> {
public:
	std::map<std::string, GpodAlbumPtr> albums;
	std::vector<GpodTrackPtr> tracks;
	std::string name;
	GpodArtist(std::string n);
	std::shared_ptr<GpodArtist> getPtr();
};
typedef std::shared_ptr<GpodArtist> GpodArtistPtr;

class GpodPlaylist : public std::enable_shared_from_this<GpodPlaylist> {
public:
	std::vector<GpodTrackPtr> tracks;
};

class Gpod {
public:
	Itdb_iTunesDB *itdb = nullptr;
	std::vector<GpodTrackPtr> tracks;
	std::map<std::string, GpodArtistPtr> artists;
	std::vector<GpodAlbumPtr> albums;
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
