#pragma once
#include <string>
#include <gpod/itdb.h>

class GpodSong {
public:
	Itdb_Track track;
	std::string title;
	GpodSong();
};

class Gpod {
public:
	Gpod();
};
