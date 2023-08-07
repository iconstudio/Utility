export module Utility.Expected;
import <optional>;
import Utility.Constraints;

export namespace util
{
	template<typename T, typename E>
	class [[nodiscard]] Expected
	{
	public:
		using value_type = T;
		using error_type = E;

		constexpr Expected()
			noexcept(nothrow_constructibles<T>)
			: myValue()
		{}

		constexpr Expected(const Expected& other)
			noexcept(nothrow_copy_constructibles<value_type, error_type>)
			: myValue(other.myValue)
			, myError(other.myError)
		{}

		constexpr Expected(Expected&& other)
			noexcept(nothrow_move_constructibles<value_type, error_type>)
			: myValue(static_cast<value_type&&>(other.myValue))
			, myError(static_cast<error_type&&>(other.myError))
		{}

		explicit constexpr Expected(const value_type& value)
			noexcept(nothrow_copy_constructibles<value_type>)
			: myValue(value)
		{}

		explicit constexpr Expected(value_type&& value)
			noexcept(nothrow_move_constructibles<value_type>)
			: myValue(static_cast<value_type&&>(value))
		{}

		explicit constexpr Expected(const error_type& error)
			noexcept(nothrow_copy_constructibles<error_type>)
			: myError(error)
		{}

		explicit constexpr Expected(error_type&& error)
			noexcept(nothrow_move_constructibles<error_type>)
			: myError(static_cast<error_type&&>(error))
		{}

		constexpr Expected& operator=(const Expected& other)
			noexcept(nothrow_copy_assignables<value_type, error_type>)
		{
			if (&other == static_cast<const Expected* const&>(this))
			{
				return *this;
			}

			myValue = other.myValue;
			myError = other.myError;
			return *this;
		}

		constexpr Expected& operator=(Expected&& other)
			noexcept(nothrow_move_assignables<value_type, error_type>)
		{
			myValue = static_cast<value_type&&>(other.myValue);
			myError = static_cast<error_type&&>(other.myError);
			return *this;
		}

		[[nodiscard]]
		constexpr value_type& value() & noexcept
		{
			return myValue;
		}

		[[nodiscard]]
		constexpr const value_type& value() const& noexcept
		{
			return myValue;
		}

		[[nodiscard]]
		constexpr const value_type&& value() const&& noexcept
		{
			return static_cast<const value_type&&>(myValue);
		}

		[[nodiscard]]
		constexpr value_type&& value() && noexcept
		{
			return static_cast<value_type&&>(myValue);
		}

		[[nodiscard]]
		constexpr const error_type& error() const& noexcept
		{
			return myError;
		}

		[[nodiscard]]
		constexpr error_type& error() & noexcept
		{
			return myError;
		}

		[[nodiscard]]
		constexpr const error_type&& error() const&& noexcept
		{
			return static_cast<const error_type&&>(myError);
		}

		[[nodiscard]]
		constexpr error_type&& error() && noexcept
		{
			return static_cast<error_type&&>(myError);
		}

		[[nodiscard]]
		constexpr bool hasValue() const noexcept
		{
			return myValue.has_value();
		}

		[[nodiscard]]
		constexpr bool hasError() const noexcept
		{
			return myError.has_value();
		}

		[[nodiscard]]
		explicit constexpr operator bool() const noexcept
		{
			return hasValue();
		}

		constexpr const value_type& operator*() const& noexcept
		{
			return myValue.value();
		}

		constexpr value_type& operator*() & noexcept
		{
			return myValue.value();
		}

		constexpr const value_type&& operator*() const&& noexcept
		{
			return static_cast<const value_type&&>(myValue.value());
		}

		constexpr value_type&& operator*() && noexcept
		{
			return static_cast<value_type&&>(myValue.value());
		}

		constexpr void swap(Expected& other)
			noexcept(nothrow_swappables<value_type, error_type>)
		{
			std::swap(myValue, other.myValue);
			std::swap(myError, other.myError);
		}

	private:
		std::optional<value_type> myValue;
		std::optional<error_type> myError;
	};
}
