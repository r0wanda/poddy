#include "gpod.hpp"
#include <thread>
#include <memory>
#include <iostream>
#include <taglib/tag.h>
#include <taglib/fileref.h>

GpodBasePtr GpodBase::getBasePtr() {
	return shared_from_this();
}

GpodBase::GpodBase(Gpod *gp): name("untitled"), gpod(gp) {
}
GpodBase::GpodBase(std::string nm, Gpod *gp): name(nm), gpod(gp) {
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

GpodAlbum::GpodAlbum(std::string n): GpodBase::GpodBase(n) {
}

Gpod::Gpod(std::string dbPath, std::function<void(GError*, bool)> errHandle):
path(dbPath), errHandle(errHandle), tracks(), err(nullptr) {
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

		// add to tracks
		tracks.push_back(gt);

		// find or create artist
		GpodArtistPtr artist;
		if (artists.find(arName) == artists.end()) {
			artist = std::make_shared<GpodArtist>(arName);
			artists[arName] = artist;
		} else {
			artist = artists.at(arName);
		}
		artist->tracks.push_back(gt->getPtr());
		gt->artist = artist->getPtr();

		// add album to artist, synchronise everything
		GpodAlbumPtr album;
		if (artist->albums.find(alName) == artist->albums.end()) {
			album = std::make_shared<GpodAlbum>(alName);
			artist->albums[alName] = album;
		} else {
			album = artist->albums.at(alName);
		}
		albums.push_back(album->getPtr());
		album->tracks.push_back(gt);
		album->artist = artist;
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
		tr.second.reset();
	}
	for (auto &al : albums) {
		al.reset();
	}
}

void GpodArtwork::write(GpodTrack *tr) {
}

#undef SHARED
