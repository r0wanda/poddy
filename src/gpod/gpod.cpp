#include "gpod.hpp"
#include <iostream>
#include <thread>
#include <taglib/tag.h>
#include <taglib/fileref.h>

GpodTrack::GpodTrack(Itdb_Track *tr, Gpod *gp): title(tr->title), playcount(), gpod(gp) {}
GpodTrack::GpodTrack(TagLib::FileRef ref, Gpod *gp): gpod(gp) {
	TagLib::Tag *tag = ref.tag();
	Itdb_Track *tr = itdb_track_new();
	if (tag == NULL) {
		// TODO: handle incomplete gpodtracks
		gp->throwG(g_error_new(G_FILE_ERROR, 1, "file has no tags"));
		return;
	}
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
		GpodTrack *gt = new GpodTrack(tr, this);
		std::string arName(tr->artist);
		std::string alName(tr->album);

		// add to tracks
		tracks.push_back(gt);

		// find or create artist
		GpodArtist *artist;
		if (artists.find(arName) == artists.end()) {
			artist = new GpodArtist(arName);
			artists[arName] = artist;
		} else {
			artist = artists.at(arName);
		}
		artist->tracks.push_back(gt);
		gt->artist = artist;

		// add album to artist, synchronise everything
		GpodAlbum *album;
		if (artist->albums.find(alName) == artist->albums.end()) {
			album = new GpodAlbum(alName);
			artist->albums[alName] = album;
		} else {
			album = artist->albums.at(alName);
		}
		album->tracks.push_back(gt);
		album->artist = artist;
		gt->album = album;
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
		delete tr;
	}
	for (auto &ar : artists) {
		delete ar.second;
	}
	for (auto &al : albums) {
		delete al.second;
	}
}

void GpodArtwork::write(GpodTrack *tr) {
}
