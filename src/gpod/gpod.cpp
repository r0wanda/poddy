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

bool GpodSortItem::compare(const GpodSortItem &it) {
	int comp = sortName.compare(it.sortName);
	if (comp != 0 || trackN < 0) return comp > 0;
	return trackN > it.trackN;
}

constexpr std::string GpodBase::typeStr(Type tp) {
	switch (tp) {
	case Type::Track:
		return "track";
	case Type::Artist:
		return "artist";
	case Type::Album:
		return "album";
	case Type::Playlist:
		return "playlist";
	default:
		return "unknown";
	};
}
GpodBase::GpodBase(Type tp, GpodPtr gp): type(tp), name(""), sortName(name), sort({{"Name", { sortName, true }}}), gpod(gp) {
}
GpodBase::GpodBase(Type tp, std::string nm, GpodPtr gp): type(tp), name(nm), sortName(name), sort({{"Name", { sortName, true }}}), gpod(gp) {
}
GpodBase::GpodBase(Type tp, std::string nm, std::string sortNm, GpodPtr gp): type(tp), name(nm), sortName(sortNm), sort({{"Name", { sortName, true }}}), gpod(gp) {
}

std::string GpodBase::display() const {
	return name;
}
void GpodBase::errQInit() {
	std::string nm = "\"" + name + "\" ";
	nm += typeStr(type);
	errq = std::make_shared<PodUtil::ErrorQueue>(nm, gpod->errq);
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

GpodTrack::GpodTrack(Itdb_Track *tr, GpodPtr gp): GpodBase::GpodBase(GpodBase::Type::Track, tr->title, gp), playData(tr), trackData(tr) {
	sort.insert({"Artist", { "", true }});
	sort.insert({"Album", { "", true, static_cast<int>(trackData.trackN) }});
	sort.insert({"Release Date", { std::to_string(tr->year), false }});
	sort.insert({"Most Played", { std::to_string(tr->playcount), false }});
	sort.insert({"Recently Played", { std::to_string(tr->time_played), false }});
}
/*
GpodTrack::GpodTrack(TagLib::FileRef ref, GpodPtr gp): GpodBase::GpodBase(GpodBase::Type::Track, gp) {
	TagLib::Tag *tag = ref.tag();
	Itdb_Track *tr = itdb_track_new();
	if (tag == NULL) {
		// TODO: handle incomplete gpodtracks
		errq->error("file has no tags");
		return;
	}
}*/
void GpodTrack::setArtist(GpodArtistPtr ar) {
	artist = ar;
	sort["Artist"].sortName = album->sortName;
}
void GpodTrack::setAlbum(GpodAlbumPtr al) {
	album = al;
	sort["Album"].sortName = album->sortName;
}

std::string GpodTrack::display() const {
	return artist->name + " - " + name;
}

GpodAlbum::GpodAlbum(const std::string &n, const std::string &sortNm, int year, GpodArtistPtr arst, GpodPtr gp): GpodBase::GpodBase(GpodBase::Type::Album, n, sortNm, gp), artist(arst) {
	artist->addAlbum(getPtr<GpodAlbum>());
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
std::string GpodAlbum::display() const {
	return artist->name + " - " + name;
}

GpodArtist::GpodArtist(const std::string &n, const std::string &sortNm, GpodPtr gp): GpodBase::GpodBase(GpodBase::Type::Artist, n, sortNm, gp),
tracks(std::make_shared<GpodSortContainer>(gp)), albums(std::make_shared<GpodSortContainer>(gp)) {
	sort.insert({"Most Played", { "0", false }});
	sort.insert({"Recently Played", { "0", false }});
}

void GpodArtist::addTrack(GpodTrackPtr tr) {
	tracks->addItem(tr);

	// increment most played
	int plays = std::stoi(sort.at("Most Played").sortName);
	plays += tr->playData.playcount;
	sort["Most Played"].sortName = std::to_string(plays);

	// edit recently played
	unsigned long ts = std::stoul(sort.at("Recently Played").sortName);
	if (ts < tr->playData.ts) sort["Recently Played"].sortName = std::to_string(tr->playData.ts);
}
void GpodArtist::addAlbum(GpodAlbumPtr al, bool addTracks) {
	albums->addItem(al);
	if (addTracks) {
		for (GpodTrackPtr tr : al->tracks) {
			addTrack(tr);
		}
	}
}
std::optional<GpodAlbumPtr> GpodArtist::getAlbum(std::string name) {
	auto al = std::find_if(albums->items.begin(), albums->items.end(), [&name](const std::shared_ptr<GpodBase> &b) -> bool {
		return b->name == name;
	});
	return al == albums->items.end() ? std::nullopt : std::optional<GpodAlbumPtr>((*al)->getPtr<GpodAlbum>());
}
/*std::string GpodArtist::display() const {
	return name;
}*/
void GpodArtist::finalize() {
	tracks->finalize();
	albums->finalize();
}


// gpod sort container
GpodSortContainer::GpodSortContainer(GpodPtr gp): finalized(false), gpod(gp),
	errq(std::make_shared<PodUtil::ErrorQueue>("sort menu", gpod->errq)) {}
GpodSortContainer::GpodSortContainer(std::vector<GpodBasePtr> vec, GpodPtr gp): items(vec), finalized(true), gpod(gp),
	errq(std::make_shared<PodUtil::ErrorQueue>("sort menu", gpod->errq)) {
	if (items.empty()) return;
	initSorts();
}
void GpodSortContainer::initSorts() {
	if (items.empty()) return;
	const GpodSortMap &sortM = items.at(0)->sort;
	std::transform(sortM.cbegin(), sortM.cend(), std::inserter(sorts, sorts.end()), [](const auto &sortMI) {
		return std::pair<std::string, std::vector<GpodBasePtr>>{sortMI.first, {}};
	});
}

std::vector<GpodBasePtr> GpodSortContainer::get(const std::string &key) {
	// if no items
	if (items.empty()) {
		return std::vector<GpodBasePtr>();
	}
	std::vector<GpodBasePtr> curSort;
	try {
		curSort = sorts.at(key);
	} catch (std::out_of_range&) {
		errq->error("invalid sort type");
		return std::vector<GpodBasePtr>();
	}

	// sort if unsorted
	if (curSort.empty()) {
		sort(key);
		curSort = sorts.at(key);
	}
	
	if (key == currentName) {
		ascending = !ascending;
	} else {
		currentName = key;
		ascending = curSort.at(0)->sort.at(key).ascend;
	}

	if (ascending) {
		return sorts.at(key);
	} else {
		return std::vector<GpodBasePtr>(curSort.rbegin(), curSort.rend());
	}
}

void GpodSortContainer::sort(std::string key) {
	sorts.insert({key, items});
	std::vector<GpodBasePtr> &curSort = sorts.at(key);
	std::sort(curSort.begin(), curSort.end(), [&key](const GpodBasePtr &a, const GpodBasePtr &b) -> bool {
		return a->sort.at(key).compare(b->sort.at(key));
	});
}
void GpodSortContainer::addItem(GpodBasePtr base) {
	items.push_back(base);
	if (!finalized) return;
	for (auto &p : sorts) {
		std::vector<GpodBasePtr> &curSort = p.second;
		auto iter = std::upper_bound(curSort.begin(), curSort.end(), base, [&p](const GpodBasePtr &a, const GpodBasePtr &b) -> bool {
			return a->sort.at(p.first).compare(b->sort.at(p.first));
		});
		curSort.insert(iter, base);
	}
}
void GpodSortContainer::finalize() {
	finalized = true;
}
std::vector<std::string> GpodSortContainer::sortKeys() const {
	const auto &keys = std::views::keys(sorts);
	return std::vector<std::string>(keys.begin(), keys.end());
}

// gpod main class
Gpod::Gpod(const std::string &dbPath):
tracks(std::make_shared<GpodSortContainer>(getPtr())), artists(std::make_shared<GpodSortContainer>(getPtr())), albums(std::make_shared<GpodSortContainer>(getPtr())),
path(dbPath) {
	GError *err = nullptr;
    itdb = itdb_parse(path.c_str(), &err);
	if (err) errq->error(err);
}

void Gpod::addTrack(GpodTrackPtr tr) {
	tracks->addItem(tr);
}
void Gpod::addArtist(GpodArtistPtr ar) {
	// TODO: merge artists if duplicate?
	if (artistMap.contains(ar->name)) errq->error("duplicate artist");
	artists->addItem(ar);
	artistMap[ar->name] = ar;
}
void Gpod::addAlbum(GpodAlbumPtr al) {
	albums->addItem(al);
}

void Gpod::process(std::function<void(int)> perCb) {
	int per = 0;
	perCb(per);
	//size_t trLen = g_list_length(itdb->tracks);
	GList *it;
	for (it = itdb->tracks; it != NULL; it = it->next) {
		Itdb_Track *tr = static_cast<Itdb_Track*>(it->data);
		GpodTrackPtr gt = std::make_shared<GpodTrack>(tr, getPtr());
		int year = gt->trackData.year;
		std::string arName(tr->artist);
		std::string alName(tr->album);

		// add to tracks
		addTrack(gt);

		// find or create artist
		GpodArtistPtr artist;
		if (artistMap.find(arName) == artistMap.end()) {
			artist = std::make_shared<GpodArtist>(arName, tr->sort_artist, getPtr());
			addArtist(artist);
		} else {
			artist = artistMap.at(arName);
		}
		artist->addTrack(gt);
		gt->artist = artist;

		// add album to artist, synchronise everything
		GpodAlbumPtr album;
		std::optional<GpodAlbumPtr> alOpt = artist->getAlbum(alName);
		if (alOpt.has_value()) {
			album = alOpt.value();
		} else {
			album = std::make_shared<GpodAlbum>(alName, tr->sort_album, year, artist->getPtr<GpodArtist>(), getPtr());
			addAlbum(album);
		}
		album->addTrack(gt);
		gt->setAlbum(album);
	}
	tracks->finalize();
	artists->finalize();
	// finalize gpodsortcontainers in artists
	for (auto &ar : artists->items) {
		ar->getPtr<GpodArtist>()->finalize();
	}
	albums->finalize();
}

Gpod::~Gpod() {
	itdb_free(itdb);
}

void GpodArtwork::write(GpodTrackPtr tr) {
}
