#include "gpod.hpp"
#include <map>
#include <vector>
#include <thread>
#include <memory>
#include <string>
#include <optional>
#include <iostream>
#include <taglib/tag.h>
#include <taglib/fileref.h>

GpodTrackPtr GpodTrack::getPtr() {
	return std::dynamic_pointer_cast<GpodTrack>(getBasePtr());
}
GpodAlbumPtr GpodAlbum::getPtr() {
	return std::dynamic_pointer_cast<GpodAlbum>(getBasePtr());
}
GpodArtistPtr GpodArtist::getPtr() {
	return std::dynamic_pointer_cast<GpodArtist>(getBasePtr());
}

bool GpodSortItem::compare(const GpodSortItem &it) {
	int comp = sortName.compare(it.sortName);
	if (comp != 0 || trackN < 0) return comp > 0;
	return trackN > it.trackN;
}

GpodBasePtr GpodBase::getBasePtr() {
	return shared_from_this();
}

GpodBase::GpodBase(Gpod *gp): name("untitled"), sortName(name), sort({{"Name", { sortName, true }}}), gpod(gp) {
}
GpodBase::GpodBase(std::string nm, Gpod *gp): name(nm), sortName(name), sort({{"Name", { sortName, true }}}), gpod(gp) {
}
GpodBase::GpodBase(std::string nm, std::string sortNm, Gpod *gp): name(nm), sortName(sortNm), sort({{"Name", { sortName, true }}}), gpod(gp) {
}

std::string GpodBase::display() const {
	return name;
}

// macro to assign track properties
#define _gts(n) n(tr->n)
GpodTrackData::GpodTrackData(Itdb_Track *tr):
	_gts(genre),
	_gts(filetype),
	_gts(comment),
	_gts(category),
	_gts(composer),
	_gts(description),
	_gts(id),
	_gts(size),
	// bounds check (in case any of these values are < 0 for some reason)
	tracklen(std::max(tr->tracklen, 0)),
	cdN(tr->cd_nr),
	_gts(cds),
	trackN(std::max(tr->track_nr, 0)),
	tracks(std::max(tr->tracks, 0)),
	_gts(bitrate),
	_gts(samplerate),
	_gts(year),
	_gts(volume),
	_gts(soundcheck) {}

GpodTrackPlayData::GpodTrackPlayData(Itdb_Track *tr):
	_gts(playcount), recentPlaycount(tr->recent_playcount), ts(tr->time_played) {}
#undef _gts

GpodTrack::GpodTrack(Itdb_Track *tr, Gpod *gp): GpodBase::GpodBase(tr->title, gp), playData(tr), trackData(tr) {
	sort.insert({"Artist", { "", true }});
	sort.insert({"Album", { "", true, static_cast<int>(trackData.trackN) }});
	sort.insert({"Release Date", { std::to_string(tr->year), false }});
	sort.insert({"Most Played", { std::to_string(tr->playcount), false }});
	sort.insert({"Recently Played", { std::to_string(tr->time_played), false }});
}
GpodTrack::GpodTrack(TagLib::FileRef ref, Gpod *gp): GpodBase::GpodBase(gp) {
	TagLib::Tag *tag = ref.tag();
	Itdb_Track *tr = itdb_track_new();
	if (tag == NULL) {
		// TODO: handle incomplete gpodtracks
		gp->throwG("file has no tags", false);
		return;
	}
}
void GpodTrack::setArtist(GpodArtistPtr ar) {
	artist = ar->getPtr();
	sort["Artist"].sortName = album->sortName;
}
void GpodTrack::setAlbum(GpodAlbumPtr al) {
	album = al->getPtr();
	sort["Album"].sortName = album->sortName;
}

std::string GpodTrack::display() const {
	return artist->name + " - " + name;
}

GpodAlbum::GpodAlbum(std::string n, std::string sortNm, int year, GpodArtistPtr arst, Gpod *gp): GpodBase::GpodBase(n, sortNm, gp), artist(arst) {
	artist->addAlbum(getPtr());
	sort.insert({"Artist", { artist->sortName, true }});
	sort.insert({"Release Date", { std::to_string(year), false }});
	sort.insert({"Most Played", { "0", false }});
	sort.insert({"Recently Played", { "0", false }});
}

void GpodAlbum::addTrack(GpodTrackPtr tr) {
	tracks.insert(tracks.begin() + std::min(static_cast<unsigned long>(tr->trackData.trackN), tracks.size() - 1), tr);

	// increment most played
	int plays = std::stoi(sort.at("Most Played").sortName);
	plays += tr->playData.playcount;
	sort["Most Played"].sortName = std::to_string(plays);

	// edit recently played
	unsigned long ts = std::stoul(sort.at("Recently Played").sortName);
	if (ts < tr->playData.ts) sort["Recently Played"].sortName = std::to_string(tr->playData.ts);
}

GpodArtist::GpodArtist(std::string n, std::string sortNm, Gpod *gp): GpodBase::GpodBase(n, sortNm, gp), tracks(gp), albums(gp) {
	sort.insert({"Most Played", { "0", false }});
	sort.insert({"Recently Played", { "0", false }});
}

