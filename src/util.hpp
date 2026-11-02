#pragma once
#include <memory>
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
};
