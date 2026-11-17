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
#include "eject.hpp"

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
GpodTrack::GpodTrack(TagLib::FileRef ref, GpodPtr gp): GpodBase::GpodBase(GpodBase::Type::Track, gp) {
	TagLib::Tag *tag = ref.tag();
	Itdb_Track *tr = itdb_track_new();
	if (tag == NULL) {
		// TODO: handle incomplete gpodtracks
		errq->error("file has no tags");
		return;
	}
}
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

GpodAlbum::GpodAlbum(std::string n, std::string sortNm, int year, GpodArtistPtr arst, GpodPtr gp): GpodBase::GpodBase(GpodBase::Type::Album, n, sortNm, gp), artist(arst) {
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

GpodArtist::GpodArtist(std::string n, std::string sortNm, GpodPtr gp): GpodBase::GpodBase(GpodBase::Type::Artist, n, sortNm, gp),
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
	for (const auto &p : items.at(0)->sort) {
		sorts.insert({p.first, {}});
	}
}

std::vector<GpodBasePtr> GpodSortContainer::get(std::string key) {
	if (!sorts.contains(key)) {
		errq->error("invalid sort type");
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
std::vector<std::string> GpodSortContainer::sortKeys() const {
	auto keys = std::views::keys(sorts);
	return std::vector<std::string>(keys.begin(), keys.end());
}

// gpod main class
Gpod::Gpod(std::string dbPath):
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
		Itdb_Track *tr = (Itdb_Track*)it->data;
		GpodTrackPtr gt = std::make_shared<GpodTrack>(tr, getPtr());
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
			artist = std::make_shared<GpodArtist>(arName, arSortName, getPtr());
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
			album = std::make_shared<GpodAlbum>(alName, alSortName, year, artist->getPtr<GpodArtist>(), getPtr());
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
void Gpod::eject() {
	eject(path);
	itdb_free(itdb);
	itdb = nullptr;
}

Gpod::~Gpod() {
}

void GpodArtwork::write(GpodTrackPtr tr) {
}
