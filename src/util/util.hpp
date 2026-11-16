#pragma once
#include <regex>
#include <memory>
#include <string>
#include <vector>
#include <glib.h>
#include <filesystem>

namespace PodUtil {
	bool startConfig();
	static std::filesystem::path homeDir;
	static std::filesystem::path cacheDir;
	static std::filesystem::path configDir;
	static std::filesystem::path musicDir;

	template <typename Base>
	class GetPtr : public std::enable_shared_from_this<Base> {
	public:
		template <typename T = Base>
		std::shared_ptr<T> getPtr() {
			return std::dynamic_pointer_cast<T>(this->shared_from_this());
		}
		virtual ~GetPtr() = default;
	};
	class ErrorQueue;
	struct ErrQ {
		ErrQ(std::string str);
		ErrQ(const char *str);
		ErrQ(GError *&err);
		ErrQ(std::string str, bool isFatal);
		ErrQ(const char *str, bool isFatal);
		ErrQ(GError *&err, bool isFatal);
		ErrQ(const ErrQ &err);
		std::shared_ptr<ErrorQueue> errq;
		const std::string msg;
		bool fatal;
	};
	class ErrorQueue : public GetPtr<ErrorQueue> {
	public:
		const std::string name;
		ErrorQueue(std::string nm);
		ErrorQueue(std::string nm, std::shared_ptr<ErrorQueue> padre);
		virtual void error(ErrQ err);
		virtual void error(ErrQ err, bool fatal);
		void setParent(std::shared_ptr<ErrorQueue> queue);
		virtual ~ErrorQueue() = default;
	protected:
		std::vector<ErrQ> queue;
		std::shared_ptr<ErrorQueue> parent;
	};
	typedef std::shared_ptr<ErrorQueue> ErrorQueuePtr;

	namespace Regex {
		// https://stackoverflow.com/a/3809435 CC BY-SA 4.0
		const std::regex url{"[-a-zA-Z0-9@:%._\\+~#=]{1,256}\\.[a-zA-Z0-9()]{1,6}\\b([-a-zA-Z0-9()@:%_\\+.~#?&//=]*)"};
		const std::regex httpUrl{"https?:\\/\\/(www\\.)?[-a-zA-Z0-9@:%._\\+~#=]{1,256}\\.[a-zA-Z0-9()]{1,6}\\b([-a-zA-Z0-9()@:%_\\+.~#?&//=]*)"};
	};
};
