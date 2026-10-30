#pragma once
#include <memory>
#include <type_traits>

namespace PodUtil {
	template <typename Base>
	class GetPtr : public std::enable_shared_from_this<GetPtr> {
	public:
		template <typename T = Base>
		inline std::shared_ptr<T> getPtr() {
			return std::dynamic_pointer_cast<T>(shared_from_this());
		}
	};
};
