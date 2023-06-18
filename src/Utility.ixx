module;
#include <cmath>
#include <limits>
#include <initializer_list>
#include <utility>
#include <optional>
#include <string>
#include <string_view>
export module Utility;

constexpr double sqrtNewtonRaphson(double x, double curr, double prev) noexcept
{
	return curr == prev
		? curr
		: sqrtNewtonRaphson(x, 0.5 * (curr + x / curr), curr);
}

constexpr float sqrtNewtonRaphson(float x, float curr, float prev) noexcept
{
	return curr == prev
		? curr
		: sqrtNewtonRaphson(x, 0.5f * (curr + x / curr), curr);
}

export extern "C++" namespace util
{
	using ::std::type_identity;
	using ::std::type_identity_t;
	using ::std::identity;
	using ::std::declval;

	using ::std::invoke_result;
	using ::std::invoke_result_t;
	using ::std::invoke;

	using ::std::min;
	using ::std::max;

	using ::std::compare_three_way;
	using ::std::compare_three_way_result;
	using ::std::compare_three_way_result_t;
	using ::std::weak_ordering;
	using ::std::weak_order;
	using ::std::partial_ordering;
	using ::std::partial_order;
	using ::std::strong_ordering;
	using ::std::strong_order;

	using ::std::underlying_type;
	using ::std::underlying_type_t;
	using ::std::to_underlying;

	using ::std::piecewise_construct_t;
	using ::std::piecewise_construct;
	using ::std::optional;
	using ::std::bad_optional_access;
	using ::std::nullopt_t;
	using ::std::nullopt;
	using ::std::make_optional;

	using ::std::initializer_list;

	using ::std::move;
	using ::std::forward;
	using ::std::move_if_noexcept;
	//using ::std::addressof;

	template <class T>
	[[nodiscard]] __forceinline constexpr T* addressof(T& _Val) noexcept
	{
		return __builtin_addressof(_Val);
	}

	template <class T>
	const T* addressof(const T&&) = delete;

	using ::std::is_constant_evaluated;
	using ::std::unreachable;

	using ::std::begin;
	using ::std::end;
	using ::std::cbegin;
	using ::std::cend;
	using ::std::size;
	using ::std::ssize;

	template<typename T>
	[[nodiscard]]
	__forceinline constexpr auto sqr(T&& value) noexcept(noexcept(value * value))
	{
		return value * value;
	}

	/// <summary>
	/// https://gist.github.com/alexshtf/eb5128b3e3e143187794
	/// </summary>
	/// <param name="value"></param>
	/// <returns> For a finite and non-negative value, returns an approximation for the square root of value. Otherwise, returns NaN.</returns>
	[[nodiscard]]
	constexpr double sqrt(const double value) noexcept
	{
		if (is_constant_evaluated())
		{
			return 0 <= value && value < std::numeric_limits<double>::infinity()
				? ::sqrtNewtonRaphson(value, value, 0)
				: std::numeric_limits<double>::quiet_NaN();
		}
		else
		{
			return std::sqrt(value);
		}
	}

	/// <summary>
	/// https://gist.github.com/alexshtf/eb5128b3e3e143187794
	/// </summary>
	/// <param name="value"></param>
	/// <returns> For a finite and non-negative value, an approximation for the square root of value. Otherwise, NaN.</returns>
	[[nodiscard]]
	constexpr float sqrt(const float value) noexcept
	{
		if (is_constant_evaluated())
		{
			return 0 <= value && value < std::numeric_limits<float>::infinity()
				? ::sqrtNewtonRaphson(value, value, 0)
				: std::numeric_limits<float>::quiet_NaN();
		}
		else
		{
			return std::sqrt(value);
		}
	}
}
