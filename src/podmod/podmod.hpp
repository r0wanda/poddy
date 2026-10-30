#pragma once
#include <string>
#include <memory>
#include <filesystem>
#include <ftxui/component/component.hpp>
#include <ftxui/component/app.hpp>
#include <ftxui/dom/elements.hpp>

class Poddy;

/**
 * @brief podmod without window
 * 
 */
class IndepPodMod : public std::enable_shared_from_this<IndepPodMod> {
public:
	std::string name;
	/**
	 * @brief IndepPodMod constructor
	 * 
	 * @param _name name of the module
	 */
	IndepPodMod(std::string _name);
protected:
	std::filesystem::path configPath;
	std::filesystem::path cachePath;
};
typedef std::shared_ptr<IndepPodMod> IndepPodModPtr;

/**
 * @brief module with window
 * 
 */
class PodMod : public IndepPodMod {
public:
	ftxui::Component content;
	/**
	 * @brief PodMod constructor
	 * 
	 * @param _name name of the module
	 * @param pod underlying poddy class
	 */
	PodMod(std::string _name, Poddy *pod);
protected:
	Poddy *poddy;
	GpodPtr gpod;
	TuiPtr tui;
};
typedef std::shared_ptr<PodMod> PodModPtr;
