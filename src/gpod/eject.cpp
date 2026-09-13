#include <string>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <mntent.h>
#include "fs.hpp"

void eject(std::string path) {
	fs::path inp(path);
	struct mntent *ent;
	FILE *file;
	file = setmntent("/proc/mounts", "r");
	if (!file) {
		std::perror("setmntent");
		return;
	}
	std::string dev;
	while ((ent = getmntent(file))) {
		fs::path dir(ent->mnt_dir);
		if (dir == inp) dev = ent->mnt_fsname;
	}
	endmntent(file);
	if (dev.empty()) return;
	std::string cmd1 = "udisksctl unmount -b " + dev;
	std::string cmd2 = "udisksctl power-off -b " + dev;
	std::system(cmd1.c_str());
	std::system(cmd2.c_str());
}

int main(int argc, char **argv) {
	if (argc < 2) {
		std::cerr << "usage: " << argv[0] << " <mountpoint>" << std::endl;
		return 1;
	}
	eject(argv[1]);
	return 0;
}
