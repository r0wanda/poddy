#pragma once
#include <memory>
#include <string>
#include <vector>
#include <glib.h>
#include <type_traits>

namespace PodUtil {
	template <typename Base>
	class GetPtr : public std::enable_shared_from_this<Base> {
	public:
		template <typename T = Base>
		std::shared_ptr<T> getPtr() {
			static_assert(std::is_base_of_v<Base, T>);
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
};
