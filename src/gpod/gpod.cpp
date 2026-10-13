#include "gpod.hpp"
#include <thread>
#include <memory>
#include <iostream>
#include <taglib/tag.h>
#include <taglib/fileref.h>

GpodBasePtr GpodBase::getBasePtr() {
	return shared_from_this();
}

GpodBase::GpodBase(Gpod *gp): name("untitled"), sortName(name), sort({{"Name", sortName}}), gpod(gp) {
}
GpodBase::GpodBase(std::string nm, Gpod *gp): name(nm), sortName(name), sort({{"Name", sortName}}), gpod(gp) {
}
GpodBase::GpodBase(std::string nm, std::string sortNm, Gpod *gp): name(nm), sortName(sortNm), sort({{"Name", sortName}}), gpod(gp) {
}

std::string GpodBase::display() {
	return name;
}

GpodTrack::GpodTrack(Itdb_Track *tr, Gpod *gp): GpodBase::GpodBase(tr->title, gp), playcount() {}
GpodTrack::GpodTrack(TagLib::FileRef ref, Gpod *gp): GpodBase::GpodBase(gp) {
	TagLib::Tag *tag = ref.tag();
	Itdb_Track *tr = itdb_track_new();
	if (tag == NULL) {
		// TODO: handle incomplete gpodtracks
		gp->throwG(g_error_new(G_FILE_ERROR, 1, "file has no tags"));
		return;
	}
}

std::string GpodTrack::display() {
	return artist->name + " - " + name;
}

GpodAlbum::GpodAlbum(std::string n, std::string sortNm, int year, GpodArtistPtr arst, Gpod *gp): GpodBase::GpodBase(n, sortNm, gp), artist(arst) {
	sort.insert({"Artist", artist->sortName});
	sort.insert({"Release Date", std::to_string(year)});
	sort.insert({"Recently Played", })
}

std::string GpodArtist::display() {
	return artist->name + " - " + name;
}

Gpod::Gpod(std::string dbPath, std::function<void(GError*, bool)> errH):
path(dbPath), errHandle(errH), tracks(), err(nullptr) {
    itdb = itdb_parse(path.c_str(), &err);
	throwG(true);
}

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
		tracks.push_back(gt);

		// find or create artist
		GpodArtistPtr artist;
		if (artists.find(arName) == artists.end()) {
			artist = std::make_shared<GpodArtist>(arName, this);
			artists[arName] = artist;
		} else {
			artist = artists.at(arName);
		}
		artist->tracks.push_back(gt->getPtr());
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

#undef SHARED
