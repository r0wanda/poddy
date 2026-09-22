#include "finddev.hpp"
#include "../config.hpp"
#include <iostream>
#include <string>
#include <ftxui/screen/color.hpp>

#ifdef IS_LINUX
#include <udisks/udisks.h>
#include <glib.h>
#include <regex>
#include <algorithm>

bool searchIpod(std::string str) {
	if (str.empty()) return false;
	// case-insensitive search for ipod in name (https://stackoverflow.com/a/19839371)
	static const std::regex rex("ipod", std::regex::icase);
	return std::regex_search(str, rex);
}

DiskDev::DiskDev(UDisksBlock *b, UDisksFilesystem *fs, FindDev *fd): dev(fs), findd(fd) {
	uuid = udisks_block_get_id_uuid(b);
	path = udisks_block_get_device(b);
	std::string _name = udisks_block_get_id_label(b);
	name = _name;
	std::string id = udisks_block_get_drive(b);
	g_object_unref(b);
	if (name.empty()) {
		if (id.empty()) {
			name = "unnamed disk";
			isIpod = 2;
			return;
		} else name = id;
	} else if (searchIpod(id) || searchIpod(_name)) isIpod = 0;
	else isIpod = 1;
}

FindDev::FindDev(): err(nullptr), menuSel(0) {
	static const std::vector<std::string> icons{"✓", "?", "✕"};
    client = udisks_client_new_sync(nullptr, &err);
    
    if (err != nullptr) {
		// TODO: error system
        std::cerr << err->message << std::endl;
        g_clear_error(&err);
    }

    manager = udisks_client_get_object_manager(client);
	refresh();

	using namespace ftxui;
	Components ents;
	for (DiskDevPtr &dd : devs) {
		MenuEntryOption opt;
		opt.transform = [dd](EntryState st) {
			st.label = (st.active ? "> " : "  ") + st.label;
			Element e = text(st.label);
			switch (dd->isIpod) {
			case 0:
				e = hbox({
					e, text(" "),
					text(icons[0]) | (st.focused ? bgcolor(Color::Green) : color(Color::Green))
				});
				break;
			case 1:
				e = hbox({
					e, text(" "),
					text(icons[1]) | (st.focused ? bgcolor(Color::Yellow) : color(Color::Yellow))
				});
				break;
			default:
				e = hbox({
					e, text(" "),
					text(icons[2]) | (st.focused ? bgcolor(Color::Red) : color(Color::Red))
				});
				if (!st.focused) e |= color(Color::GrayDark);
			}
			if (st.focused) e |= inverted;
			return e;
		};
		std::string txt = dd->path + ": " + dd->name;
		ents.push_back(MenuEntry(txt, opt));
	}
	menu = Container::Vertical(ents, &menuSel);
	popup = Renderer(menu, [&] {
		return vbox({
			hbox({text(devs[menuSel]->name) | bold}),
			menu->Render() | frame | size(HEIGHT, LESS_THAN, 10)
		});
	});
}
void FindDev::refresh() {
	std::map<std::string, DiskDevPtr> ndevs;
	GList *objects = g_dbus_object_manager_get_objects(manager);

    for (GList *l = objects; l != nullptr; l = l->next) {
        UDisksObject *object = UDISKS_OBJECT(l->data);
        UDisksBlock *block = udisks_object_get_block(object);
		UDisksFilesystem *fs = udisks_object_get_filesystem(object);
		if (block == nullptr || fs == nullptr) {
			if (block) g_object_unref(block);
			if (fs) g_object_unref(fs);
			continue;
		}

		std::string uuid(udisks_block_get_id_uuid(block));
		if (std::find_if(devs.begin(), devs.end(), [&uuid](DiskDevPtr const &d) -> bool {
			return d->uuid == uuid;
		}) != devs.end()) continue;
		DiskDevPtr ddev(new DiskDev(block, fs, this));
		ndevs.insert({uuid, ddev});
    }
	for (auto it = devs.begin(); it != devs.end(); it++) {
		if (ndevs.contains((*it)->uuid)) {
			it->reset();
			devs.erase(it);
		}
	}
	for (auto &dd : ndevs) {
		devs.push_back(dd.second);
	}
    g_list_free_full(objects, g_object_unref);
	std::sort(devs.begin(), devs.end(), [](DiskDevPtr const &a, DiskDevPtr const &b) {
		return a->isIpod < b->isIpod;
	});
}

FindDev::~FindDev() {
	g_object_unref(client);
	for (auto &dd : devs) {
		dd.reset();
	}
}

struct LoopCb {
	GMainLoop *loop;
	std::function<void(std::string)> *cb;
};

std::string DiskDev::connect() {
	const gchar *const *mp = udisks_filesystem_get_mount_points(dev);
	if (mp != nullptr) {
		if (mp[0] != nullptr) return std::string(mp[0]);
	}
	GVariantBuilder bd;
	g_variant_builder_init(&bd, G_VARIANT_TYPE_VARDICT);
	GVariant *opt = g_variant_builder_end(&bd);

	gchar *mountPath;
	std::string mnt;
	GError *err = findd->err;

	if (udisks_filesystem_call_mount_sync(dev, opt, &mountPath, NULL, &err)) {
		mnt = mountPath;
		g_free(mountPath);
	} else {
		std::cerr << err->message << std::endl;
        g_clear_error(&err);
		return "";
	}
	findd->connected = this;
	return mnt;
}

DiskDev::~DiskDev() {
	g_object_unref(dev);
	if (findd->connected == this) findd->connected = nullptr;
	//if (client.unique()) g_object_unref(client.get());
}

#endif

/*int main() {
	FindDev findd;
	for (DiskDev *dd : findd.devs) {
		std::cout << dd->path << ": " << dd->name << " " << (int)dd->isIpod << std::endl;
		//if (dd->path == "/dev/sdc1") std::cout << dd->connect() << std::endl;
	}
	return 0;
}*/
