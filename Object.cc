#include "Reflection.cc"

import <string>;
import <string_view>;
import <vector>;
import Utility;
import Utility.Constraints;
import Utility.Classifier;
import Utility.Meta;
import Utility.Indexer;
import Utility.Named;

namespace util
{
	/// <summary>
	/// Reflected base class for all objects.
	/// </summary>
	class Object :
		public Named,
		public Classifier<Object>,
		public Indexer<Object>
	{
	public:
		constexpr Object() noexcept = default;
		constexpr ~Object() noexcept = default;

		template<typename Self>
		[[nodiscard]]
		constexpr Self&& GetClass(this Self&& self) noexcept
		{
			//static_assert(always_false<Self>());
			return forward<Self>(self);
		}

		template<typename Self>
		[[nodiscard]]
		constexpr std::string_view GetName(this Self&& self) noexcept
		{
			GetClass().GetName();
		}

		constexpr Object(const Object& other) noexcept = default;
		constexpr Object& operator=(const Object& other) noexcept = default;
		constexpr Object(Object&& other) noexcept = default;
		constexpr Object& operator=(Object&& other) noexcept = default;
	};
}

template<>
struct util::class_id<util::Object>
{
	static constexpr size_t value = static_cast<size_t>(-1);
};
