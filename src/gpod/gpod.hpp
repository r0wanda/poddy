#pragma once
#include <map>
#include <ctime>
#include <string>
#include <vector>
#include <memory>
#include <ranges>
#include <variant>
#include <algorithm>
#include <functional>
#include <type_traits>
#include <gpod/itdb.h>
#include <taglib/tag.h>
#include <taglib/fileref.h>


class GpodArtist;
class GpodAlbum;
class GpodTrack;
class Gpod;

struct GpodSortItem {
	std::string sortName;
	bool ascend;
};
/**
 * @brief Gpod* sorting methods
 * { display name, { string to sort, ascending? } }
 */
typedef std::map<const std::string, const GpodSortItem> GpodSortMap;

/**
 * @brief utility 
 * 
 * @tparam T class type
 */
template <typename T>
class SharedPtrGet : public std::enable_shared_from_this<T> {
public:
	virtual std::shared_ptr<T> getPtr() {
		return this->shared_from_this();
	}
};

class GpodBase : public SharedPtrGet<GpodBase> {
public:
	GpodBase(Gpod *gp);
	GpodBase(std::string nm, Gpod *gp);
	GpodBase(std::string nm, std::string sortNm, Gpod *gp);
	const std::string name;
	const std::string sortName;
	virtual std::string display() const;
	std::shared_ptr<GpodBase> getBasePtr();
	GpodSortMap sort;
	virtual ~GpodBase() = default;
private:
	Gpod *gpod = nullptr;
};

#define GPODBASEPTR_CAST(b, C) std::dynamic_pointer_cast<C>(b.getBasePtr());

class GpodArtwork : public GpodBase, public SharedPtrGet<GpodArtwork> {
public:
	void write(GpodTrack *tr);
protected:
	Itdb_Artwork *art = nullptr;
};

#define GPODARTWORK(b) GPODBASEPTR_CAST(b, GpodArtwork)

struct GpodTrackData {
	std::string genre;
	std::string filetype;
	std::string comment;
	std::string category;
	std::string composer;
	std::string description;
};
/*struct GpodTrackSort {
	std::string sort_title;
	std::string sort_composer;
	std::string sort_tvshow;
};*/
struct GpodTrackPlayData {
	unsigned short playcount;
	unsigned short recentPlaycount;
	unsigned long ts;
};

class GpodTrack : public GpodBase, public SharedPtrGet<GpodTrack> {
public:
	std::shared_ptr<GpodArtist> artist;
	std::shared_ptr<GpodAlbum> album;
	GpodTrackPlayData playData;
	
	GpodTrack(Itdb_Track *tr, Gpod *gp);
	GpodTrack(TagLib::FileRef ref, Gpod *gp);
	std::string display() const override;
protected:
	Itdb_Track *track = nullptr;
private:
};

#define GPODTRACK(b) GPODBASEPTR_CAST(b, GpodTrack)

class GpodAlbum : public GpodBase, public SharedPtrGet<GpodAlbum> {
public:
	std::shared_ptr<GpodArtist> artist;
	std::vector<std::shared_ptr<GpodTrack>> tracks;
	GpodAlbum(std::string n, std::string sortNm, int year, std::shared_ptr<GpodArtist> arst, Gpod *gp);

	void addTrack(std::shared_ptr<GpodTrack> tr);

	std::string display() const override;
};

#define GPODALBUM(b) GPODBASEPTR_CAST(b, GpodAlbum)

struct GpodArtistSort {
	std::string sort_artist;
};

class GpodArtist : public GpodBase, public SharedPtrGet<GpodArtist> {
public:
	std::map<std::string, std::shared_ptr<GpodAlbum>> albums;
	std::vector<std::shared_ptr<GpodTrack>> tracks;
	GpodArtist(std::string n, Gpod *gp);

	void addTrack(std::shared_ptr<GpodTrack> tr);
	void addAlbum(std::shared_ptr<GpodAlbum> al);
};

#define GPODARTIST(b) GPODBASEPTR_CAST(b, GpodArtist)

class GpodPlaylist : public GpodBase, public SharedPtrGet<GpodPlaylist> {
public:
	std::vector<std::shared_ptr<GpodTrack>> tracks;
};

#define GPODPLAYLIST(b) GPODBASEPTR_CAST(b, GpodPlaylist)

using _GpodSortMapCont = std::vector<std::shared_ptr<GpodBase>>;

/**
 * @brief lazy-sort gpodbase objects
 * 
 */
class GpodSortContainer {
public:
	GpodSortContainer();
	GpodSortContainer(std::vector<std::shared_ptr<GpodBase>> vec, std::string init = "Name", Gpod *gp);
	
	std::vector<std::shared_ptr<GpodBase>> get(std::string key);
	void sort(std::string key);

	std::string currentName;
	bool ascending;

	friend class Gpod;
protected:
	void addItem(std::shared_ptr<GpodBase> base);
private:
	std::vector<std::shared_ptr<GpodBase>> items;
	std::map<std::string, std::vector<std::shared_ptr<GpodBase>>> sorts;
	Gpod *gpod;
};

class Gpod {
public:
	std::vector<std::shared_ptr<GpodTrack>> tracks;
	std::vector<std::shared_ptr<GpodArtist>> artists;
	std::vector<std::shared_ptr<GpodAlbum>> albums;
	std::map<std::string, std::shared_ptr<GpodArtist>> artistMap;

	GpodSortContainer sortTracks;
	GpodSortContainer sortArtists;
	GpodSortContainer sortAlbums;

	Gpod(std::string dbPath, std::function<void(GError*, bool)> errH);

	void addTrack(std::shared_ptr<GpodTrack> tr);
	void addArtist(std::shared_ptr<GpodArtist> ar);
	void addAlbum(std::shared_ptr<GpodAlbum> al);
	void process(std::function<void(int)> perCb);

	~Gpod();

	friend class GpodTrack;
	friend class GpodAlbum;
	friend class GpodArtist;
	friend class GpodPlaylist;
	friend class GpodSortContainer;
protected:
	Itdb_iTunesDB *itdb = nullptr;
	void throwG(bool fatal);
	void throwG(GError **e, bool fatal);
	void throwG(std::string e, bool fatal);
	std::string path;
	std::function<void(GError*, bool)> errHandle;
	GError *err;
};

typedef std::shared_ptr<GpodBase> GpodBasePtr;
typedef std::shared_ptr<GpodArtwork> GpodArtworkPtr;
typedef std::shared_ptr<GpodTrack> GpodTrackPtr;
typedef std::shared_ptr<GpodAlbum> GpodAlbumPtr;
typedef std::shared_ptr<GpodArtist> GpodArtistPtr;
