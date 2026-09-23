#pragma once
#include <map>
#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include "../config.hpp"
#include "../tui/tui.hpp"

#ifdef IS_LINUX
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
	FindDev(Tui *_tui);
	void start();
	void stop();
	void refresh();
	ftxui::Component popup;
	~FindDev();
	friend class DiskDev;
private:
#ifdef IS_LINUX
	GError *err;
	UDisksClient *client;
	GDBusObjectManager *manager;
#endif
	Tui *tui;
	ftxui::Component menu;
	ftxui::Element connectDialog;
	int menuSel;
	std::jthread notifLoop;
	void setupPopup();
};
