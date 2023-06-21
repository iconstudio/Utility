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
	__forceinline constexpr auto sqr(T&& value) noexcept(noexcept(value* value))
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

	[[nodiscard]]
	constexpr int abs(const int& value) noexcept
	{
		return 0 < value ? value : -value;
	}

	[[nodiscard]]
	constexpr const unsigned int& abs(const unsigned int& value) noexcept
	{
		return value;
	}

	[[nodiscard]]
	constexpr unsigned int&& abs(unsigned int&& value) noexcept
	{
		return static_cast<unsigned int&&>(value);
	}

	[[nodiscard]]
	constexpr long long abs(const long long& value) noexcept
	{
		return 0 < value ? value : -value;
	}

	[[nodiscard]]
	constexpr const unsigned long long& abs(const unsigned long long& value) noexcept
	{
		return value;
	}

	[[nodiscard]]
	constexpr unsigned long long&& abs(unsigned long long&& value) noexcept
	{
		return static_cast<unsigned long long&&>(value);
	}

	[[nodiscard]]
	constexpr double abs(const double& value) noexcept
	{
		return 0 < value ? value : -value;
	}

	[[nodiscard]]
	constexpr double abs(const long double& value) noexcept
	{
		return 0 < value ? value : -value;
	}

	[[nodiscard]]
	constexpr int sign(const int& value) noexcept
	{
		return 0 < value ? 1 : (value < 0 ? -1 : 0);
	}

	[[nodiscard]]
	constexpr int sign(const unsigned int& value) noexcept
	{
		return 1;
	}

	[[nodiscard]]
	constexpr int sign(const float& value) noexcept
	{
		return 0 < value ? 1 : (value < 0 ? -1 : 0);
	}

	[[nodiscard]]
	constexpr int sign(const long long& value) noexcept
	{
		return 0 < value ? 1 : (value < 0 ? -1 : 0);
	}

	[[nodiscard]]
	constexpr int sign(const unsigned long long& value) noexcept
	{
		return 1;
	}

	[[nodiscard]]
	constexpr int sign(const double& value) noexcept
	{
		return 0 < value ? 1 : (value < 0 ? -1 : 0);
	}

	[[nodiscard]]
	constexpr int sign(const long double& value) noexcept
	{
		return 0 < value ? 1 : (value < 0 ? -1 : 0);
	}

	[[nodiscard]]
	constexpr auto MulDiv(const auto& value, const auto& mul, const auto& div)
	{
		const auto& mulval = value * mul;

		auto result = mulval / div;
		if (div / 2 <= mulval % div) // Round up for 0.5
		{
			result++;
		}

		return result;
	}

	template<typename T>
	[[nodiscard]]
	constexpr auto SafeDenom(const T& value, const T& mul, const T& div)
	{
		const int& signs = util::sign(value) * util::sign(mul) * util::sign(div);
		if (0 == signs)
		{
			if (0 == div)
			{
				static_assert(always_false<decltype(div)>, "div is zero");

				throw NAN;
			}
			else
			{
				return T{};
			}
		}

		T tval;
		T tmul;
		T tdiv;
		if (signs == 1)
		{
			tval = util::abs(value);
			tmul = util::abs(mul);
			tdiv = util::abs(div);
		}
		else
		{
			tval = util::abs(value);
			tmul = util::abs(mul);
			// push all signs to div
			tdiv = util::abs(div) * signs;
		}

		if (tval < tmul)
		{
			const double mdiv = static_cast<double>(tmul) / tdiv;

			return static_cast<T>(mdiv * tval);
		}
		else
		{
			const double vdiv = static_cast<double>(tval) / tdiv;

			return static_cast<T>(vdiv * tmul);
		}
	}
}

#pragma warning(push, 1)
namespace util::test
{
	void test_safe_denom()
	{
		constexpr auto aaa = util::SafeDenom(754638, -6352, 1);
		constexpr auto bbb = util::SafeDenom(754638, -6352, 10);
		constexpr auto ccc = util::SafeDenom(754638, -6352, 100);
		constexpr auto ddd = util::SafeDenom(754638, -6352, 1000);
		constexpr auto eee = util::SafeDenom(754638, -6352, 10000);
		constexpr auto fff = util::SafeDenom(754638, -6352, 100000);
	}
}
#pragma warning(pop)