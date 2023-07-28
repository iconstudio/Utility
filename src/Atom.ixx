export module Utility.Atom;
import <utility>;
import <memory>;
import Utility.Constraints;

export namespace util
{
	template<typename T>
	struct Atom;

	template<typename T>
	struct Atom
	{
		constexpr Atom()
			noexcept(nothrow_default_constructibles<T>)
			requires(default_initializable<T>) = default;
		constexpr ~Atom()
			noexcept(nothrow_destructibles<T>) = default;

		constexpr Atom(const Atom&)
			noexcept(nothrow_copy_constructibles<T>)
			requires(copyable<T>) = default;
		constexpr Atom(Atom&&)
			noexcept(nothrow_move_constructibles<T>)
			requires(movable<T>) = default;

		constexpr Atom& operator=(const Atom&)
			noexcept(nothrow_copy_assignables<T>)
			requires(copy_assignables<T>) = default;
		constexpr Atom& operator=(Atom&&)
			noexcept(nothrow_move_assignables<T>)
			requires(move_assignables<T>) = default;

		constexpr Atom(const T& value)
			noexcept(nothrow_copy_constructibles<T>) requires(copyable<T>)
			: value(value)
		{}

		constexpr Atom(T&& value)
			noexcept(nothrow_move_constructibles<T>) requires(movable<T>)
			: value(std::move(value))
		{}

		constexpr Atom& operator=(const T& value)
			noexcept(nothrow_copy_assignables<T>)
			requires(copy_assignables<T>)
		{
			this->value = value;
			return *this;
		}

		constexpr Atom& operator=(T&& value)
			noexcept(nothrow_move_assignables<T>)
			requires(move_assignables<T>)
		{
			this->value = std::move(value);
			return *this;
		}

		constexpr operator T& () & noexcept
		{
			return value;
		}

		constexpr operator const T& () const& noexcept
		{
			return value;
		}

		constexpr operator T && () && noexcept
		{
			return std::move(value);
		}

		constexpr operator const T && () const&& noexcept
		{
			return std::move(value);
		}

		constexpr T* operator->() noexcept
		{
			if constexpr (std::is_pointer_v<T>)
				return value;
			else
				return std::addressof(value);
		}

		constexpr const T* operator->() const noexcept
		{
			if constexpr (std::is_pointer_v<T>)
				return value;
			else
				return std::addressof(value);
		}

		[[nodiscard]]
		constexpr T& operator*() & noexcept
		{
			if constexpr (std::is_pointer_v<T>)
			{
				return *value;
			}
			else
			{
				return value;
			}
		}

		[[nodiscard]]
		constexpr const T& operator*() const& noexcept
		{
			if constexpr (std::is_pointer_v<T>)
			{
				return *value;
			}
			else
			{
				return value;
			}
		}

		[[nodiscard]]
		constexpr T&& operator*() && noexcept
		{
			if constexpr (std::is_pointer_v<T>)
			{
				return std::move(*value);
			}
			else
			{
				return std::move(value);
			}
		}

		[[nodiscard]]
		constexpr const T&& operator*() const&& noexcept
		{
			if constexpr (std::is_pointer_v<T>)
			{
				return std::move(*value);
			}
			else
			{
				return std::move(value);
			}
		}

		[[nodiscard]]
		friend constexpr bool operator==(const Atom& lhs, const Atom& rhs)
			noexcept(noexcept(lhs.value == rhs.value))
		{
			return lhs.value == rhs.value;
		}

		template<typename S>
		[[nodiscard]]
		friend constexpr bool operator==(const Atom& lhs, const Atom<S>& rhs)
			noexcept(noexcept(lhs.value == rhs.value))
		{
			static_assert(util::equality_comparable_with<T, S>);

			return lhs.value == rhs.value;
		}

		[[nodiscard]]
		friend constexpr auto operator<=>(const Atom& lhs, const Atom& rhs)
			noexcept(noexcept(lhs.value <=> rhs.value))
			requires(std::three_way_comparable<T>)
		{
			return lhs.value <=> rhs.value;
		}

		template<typename S>
		[[nodiscard]]
		friend constexpr auto operator<=>(const Atom& lhs, const Atom<S>& rhs)
			noexcept(noexcept(lhs.value <=> rhs.value))
			requires(std::three_way_comparable_with<T, S>)
		{
			return lhs.value <=> rhs.value;
		}

		friend constexpr Atom& operator++(Atom& lhs)
			noexcept(nothrow_incrementable<T>)
			requires(incrementable<T>)
		{
			++lhs.value;
			return lhs;
		}

