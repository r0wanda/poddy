#pragma once
#include <filesystem>
#include <settings.hpp>

class Files : public PodSettingsMod {
public:
    std::filesystem::path path;
    Files(std::shared_ptr<Poddy> pod);
    void refresh();
};
