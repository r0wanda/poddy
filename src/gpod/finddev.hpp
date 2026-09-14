#pragma once
#include <string>
#include <vector>
#include "../config.h"

#ifdef IS_LINUX
#include <memory>
#include <functional>
#include <udisks/udisks.h>
#endif

/**
 * idea: windows + linux: use finddev to generate a list of devices and connect to them
 * macos: open finder window to select drive
 */

class DiskDev {
public:
	std::string path;
	std::string name;
	std::string disp;
	// 0 = prob 1 = maybe 2 = nah
	char isIpod;
	bool _discard;
#ifdef IS_LINUX
	//std::shared_ptr<UDisksClient> client;
	UDisksFilesystem *dev;
	DiskDev(UDisksBlock *b, UDisksFilesystem *fs);
#endif
	void connect(std::function<void(std::string)> cb);
	~DiskDev();
};

#ifdef IS_LINUX
std::vector<DiskDev*> finddev();
#endif
