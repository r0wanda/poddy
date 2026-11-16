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
	const std::string name;
	/**
	 * @brief IndepPodMod constructor
	 * 
	 * @param _name name of the module
	 */
	IndepPodMod(std::string _name, std::shared_ptr<Poddy> pod);
	PodUtil::ErrorQueuePtr errq;
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
class PodMod : public virtual IndepPodMod {
public:
	ftxui::Component content;
	PodMod();
	virtual ~PodMod() = 0;
protected:
};
typedef std::shared_ptr<PodMod> PodModPtr;
typedef std::function<PodModPtr(std::shared_ptr<Poddy>)>& PodModWrapper;
typedef std::function<IndepPodModPtr(std::shared_ptr<Poddy>)>& IndepPodModWrapper;
#define WRAPPODMOD(base) [](std::shared_ptr<Poddy> pod) -> PodModPtr { return std::make_shared<base>(pod)->getPtr(); }
