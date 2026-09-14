#include "gpod.hpp"
#include <iostream>
#include <

GpodTrack::GpodTrack() {}

Gpod::Gpod(std::string dbPath, std::function<void(GError*, bool)> errHandle):
path(dbPath), errorHandle(errHandle), err(nullptr), tracks() {
    itdb = itdb_parse(path, &err);
	throwG(true);
}

void Gpod::process(std::function<void(int)> perCb) {
	size_t trLen = g_list_length(itdb->tracks)
	GList *it;
	for (it = itdb->tracks; it != NULL; it = it->next) {
		Itdb_Track *tr = (Itdb_Track*)it->data;
		tracks.push_back(GpodTrack(tr));
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
}