void GpodArtist::addTrack(GpodTrackPtr tr) {
	tracks.addItem(tr);

	// increment most played
	int plays = std::stoi(sort.at("Most Played").sortName);
	plays += tr->playData.playcount;
	sort["Most Played"].sortName = std::to_string(plays);

	// edit recently played
	unsigned long ts = std::stoul(sort.at("Recently Played").sortName);
	if (ts < tr->playData.ts) sort["Recently Played"].sortName = std::to_string(tr->playData.ts);
}
void GpodArtist::addAlbum(GpodAlbumPtr al, bool addTracks) {
	albums.addItem(al);
	if (addTracks) {
		for (GpodTrackPtr tr : al->tracks) {
			addTrack(tr);
		}
	}
}
std::optional<GpodAlbumPtr> GpodArtist::getAlbum(std::string name) {
	auto al = std::find_if(albums.items.begin(), albums.items.end(), [&name](const std::shared_ptr<GpodBase> &b) -> bool {
		return b->name == name;
	});
	return al == albums.items.end() ? std::nullopt : std::optional<GpodAlbumPtr>(GPODALBUM(*al));
}


// gpod sort container
GpodSortContainer::GpodSortContainer(Gpod *gp): finalized(false), gpod(gp) {}
GpodSortContainer::GpodSortContainer(std::vector<GpodBasePtr> vec, Gpod *gp): items(vec), finalized(true), gpod(gp) {
	for (const auto &p : items.at(0)->sort) {
		sorts.insert({p.first, {}});
	}
}
std::vector<GpodBasePtr> GpodSortContainer::get(std::string key) {
	if (!sorts.contains(key)) {
		gpod->throwG("invalid sort type", false);
		return std::vector<GpodBasePtr>();
	}
	
	if (key == currentName) {
		ascending = !ascending;
	} else {
		currentName = key;
	}

	// sort if unsorted
	if (sorts.at(key).empty()) sort(key);

	if (ascending) {
		return sorts.at(key);
	} else {
		return std::vector<GpodBasePtr>(sorts.at(key).rbegin(), sorts.at(key).rend());
	}
}
void GpodSortContainer::sort(std::string key) {
	sorts[key] = items;
	std::sort(sorts.at(key).begin(), sorts.at(key).end(), [&key](const GpodBasePtr &a, const GpodBasePtr &b) -> bool {
		return a->sort.at(key).compare(b->sort.at(key));
	});

	currentName = key;
	ascending = sorts.at(key).at(0)->sort.at(key).ascend;
}
void GpodSortContainer::addItem(GpodBasePtr base) {
	items.push_back(base);
	if (!finalized) return;
	for (const auto &p : sorts) {
		const std::string &key = p.first;
		auto iter = std::upper_bound(sorts.at(key).begin(), sorts.at(key).end(), base, [&key](const GpodBasePtr &a, const GpodBasePtr &b) -> bool {
			return a->sort.at(key).compare(b->sort.at(key));
		});
		sorts.at(key).insert(iter, base);
	}
}
void GpodSortContainer::finalize() {
	finalized = true;
}


// gpod main class
Gpod::Gpod(std::string dbPath, const std::function<void(GError*, bool)> &errH):
tracks(this), artists(this), albums(this), path(dbPath), errHandle(errH), err(nullptr) {
    itdb = itdb_parse(path.c_str(), &err);
	throwG(true);
}

void Gpod::addTrack(GpodTrackPtr tr) {
	tracks.addItem(tr->getPtr());
}
void Gpod::addArtist(GpodArtistPtr ar) {
	if (artistMap.contains(ar->name)) throwG("duplicate artist", false);
	artists.addItem(ar->getPtr());
	artistMap[ar->name] = ar;
}
void Gpod::addAlbum(GpodAlbumPtr al) {
	albums.addItem(al);
}

void Gpod::process(std::function<void(int)> perCb) {
	int per = 0;
	perCb(per);
	//size_t trLen = g_list_length(itdb->tracks);
	GList *it;
	for (it = itdb->tracks; it != NULL; it = it->next) {
		Itdb_Track *tr = (Itdb_Track*)it->data;
		GpodTrackPtr gt = std::make_shared<GpodTrack>(tr, this);
		int year = gt->trackData.year;
		std::string arName(tr->artist);
		std::string alName(tr->album);
		std::string arSortName(tr->sort_artist);
		std::string alSortName(tr->sort_album);

		// add to tracks
		addTrack(gt);

		// find or create artist
		GpodArtistPtr artist;
		if (artistMap.find(arName) == artistMap.end()) {
			artist = std::make_shared<GpodArtist>(arName, arSortName, this);
			addArtist(artist);
		} else {
			artist = artistMap.at(arName);
		}
		artist->addTrack(gt->getPtr());
		gt->artist = artist->getPtr();

		// add album to artist, synchronise everything
		GpodAlbumPtr album;
		std::optional<GpodAlbumPtr> alOpt = artist->getAlbum(alName);
		if (alOpt.has_value()) {
			album = alOpt.value();
		} else {
			album = std::make_shared<GpodAlbum>(alName, alSortName, year, artist->getPtr(), this);
			addAlbum(album);
		}
		album->addTrack(gt);
		gt->setAlbum(album);
	}
	tracks.finalize();
	artists.finalize();
	albums.finalize();
}

void Gpod::throwG(bool fatal) {
	if (!err) return;
	if (err->message) errHandle(err, fatal);
	g_error_free(err);
	err = nullptr;
}
void Gpod::throwG(GError **e, bool fatal) {
	if ((*e)->message) errHandle(*e, fatal);
	g_error_free(*e);
	*e = nullptr;
}
void Gpod::throwG(std::string e, bool fatal) {
	GError *gerr = g_error_new(g_option_error_quark(), 1, e.c_str());
	errHandle(gerr, fatal);
	g_error_free(gerr);
}

Gpod::~Gpod() {
	itdb_free(itdb);
}

void GpodArtwork::write(GpodTrack *tr) {
}
