#pragma once
#include <string>
#include <memory>
#include <functional>
#include <filesystem>
#include <ftxui/component/component.hpp>
#include <ftxui/component/app.hpp>
#include <ftxui/dom/elements.hpp>
#include <util.hpp>


class Tui;
class Poddy;
class Gpod;

/**
 * @brief podmod without window
 * 
 */
class IndepPodMod : public PodUtil::GetPtr<IndepPodMod> {
public:
	std::string name;
	/**
	 * @brief IndepPodMod constructor
	 * 
	 * @param _name name of the module
	 */
	IndepPodMod(std::string _name, std::shared_ptr<Poddy> pod);
	PodUtil::ErrorQueue errq;
	virtual ~IndepPodMod() = default;
protected:
	std::filesystem::path configPath;
	std::filesystem::path cachePath;
	std::shared_ptr<Poddy> poddy;
	std::shared_ptr<Gpod> gpod;
	std::shared_ptr<Tui> tui;
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
	PodMod(std::string _name);
	virtual ~PodMod() = default;
protected:
};
typedef std::shared_ptr<PodMod> PodModPtr;
typedef std::function<PodModPtr(std::shared_ptr<Poddy>)>& PodModWrapper;
#define WRAPPODMOD(base) [](std::shared_ptr<Poddy> pod) -> PodModPtr { return std::make_shared<base>(pod)->getPtr(); }