		friend constexpr Atom& operator--(Atom& lhs)
			noexcept(nothrow_decrementable<T>)
			requires(decrementable<T>)
		{
			--lhs.value;
			return lhs;
		}

		friend constexpr Atom operator++(Atom& lhs, int)
			noexcept(nothrow_incrementable<T>)
			requires(incrementable<T>)
		{
			return Atom{ lhs.value++ };
		}

		friend constexpr Atom operator--(Atom& lhs, int)
			noexcept(nothrow_decrementable<T>)
			requires(decrementable<T>)
		{
			return Atom{ lhs.value-- };
		}

		friend constexpr Atom operator+(const Atom& lhs, const Atom& rhs)
			noexcept(nothrow_addable<T>)
			requires(addable<T>)
		{
			return Atom{ lhs.value + rhs.value };
		}

		template<addable_with<T> S>
		friend constexpr Atom operator+(const Atom& lhs, const Atom<S>& rhs)
			noexcept(nothrow_addable_with<T, S>)
		{
			return Atom{ lhs.value + rhs.value };
		}

		friend constexpr Atom operator-(const Atom& lhs, const Atom& rhs)
			noexcept(nothrow_subtractable<T>)
			requires(subtractable<T>)
		{
			return Atom{ lhs.value - rhs.value };
		}

		template<subtractable_with<T> S>
		friend constexpr Atom operator-(const Atom& lhs, const Atom<S>& rhs)
			noexcept(nothrow_subtractable_with<T, S>)
		{
			return Atom{ lhs.value - rhs.value };
		}

		friend constexpr Atom operator*(const Atom& lhs, const Atom& rhs)
			noexcept(nothrow_multipliable<T>)
			requires(multipliable<T>)
		{
			return Atom{ lhs.value * rhs.value };
		}

		template<multipliable_with<T> S>
		friend constexpr Atom operator*(const Atom& lhs, const Atom<S>& rhs)
			noexcept(nothrow_multipliable_with<T, S>)
		{
			return Atom{ lhs.value * rhs.value };
		}

		friend constexpr Atom operator/(const Atom& lhs, const Atom& rhs)
			noexcept(nothrow_dividable<T>)
			requires(dividable<T>)
		{
			return Atom{ lhs.value / rhs.value };
		}

		template<dividable_with<T> S>
		friend constexpr Atom operator/(const Atom& lhs, const Atom<S>& rhs)
			noexcept(nothrow_dividable_with<T, S>)
		{
			return Atom{ lhs.value / rhs.value };
		}

		friend constexpr Atom& operator+=(Atom& lhs, const Atom& rhs)
			noexcept(nothrow_addable<T>)
			requires(addable<T>)
		{
			lhs.value += rhs.value;
			return lhs;
		}

		template<addable_with<T> S>
		friend constexpr Atom& operator+=(Atom& lhs, const Atom<S>& rhs)
			noexcept(nothrow_addable_with<T, S>)
		{
			lhs.value += rhs.value;
			return lhs;
		}

		friend constexpr Atom& operator-=(Atom& lhs, const Atom& rhs)
			noexcept(nothrow_subtractable<T>)
			requires(subtractable<T>)
		{
			lhs.value -= rhs.value;
			return lhs;
		}

		template<subtractable_with<T> S>
		friend constexpr Atom& operator-=(Atom& lhs, const Atom<S>& rhs)
			noexcept(nothrow_subtractable_with<T, S>)
		{
			lhs.value -= rhs.value;
			return lhs;
		}

		friend constexpr Atom& operator*=(Atom& lhs, const Atom& rhs)
			noexcept(nothrow_multipliable<T>)
			requires(multipliable<T>)
		{
			lhs.value *= rhs.value;
			return lhs;
		}

		template<multipliable_with<T> S>
		friend constexpr Atom& operator*=(Atom& lhs, const Atom<S>& rhs)
			noexcept(nothrow_multipliable_with<T, S>)
		{
			lhs.value *= rhs.value;
			return lhs;
		}

		friend constexpr Atom& operator/=(Atom& lhs, const Atom& rhs)
			noexcept(nothrow_dividable<T>)
			requires(dividable<T>)
		{
			lhs.value /= rhs.value;
			return lhs;
		}

		template<dividable_with<T> S>
		friend constexpr Atom& operator/=(Atom& lhs, const Atom<S>& rhs)
			noexcept(nothrow_dividable_with<T, S>)
		{
			lhs.value /= rhs.value;
			return lhs;
		}

	private:
		T value;
	};

	template<typename T>
	struct [[nodiscard]] Atom<const T>
	{

	};

	template<typename T>
	Atom(T) -> Atom<T>;
}
