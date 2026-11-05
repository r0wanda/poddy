#pragma once
#include <memory>
#include <string>
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
	};
	class ErrorQueue;
	struct ErrQ {
		ErrQ(const char *str);
		ErrQ(GError *err);
		const std::shared_ptr<ErrorQueue> errq;
		const std::string msg;
	};
	class ErrorQueue : public GetPtr<ErrorQueue> {
	public:
		const std::string name;
		ErrorQueue(std::string nm);
		ErrorQueue(std::string nm, ErrorQueue &padre);
		void error(ErrQ err);
		void setParent(std::shared_ptr<ErrorQueue> queue);
	protected:
		std::vector<ErrQ> queue;
		std::shared_ptr<ErrorQueue> parent;
	};
	typedef std::shared_ptr<ErrorQueue> ErrorQueuePtr;
};
