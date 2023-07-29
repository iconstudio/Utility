export module Utility.Atom;
import <utility>;
import <memory>;
import Utility;
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

		constexpr Atom& operator++()
			noexcept(nothrow_incrementable<T>)
			requires(incrementable<T>)
		{
			++value;
			return *this;
		}

		constexpr Atom& operator--()
			noexcept(nothrow_decrementable<T>)
			requires(decrementable<T>)
		{
			--value;
			return *this;
		}

		constexpr Atom operator++(int)
			noexcept(nothrow_incrementable<T>)
			requires(incrementable<T>)
		{
			return Atom{ value++ };
		}

		constexpr Atom operator--(int)
			noexcept(nothrow_decrementable<T>)
			requires(decrementable<T>)
		{
			return Atom{ value-- };
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
			static_assert(not std::is_pointer_v<S>);

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
			static_assert(not std::is_pointer_v<S>);

			lhs.value /= rhs.value;
			return lhs;
		}

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

	private:
		T value;
	};

	template<typename T>
	struct [[nodiscard]] Atom<const T>
	{
		constexpr Atom()
			noexcept(nothrow_default_constructibles<T>)
			requires(default_initializable<T>) = default;
		constexpr ~Atom()
			noexcept(nothrow_destructibles<T>) = default;

		constexpr Atom(const T& value)
			noexcept(nothrow_copy_constructibles<T>) requires(copyable<T>)
			: value(value)
		{}

		constexpr Atom(T&& value)
			noexcept(nothrow_move_constructibles<T>) requires(movable<T>)
			: value(std::move(value))
		{}

		constexpr operator const T& () const& noexcept
		{
			return value;
		}

		constexpr operator const T && () const&& noexcept
		{
			return std::move(value);
		}

		constexpr const T* operator->() const noexcept
		{
			if constexpr (std::is_pointer_v<T>)
				return value;
			else
				return std::addressof(value);
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

		constexpr Atom(const Atom&)
			noexcept(nothrow_copy_constructibles<T>)
			requires(copyable<T>) = default;
		constexpr Atom(Atom&&)
			noexcept(nothrow_move_constructibles<T>)
			requires(movable<T>) = default;
		Atom& operator=(const Atom&) = delete;
		Atom& operator=(Atom&&) = delete;

	private:
		T value;
	};

	template<typename T>
	struct [[nodiscard]] Atom<T&>
	{
		constexpr Atom(T& value) noexcept
			: value(std::addressof(value))
		{}

		constexpr Atom(const T&) noexcept
		{
			static_assert(util::always_false<T>, "Cannot create an Atom of lvalue from const lvalue");
		}

		constexpr Atom(const T&&) noexcept
		{
			static_assert(util::always_false<T>, "Cannot create an Atom of reference from the temporary value");
		}

		constexpr ~Atom() noexcept = default;

		constexpr Atom& operator=(const T& value)
			noexcept(nothrow_copy_assignables<T>)
			requires(copy_assignables<T>)
		{
			*(this->value) = value;
			return *this;
		}

		constexpr Atom& operator=(T&& value)
			noexcept(nothrow_move_assignables<T>)
			requires(move_assignables<T>)
		{
			*(this->value) = std::move(value);
			return *this;
		}

		constexpr operator T& () & noexcept
		{
			return *value;
		}

		constexpr operator const T& () const& noexcept
		{
			return *value;
		}

		constexpr operator T && () && noexcept
		{
			return std::move(*value);
		}

		constexpr operator const T && () const&& noexcept
		{
			return std::move(*value);
		}

		constexpr T* operator->() noexcept
		{
			return value;
		}

		constexpr const T* operator->() const noexcept
		{
			return value;
		}

		[[nodiscard]]
		constexpr T& operator*() & noexcept
		{
			return *value;
		}

		[[nodiscard]]
		constexpr const T& operator*() const& noexcept
		{
			return *value;
		}

		[[nodiscard]]
		constexpr T&& operator*() && noexcept
		{
			return *value;
		}

		[[nodiscard]]
		constexpr const T&& operator*() const&& noexcept
		{
			return *value;
		}

		constexpr Atom(const Atom&) noexcept = default;
		constexpr Atom(Atom&&) noexcept = default;
		constexpr Atom& operator=(const Atom&) noexcept = default;
		constexpr Atom& operator=(Atom&&) noexcept = default;

	private:
		T* value = nullptr;
	};

	template<typename T>
	struct [[nodiscard]] Atom<const T&>
	{
		constexpr Atom(T& value) noexcept
			: value(std::addressof(value))
		{}

		constexpr Atom(const T& value) noexcept
			: value(std::addressof(value))
		{}

		constexpr Atom(const T&&) noexcept
		{
			static_assert(util::always_false<T>, "Cannot create an Atom of reference from the temporary value");
		}

		constexpr ~Atom() noexcept = default;

		constexpr operator const T& () const& noexcept
		{
			return *value;
		}

		constexpr operator const T && () const&& noexcept
		{
			return std::move(*value);
		}

		constexpr const T* operator->() const noexcept
		{
			return value;
		}

		[[nodiscard]]
		constexpr const T& operator*() const& noexcept
		{
			return *value;
		}

		[[nodiscard]]
		constexpr const T&& operator*() const&& noexcept
		{
			return *value;
		}

		constexpr Atom(const Atom&) noexcept = default;
		constexpr Atom(Atom&&) noexcept = default;
		Atom& operator=(const Atom&) = delete;
		Atom& operator=(Atom&&) = delete;

	private:
		const T* value = nullptr;
	};

	template<typename T>
	struct [[nodiscard]] Atom<std::shared_ptr<T>>
	{
		constexpr Atom() noexcept = default;

		constexpr Atom(nullptr_t) noexcept
			: value(nullptr)
		{}

		Atom(const std::shared_ptr<T>& handle) noexcept
			: value(handle)
		{}

		Atom(std::shared_ptr<T>&& handle) noexcept
			: value(std::move(handle))
		{}

		explicit Atom(const std::weak_ptr<T>& handle) noexcept
			: value(handle)
		{}

		explicit Atom(std::weak_ptr<T>&& handle) noexcept
			: value(std::move(handle))
		{}

		explicit Atom(std::unique_ptr<T>&& handle)
			: value(std::move(handle))
		{}

		~Atom() noexcept(nothrow_destructibles<T>&& trivially_destructibles<T>) = default;

		Atom& operator=(std::shared_ptr<T>& ptr) noexcept
		{
			value = ptr;
			return *this;
		}

		Atom& operator=(const std::shared_ptr<T>& ptr) noexcept
		{
			value = ptr;
			return *this;
		}

		Atom& operator=(std::shared_ptr<T>&& ptr) noexcept
		{
			value = std::move(ptr);
			return *this;
		}

		Atom& operator=(const std::shared_ptr<T>&& ptr) noexcept
		{
			value = std::move(ptr);
			return *this;
		}

		template<typename S>
			requires convertible_to<S*, T*>
		Atom& operator=(std::unique_ptr<S>&& ptr)
		{
			value = std::move(ptr);
			return *this;
		}

		constexpr Atom& operator=(const T& value)
			noexcept(nothrow_copy_assignables<T>)
			requires(copy_assignables<T>)
		{
			*(this->value) = value;
			return *this;
		}

		constexpr Atom& operator=(T&& value)
			noexcept(nothrow_move_assignables<T>)
			requires(move_assignables<T>)
		{
			*(this->value) = std::move(value);
			return *this;
		}

		constexpr operator std::shared_ptr<T>& () & noexcept
		{
			return value;
		}

		constexpr operator const std::shared_ptr<T>& () const& noexcept
		{
			return value;
		}

		constexpr operator std::shared_ptr<T> && () && noexcept
		{
			return std::move(value);
		}

		constexpr operator const std::shared_ptr<T> && () const&& noexcept
		{
			return std::move(value);
		}

		explicit constexpr operator T& () noexcept
		{
			return *value;
		}

		explicit constexpr operator const T& () const noexcept
		{
			return *value;
		}

		constexpr T*& operator->() noexcept
		{
			return *value;
		}

		constexpr T* const& operator->() const noexcept
		{
			return *value;
		}

		[[nodiscard]]
		constexpr T& operator*() noexcept
		{
			return *value;
		}

		[[nodiscard]]
		constexpr const T& operator*() const noexcept
		{
			return *value;
		}

		constexpr Atom(const Atom&) noexcept = default;
		constexpr Atom(Atom&&) noexcept = default;
		constexpr Atom& operator=(const Atom&) noexcept = default;
		constexpr Atom& operator=(Atom&&) noexcept = default;

	private:
		std::shared_ptr<T> value;
	};

	template<typename T>
	struct [[nodiscard]] Atom<std::shared_ptr<const T>>
	{
		template<typename S>
			requires(assignable_from<std::shared_ptr<const T>, const std::shared_ptr<S>&>)
		Atom(const std::shared_ptr<S>& handle) noexcept
			: value(handle)
		{}

		template<typename S>
			requires(assignable_from<std::shared_ptr<const T>, std::shared_ptr<S>>)
		Atom(const std::shared_ptr<S>& handle) noexcept
			: value(std::move(handle))
		{}

		explicit Atom(const std::weak_ptr<T>& handle) noexcept
			: value(handle)
		{}

		explicit Atom(std::weak_ptr<T>&& handle) noexcept
			: value(std::move(handle))
		{}

		template<typename S>
			requires convertible_to<S*, T*>
		Atom& operator=(std::unique_ptr<S>&& ptr)
		{
			value = std::move(ptr);
			return *this;
		}

		~Atom() noexcept(nothrow_destructibles<T>) = default;

		template<typename S>
			requires(assignable_from<std::shared_ptr<const T>, const std::shared_ptr<S>&>)
		Atom& operator=(std::shared_ptr<S>& ptr) noexcept
		{
			value = ptr;
			return *this;
		}

		template<typename S>
			requires(assignable_from<std::shared_ptr<const T>, const std::shared_ptr<S>&>)
		Atom& operator=(const std::shared_ptr<S>& ptr) noexcept
		{
			value = ptr;
			return *this;
		}

		Atom& operator=(std::shared_ptr<T>&& ptr) noexcept
		{
			value = std::move(ptr);
			return *this;
		}

		Atom& operator=(const std::shared_ptr<T>&& ptr) noexcept
		{
			value = std::move(ptr);
			return *this;
		}

		Atom& operator=(std::unique_ptr<T>&& ptr)
		{
			value = std::move(ptr);
			return *this;
		}

		constexpr Atom& operator=(const T& value)
			noexcept(nothrow_copy_assignables<T>)
			requires(copy_assignables<T>)
		{
			*(this->value) = value;
			return *this;
		}

		constexpr Atom& operator=(T&& value)
			noexcept(nothrow_move_assignables<T>)
			requires(move_assignables<T>)
		{
			*(this->value) = std::move(value);
			return *this;
		}

		constexpr operator std::shared_ptr<T>& () & noexcept
		{
			return value;
		}

		constexpr operator const std::shared_ptr<T>& () const& noexcept
		{
			return value;
		}

		constexpr operator std::shared_ptr<T> && () && noexcept
		{
			return std::move(value);
		}

		constexpr operator const std::shared_ptr<T> && () const&& noexcept
		{
			return std::move(value);
		}

		explicit constexpr operator T& () noexcept
		{
			return *value;
		}

		explicit constexpr operator const T& () const noexcept
		{
			return *value;
		}

		constexpr T*& operator->() noexcept
		{
			return *value;
		}

		constexpr T* const& operator->() const noexcept
		{
			return *value;
		}

		[[nodiscard]]
		constexpr T& operator*() noexcept
		{
			return *value;
		}

		[[nodiscard]]
		constexpr const T& operator*() const noexcept
		{
			return *value;
		}

		constexpr Atom(const Atom&) noexcept = default;
		constexpr Atom(Atom&&) noexcept = default;
		constexpr Atom& operator=(const Atom&) noexcept = default;
		constexpr Atom& operator=(Atom&&) noexcept = default;

	private:
		std::shared_ptr<const T> value;
	};

	template<typename T, typename Deleter>
	struct [[nodiscard]] Atom<std::unique_ptr<T, Deleter>>
		: public std::unique_ptr<T, Deleter>
	{
		using base = std::unique_ptr<T, Deleter>;

		using base::base;

		Atom(const Atom&) = delete;
		constexpr Atom(Atom&&) noexcept = default;
		Atom& operator=(const Atom&) = delete;
		constexpr Atom& operator=(Atom&&) noexcept = default;
	};

	template<typename T>
	Atom(T) -> Atom<T>;

	template<typename T, typename S>
	[[nodiscard]]
	constexpr bool operator==(const Atom<T>& lhs, const Atom<S>& rhs)
		noexcept(noexcept((*lhs) == (*rhs)))
	{
		static_assert(equality_comparable_with<T, S>, "T and S must be equality comparable");

		return (*lhs) == (*rhs);
	}

	template<typename T, typename S>
	[[nodiscard]]
	constexpr auto operator<=>(const Atom<T>& lhs, const Atom<S>& rhs)
		noexcept(noexcept((*lhs) <=> (*rhs)))
	{
		static_assert(std::three_way_comparable_with<T, S>, "T and S must be three way comparable");

		return (*lhs) <=> (*rhs);
	}

	template<typename T, typename S>
	[[nodiscard]]
	constexpr auto operator+(const Atom<T>& lhs, const Atom<S>& rhs)
		noexcept(nothrow_addable_with<T, S>)
	{
		static_assert(addable_with<T, S>);

		return Atom{ (*lhs) + (*rhs) };
	}

	template<typename T, typename S>
	[[nodiscard]]
	constexpr auto operator-(const Atom<T>& lhs, const Atom<S>& rhs)
		noexcept(nothrow_subtractable_with<T, S>)
	{
		static_assert(subtractable_with<T, S>);

		return Atom{ (*lhs) - (*rhs) };
	}

	template<typename T, typename S>
	[[nodiscard]]
	constexpr auto operator*(const Atom<T>& lhs, const Atom<S>& rhs)
		noexcept(nothrow_multipliable_with<T, S>)
	{
		static_assert(multipliable_with<T, S>);
		static_assert(not std::is_pointer_v<S>);

		return Atom{ (*lhs) * (*rhs) };
	}

	template<typename T, typename S>
	[[nodiscard]]
	constexpr auto operator/(const Atom<T>& lhs, const Atom<S>& rhs)
		noexcept(nothrow_dividable_with<T, S>)
	{
		static_assert(dividable_with<T, S>);
		static_assert(not std::is_pointer_v<S>);

		return Atom{ (*lhs) / (*rhs) };
	}
}

#pragma warning(push, 1)
namespace util::test
{
	void test_atom() noexcept
	{
		Atom atom00{ 400 };
		const Atom atom01{ 400 };
		constexpr Atom atom02{ 400 };

		Atom<int> atom03{ 400 };
		const Atom<int> atom04{ 400 };
		constexpr Atom<int> atom05{ 400 };

		Atom<const int> atom06{ 400 };
		const Atom<const int> atom07{ 400 };
		constexpr Atom<const int> atom08{ 400 };

		Atom<int*> atom09{ nullptr };
		const Atom<int*> atom10{ nullptr };
		constexpr Atom<int*> atom11{ nullptr };

		Atom<const int*> atom12{ nullptr };
		const Atom<const int*> atom13{ nullptr };
		constexpr Atom<const int*> atom14{ nullptr };
	}
}
#pragma warning(pop)
