#pragma once
#include <map>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <gpod/itdb.h>
#include <taglib/tag.h>
#include <taglib/fileref.h>


class GpodArtist;
class GpodAlbum;
class GpodTrack;
class Gpod;

template <typename T>
class SharedPtrGet : public std::enable_shared_from_this<T> {
public:
	std::shared_ptr<T> getPtr() {
		return this->shared_from_this();
	}
};

class GpodBase : public std::enable_shared_from_this<GpodBase> {
public:
	GpodBase(Gpod *gp);
	GpodBase(std::string nm, Gpod *gp);
	std::string name;
	virtual explicit operator std::string() const;
	std::shared_ptr<GpodBase> getBasePtr();
private:
	Gpod *gpod = nullptr;
};

#define GPODBASEPTR_CAST(b, C) std::dynamic_pointer_cast<C>(b.getBasePtr());

class GpodArtwork : public GpodBase, public SharedPtrGet<GpodArtwork> {
public:
	Itdb_Artwork *art = nullptr;
	void write(GpodTrack *tr);
};

#define GPODARTWORK(b) GPODBASEPTR_CAST(b, GpodArtwork)

class GpodTrack : public GpodBase, public SharedPtrGet<GpodTrack> {
public:
	Itdb_Track *track = nullptr;
	std::shared_ptr<GpodArtist> artist;
	std::shared_ptr<GpodAlbum> album;
	unsigned short playcount;
	unsigned short recentPlaycount;
	unsigned long ts;
	GpodTrack(Itdb_Track *tr, Gpod *gp);
	GpodTrack(TagLib::FileRef ref, Gpod *gp);
	virtual explicit operator std::string() const;
};

#define GPODTRACK(b) GPODBASEPTR_CAST(b, GpodTrack)

class GpodAlbum : public GpodBase, public SharedPtrGet<GpodAlbum> {
public:
	std::shared_ptr<GpodArtist> artist;
	std::vector<std::shared_ptr<GpodTrack>> tracks;
	GpodAlbum(std::string n, Gpod *gp);
	virtual explicit operator std::string() const;
};

#define GPODALBUM(b) GPODBASEPTR_CAST(b, GpodAlbum)

class GpodArtist : public GpodBase, public SharedPtrGet<GpodArtist> {
public:
	std::map<std::string, std::shared_ptr<GpodAlbum>> albums;
	std::vector<std::shared_ptr<GpodTrack>> tracks;
	GpodArtist(std::string n, Gpod *gp);
	virtual explicit operator std::string() const;
};

#define GPODARTIST(b) GPODBASEPTR_CAST(b, GpodArtist)

class GpodPlaylist : public GpodBase, public SharedPtrGet<GpodPlaylist> {
public:
	std::vector<std::shared_ptr<GpodTrack>> tracks;
};

#define GPODPLAYLIST(b) GPODBASEPTR_CAST(b, GpodPlaylist)

class Gpod {
public:
	Itdb_iTunesDB *itdb = nullptr;
	std::vector<std::shared_ptr<GpodTrack>> tracks;
	std::map<std::string, std::shared_ptr<GpodArtist>> artists;
	std::vector<std::shared_ptr<GpodAlbum>> albums;
	Gpod(std::string dbPath, std::function<void(GError*, bool)> errH);
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

typedef std::shared_ptr<GpodBase> GpodBasePtr;
typedef std::shared_ptr<GpodArtwork> GpodArtworkPtr;
typedef std::shared_ptr<GpodTrack> GpodTrackPtr;
typedef std::shared_ptr<GpodAlbum> GpodAlbumPtr;
typedef std::shared_ptr<GpodArtist> GpodArtistPtr;
