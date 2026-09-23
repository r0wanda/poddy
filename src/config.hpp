#pragma once

#include <filesystem>
bool startConfig();
static bool confReady = false;
static std::filesystem::path cacheDir;
static std::filesystem::path configDir;
