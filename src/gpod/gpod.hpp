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
	GpodBase(Gpod *gp);
	GpodBase(std::string nm, Gpod *gp);
	std::string name;
	virtual explicit operator std::string() const;
	std::shared_ptr<GpodBase> getBasePtr();
private:
	Gpod *gpod = nullptr;
}
typedef std::shared_ptr<GpodBase> GpodBasePtr;
#define GPODBASEPTR_CAST(b, C) std::dynamic_pointer_cast<C>(b.getBasePtr());

class GpodArtwork : public GpodBase {
public:
	Itdb_Artwork *art = nullptr;
	void write(GpodTrack *tr);
};
typedef std::shared_ptr<GpodArtwork> GpodArtworkPtr;
#define GPODARTWORK(b) GPODBASEPTR_CAST(b, GpodArtwork)

class GpodTrack : public GpodBase {
public:
	Itdb_Track *track = nullptr;
	GpodArtistPtr artist;
	GpodAlbumPtr album;
	unsigned short playcount;
	unsigned short recentPlaycount;
	unsigned long ts;
	GpodTrack(Itdb_Track *tr, Gpod *gp);
	GpodTrack(TagLib::FileRef ref, Gpod *gp);
	virtual explicit operator std::string() const;
};
typedef std::shared_ptr<GpodTrack> GpodTrackPtr;
#define GPODTRACK(b) GPODBASEPTR_CAST(b, GpodTrack)

class GpodAlbum : public GpodBase {
public:
	GpodArtistPtr artist;
	std::vector<GpodTrackPtr> tracks;
	GpodAlbum(std::string n);
	virtual explicit operator std::string() const;
};
typedef std::shared_ptr<GpodAlbum> GpodAlbumPtr;
#define GPODALBUM(b) GPODBASEPTR_CAST(b, GpodAlbum)

class GpodArtist : public GpodBase {
public:
	std::map<std::string, GpodAlbumPtr> albums;
	std::vector<GpodTrackPtr> tracks;
	GpodArtist(std::string n);
	virtual explicit operator std::string() const;
};
typedef std::shared_ptr<GpodArtist> GpodArtistPtr;
#define GPODARTIST(b) GPODBASEPTR_CAST(b, GpodArtist)

class GpodPlaylist : public GpodBase {
public:
	std::vector<GpodTrackPtr> tracks;
};
#define GPODPLAYLIST(b) GPODBASEPTR_CAST(b, GpodPlaylist)

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
