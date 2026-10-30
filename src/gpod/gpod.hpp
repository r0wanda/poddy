#pragma once
#include <map>
#include <ctime>
#include <string>
#include <vector>
#include <memory>
#include <ranges>
#include <optional>
#include <algorithm>
#include <functional>
#include <type_traits>
#include <gpod/itdb.h>
#include <taglib/tag.h>
#include <taglib/fileref.h>
#include "../util.hpp"


class GpodArtist;
class GpodAlbum;
class GpodTrack;
class Gpod;
class GpodSortContainer;

struct GpodSortItem {
	std::string sortName;
	bool ascend = true;
	int trackN = -1;
	bool compare(const GpodSortItem &it);
};
/**
 * @brief Gpod* sorting methods
 * { display name, { string to sort, ascending? } }
 */
typedef std::map<const std::string, GpodSortItem> GpodSortMap;

/**
 * @brief gpod base class
 * 
 */
class GpodBase : public PodUtil::GetPtr<GpodBase> {
public:
	GpodBase(std::shared_ptr<Gpod> gp);
	GpodBase(std::string nm, std::shared_ptr<Gpod> gp);
	GpodBase(std::string nm, std::string sortNm, std::shared_ptr<Gpod> gp);
	const std::string name;
	const std::string sortName;
	virtual std::string display() const;
	GpodSortMap sort;
	virtual ~GpodBase() = default;

	friend class Gpod;
private:
	std::shared_ptr<Gpod> gpod = nullptr;
};

/**
 * @brief lazy-sort gpodbase objects
 * 
 */
class GpodSortContainer {
public:
	GpodSortContainer(std::shared_ptr<Gpod> gp);
	GpodSortContainer(std::vector<std::shared_ptr<GpodBase>> vec, std::shared_ptr<Gpod> gp);
	
	std::vector<std::shared_ptr<GpodBase>> get(std::string key);
	void sort(std::string key);
	void addItem(std::shared_ptr<GpodBase> base);
	void finalize();

	std::string currentName;
	bool ascending;
	std::vector<std::shared_ptr<GpodBase>> items;
	
	friend class Gpod;
	friend class GpodBase;
protected:
	bool finalized;
private:
	std::map<std::string, std::vector<std::shared_ptr<GpodBase>>> sorts;
	std::shared_ptr<Gpod> gpod;
};


class GpodArtwork : public GpodBase {
public:

	void write(GpodTrack *tr);
protected:
	Itdb_Artwork *art = nullptr;
};

struct GpodTrackData {
	GpodTrackData() = default;
	GpodTrackData(Itdb_Track *tr);
	std::string genre;
	std::string filetype;
	std::string comment;
	std::string category;
	std::string composer;
	std::string description;
	int id;
	unsigned int size;
	unsigned int tracklen;
	int cdN;
	int cds;
	unsigned int trackN;
	unsigned int tracks;
	int bitrate;
	int samplerate;
	int year;
	int volume;
	int soundcheck;
};
/*struct GpodTrackSort {
	std::string sort_title;
	std::string sort_composer;
	std::string sort_tvshow;
};*/
struct GpodTrackPlayData {
	GpodTrackPlayData() = default;
	GpodTrackPlayData(Itdb_Track *tr);
	unsigned short playcount;
	unsigned short recentPlaycount;
	unsigned long ts;
};

class GpodTrack : public GpodBase {
public:
	std::shared_ptr<GpodArtist> artist;
	std::shared_ptr<GpodAlbum> album;
	GpodTrackPlayData playData;
	GpodTrackData trackData;
	
	GpodTrack(Itdb_Track *tr, std::shared_ptr<Gpod> gp);
	GpodTrack(TagLib::FileRef ref, std::shared_ptr<Gpod> gp);
	void setArtist(std::shared_ptr<GpodArtist> ar);
	void setAlbum(std::shared_ptr<GpodAlbum> al);
	std::string display() const override;
protected:
	Itdb_Track *track = nullptr;
};

class GpodAlbum : public GpodBase {
public:
	std::shared_ptr<GpodArtist> artist;
	// not gpodsortcontainer bc album tracks have a set order
	// maybe in future add sorting in album view? cant think of a reason tho
	std::vector<std::shared_ptr<GpodTrack>> tracks;

	GpodAlbum(std::string n, std::string sortNm, int year, std::shared_ptr<GpodArtist> arst, std::shared_ptr<Gpod> gp);

	void addTrack(std::shared_ptr<GpodTrack> tr);

	std::string display() const override;
protected:
	void finalize();
};

struct GpodArtistSort {
	std::string sort_artist;
};

class GpodArtist : public GpodBase {
public:
	GpodSortContainer tracks;
	GpodSortContainer albums;

	GpodArtist(std::string n, std::string sortNm, std::shared_ptr<Gpod> gp);

	void addTrack(std::shared_ptr<GpodTrack> tr);
	void addAlbum(std::shared_ptr<GpodAlbum> al, bool addTracks = false);
	std::optional<std::shared_ptr<GpodAlbum>> getAlbum(std::string name);
	std::string display() const override;
protected:
	void finalize();
};

#define GPODARTIST(b) GPODBASEPTR_CAST(b, GpodArtist)

class GpodPlaylist : public GpodBase {
public:
	std::vector<std::shared_ptr<GpodTrack>> tracks;
};

#define GPODPLAYLIST(b) GPODBASEPTR_CAST(b, GpodPlaylist)


class Gpod : public PodUtil::GetPtr<Gpod> {
public:
	GpodSortContainer tracks;
	GpodSortContainer artists;
	GpodSortContainer albums;
	std::map<std::string, std::shared_ptr<GpodArtist>> artistMap;

	Gpod(std::string dbPath, const std::function<void(GError*, bool)> &errH);

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
	const std::function<void(GError*, bool)> &errHandle;
	GError *err;
};

typedef std::shared_ptr<Gpod> GpodPtr;
typedef std::shared_ptr<GpodBase> GpodBasePtr;
typedef std::shared_ptr<GpodArtwork> GpodArtworkPtr;
typedef std::shared_ptr<GpodTrack> GpodTrackPtr;
typedef std::shared_ptr<GpodAlbum> GpodAlbumPtr;
typedef std::shared_ptr<GpodArtist> GpodArtistPtr;
