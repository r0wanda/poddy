#pragma once
#include <string>
#include <vector>
#include <map>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include "../config.hpp"

#ifdef IS_LINUX
#include <memory>
#include <functional>
#include <udisks/udisks.h>
#endif

/**
 * idea: windows + linux: use finddev to generate a list of devices and connect to them
 * macos: open finder window to select drive
 */

class FindDev;

class DiskDev {
public:
	std::string uuid;
	std::string path;
	std::string name;
	// 0 = prob 1 = maybe 2 = nah
	char isIpod = 2;
	bool _discard;
#ifdef IS_LINUX
	UDisksFilesystem *dev;
	DiskDev(UDisksBlock *b, UDisksFilesystem *fs, FindDev *fd);
#endif
	std::string connect();
	~DiskDev();
private:
	FindDev *findd;
};
typedef std::shared_ptr<DiskDev> DiskDevPtr;

class FindDev {
public:
	DiskDev *connected = nullptr;
	std::vector<DiskDevPtr> devs;
	FindDev();
	void refresh();
	ftxui::Component popup;
	~FindDev();
	friend class DiskDev;
private:
	GError *err;
	UDisksClient *client;
	GDBusObjectManager *manager;
	ftxui::Component menu;
	int menuSel;
};
