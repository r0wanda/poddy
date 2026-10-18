#include "gpod.hpp"
#include <thread>
#include <memory>
#include <iostream>
#include <taglib/tag.h>
#include <taglib/fileref.h>

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

GpodTrack::GpodTrack(Itdb_Track *tr, Gpod *gp): GpodBase::GpodBase(tr->title, gp) {
	playData = GpodTrackPlayData{tr->playcount, tr->recent_playcount, tr->time_played};
	sort.insert({"Artist", { artist->sortName, true }});
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

std::string GpodTrack::display() const {
	return artist->name + " - " + name;
}

GpodAlbum::GpodAlbum(std::string n, std::string sortNm, int year, GpodArtistPtr arst, Gpod *gp): GpodBase::GpodBase(n, sortNm, gp), artist(arst) {
	sort.insert({"Artist", { artist->sortName, true }});
	sort.insert({"Release Date", { std::to_string(year), false }});
	sort.insert({"Most Played", { 0, false }});
	sort.insert({"Recently Played", { 0, false }});
}

void GpodAlbum::addTrack(GpodTrackPtr tr) {
	tracks.push_back(tr);
}

void GpodArtist::addTrack(GpodTrackPtr tr) {

}
void GpodArtist::addAlbum(std::shared_ptr<GpodAlbum> al) {

}

GpodSortContainer::GpodSortContainer(): items(), gpod(nullptr) {}
GpodSortContainer::GpodSortContainer(std::vector<std::shared_ptr<GpodBase>> vec, std::string init = "Name", Gpod *gp): items(vec), gpod(gp) {
	for (const auto &p : items.at(0)->sort) {
		sorts.insert({p.first, {}});
	}
	sort(init);
}

std::vector<std::shared_ptr<GpodBase>> GpodSortContainer::get(std::string key) {
	if (!sorts.contains(key)) {
		gpod->throwG("invalid sort type", false);
		return std::vector<std::shared_ptr<GpodBase>>();
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
		return std::vector<std::shared_ptr<GpodBase>>(sorts.at(key).rbegin(), sorts.at(key).rend());
	}
}

void GpodSortContainer::sort(std::string key) {
	sorts[key] = items;
	std::sort(sorts.at(key).begin(), sorts.at(key).end(), [&key](const std::shared_ptr<GpodBase> &a, const std::shared_ptr<GpodBase> &b) -> bool {
		return a->sort.at(key).sortName.compare(b->sort.at(key).sortName) > 0;
	});

	currentName = key;
	ascending = sorts.at(key).at(0)->sort.at(key).ascend;
}
void GpodSortContainer::addItem(std::shared_ptr<GpodBase> base) {
	items.push_back(base);
	for (const auto &p : sorts) {
		const std::string &key = p.first;
		auto iter = std::upper_bound(sorts.at(key).begin(), sorts.at(key).end(), base, [&key](const std::shared_ptr<GpodBase> &a, const std::shared_ptr<GpodBase> &b) -> bool {
			return a->sort.at(key).sortName.compare(b->sort.at(key).sortName) > 0;
		});
		sorts.at(key).insert(iter, base);
	}
}

Gpod::Gpod(std::string dbPath, std::function<void(GError*, bool)> errH):
path(dbPath), errHandle(errH), tracks(), err(nullptr) {
    itdb = itdb_parse(path.c_str(), &err);
	throwG(true);
}

void Gpod::addTrack(std::shared_ptr<GpodTrack> tr) {
	if (sortTracks.gpod == this) sortTracks.addItem(tr->getBasePtr());

}
void Gpod::addArtist(std::shared_ptr<GpodArtist> ar) {
	if (artistMap.contains(ar->name)) throwG("duplicate artist", false);
	artists.push_back(ar);
	artistMap[ar->name] = ar;
}
void Gpod::addAlbum(std::shared_ptr<GpodAlbum> al);

void Gpod::process(std::function<void(int)> perCb) {
	int per = 0;
	perCb(per);
	//size_t trLen = g_list_length(itdb->tracks);
	GList *it;
	for (it = itdb->tracks; it != NULL; it = it->next) {
		Itdb_Track *tr = (Itdb_Track*)it->data;
		GpodTrackPtr gt = std::make_shared<GpodTrack>(tr, this);
		std::string arName(tr->artist);
		std::string alName(tr->album);
		std::string arSortName(tr->sort_artist);
		std::string alSortName(tr->sort_album);

		// add to tracks
		addTrack(gt);

		// find or create artist
		GpodArtistPtr artist;
		if (artistMap.find(arName) == artistMap.end()) {
			artist = std::make_shared<GpodArtist>(arName, this);
			addArtist(artist);
		} else {
			artist = artists.at(arName);
		}
		artist->addTrack(gt->getPtr());
		gt->artist = artist->getPtr();

		// add album to artist, synchronise everything
		GpodAlbumPtr album;
		if (artist->albums.find(alName) == artist->albums.end()) {
			album = std::make_shared<GpodAlbum>(alName, alSortName, artist->getPtr(), this);
			artist->albums[alName] = album;
		} else {
			album = artist->albums.at(alName);
		}
		albums.push_back(album->getPtr());
		album->tracks.push_back(gt);
		gt->album = album->getPtr();
	}
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
	for (auto &tr : tracks) {
		tr.reset();
	}
	for (auto &ar : artists) {
		ar.second.reset();
	}
	for (auto &al : albums) {
		al.reset();
	}
}

void GpodArtwork::write(GpodTrack *tr) {
}
