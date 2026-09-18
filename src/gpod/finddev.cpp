#include "finddev.hpp"
#include "../config.h"
#include <iostream>
#include <string>

#ifdef IS_LINUX
#include <udisks/udisks.h>
#include <glib.h>
#include <memory>
#include <cctype>
#include <algorithm>

DiskDev::DiskDev(UDisksBlock *b, UDisksFilesystem *fs): dev(fs) {
	path = udisks_block_get_device(b);
	name = udisks_block_get_id_label(b);
	if (name.empty()) {
		name = udisks_block_get_id(b);
	}
	if (name.empty()) {
		name = "unnamed disk";
		isIpod = 2;
	} else {
		std::string subs = "IPOD";
		// case-insensitive search for ipod in name (https://stackoverflow.com/a/19839371)
		auto it = std::search(name.begin(), name.end(), subs.begin(), subs.end(), [](unsigned char ch1, unsigned char ch2) {
			return std::toupper(ch1) == std::toupper(ch2);
		});
		if (it == name.end()) isIpod = 1;
		else isIpod = 0;
	}
	g_object_unref(b);
}

std::vector<DiskDev*> finddev() {
	GError *err = nullptr;
    UDisksClient *client = udisks_client_new_sync(nullptr, &err);
	//std::shared_ptr<UDisksClient> clPtr(client);
    
    if (err != nullptr) {
		// TODO: error system
        std::cerr << err->message << std::endl;
        g_clear_error(&err);
        exit(1);
    }

    GDBusObjectManager *manager = udisks_client_get_object_manager(client);
    GList *objects = g_dbus_object_manager_get_objects(manager);
	std::vector<DiskDev*> devs;

    for (GList *l = objects; l != nullptr; l = l->next) {
        UDisksObject *object = UDISKS_OBJECT(l->data);
        UDisksBlock *block = udisks_object_get_block(object);
		UDisksFilesystem *fs = udisks_object_get_filesystem(object);
		if (block == nullptr || fs == nullptr) {
			g_object_unref(block);
			g_object_unref(fs);
			continue;
		}

		DiskDev *ddev = new DiskDev(block, fs);
		devs.push_back(ddev);
    }
    g_list_free_full(objects, g_object_unref);
    g_object_unref(client);
	return devs;
}

void udisksCallback(GObject *src, GAsyncResult *res, gpointer cb) {
	gchar *mountPath;
	GError *err;
	udisks_filesystem_call_mount_finish(UDISKS_FILESYSTEM(src), &mountPath, res, &err);
	if (err != nullptr) {
		// TODO: error system
        std::cerr << err->message << std::endl;
        g_clear_error(&err);
        exit(1);
    }
	std::string mnt(mountPath);
	g_free(mountPath);
	(*static_cast<std::function<void(std::string)>*>(cb))(mnt);
}

void DiskDev::connect(std::function<void(std::string)> cb) {
	GVariantBuilder bd;
	g_variant_builder_init(&bd, G_VARIANT_TYPE_VARDICT);
	GVariant *opt = g_variant_builder_end(&bd);

	udisks_filesystem_call_mount(dev, opt, NULL, udisksCallback, &cb);
}

DiskDev::~DiskDev() {
	g_object_unref(dev);
	//if (client.unique()) g_object_unref(client.get());
}

#endif
