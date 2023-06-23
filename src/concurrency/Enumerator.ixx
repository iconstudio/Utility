module;
export module Utility.Coroutine.Enumerator;
import <memory>;
import <stack>;
import <ranges>;
import Utility;
import Utility.Monad;
import Utility.Coroutine;

namespace util::coroutine
{
	export template<typename Rng>
		concept enumerable = std::ranges::forward_range<Rng> && notvoids<std::ranges::range_value_t<Rng>>;

	namespace detail
	{
		template<typename Rng>
		concept available_data = enumerable<Rng>
			&& std::contiguous_iterator<std::ranges::iterator_t<Rng>>
			&& requires(Rng & range)
		{
			range.data();
		};

		template<typename Rng>
		struct noexcept_data
		{
			static constexpr bool value = false;
		};

		template<available_data Rng>
		struct noexcept_data<Rng>
		{
			static constexpr bool value = noexcept(declval<Rng>().data());
		};
	}

	export template<enumerable Rng>
		requires movable<Rng>
	class [[nodiscard]] Enumerator : public std::ranges::view_interface<Enumerator<Rng>>
	{
	public:
		using value_type = std::ranges::range_value_t<Rng>;
		using reference = std::ranges::range_reference_t<Rng>;
		using const_reference = std::ranges::range_reference_t<const Rng>;
		using rvalue_reference = std::ranges::range_rvalue_reference_t<Rng>;
		using const_rvalue_reference = std::ranges::range_rvalue_reference_t<const Rng>;
		using size_type = size_t;
		using difference_type = ptrdiff_t;

		using internal_iterator = std::ranges::iterator_t<Rng>;
		using type = Enumerator<Rng>;
		using interface = std::ranges::view_interface<Enumerator<Rng>>;
		using promise_type = DeferredValuePromise<type, internal_iterator>;
		using handle_type = promise_type::handle_type;

		class iterator;
		using const_iterator = iterator;

		constexpr Enumerator()
			noexcept(nothrow_default_constructibles<Rng>)
			requires(default_initializable<Rng>) = default;
		constexpr ~Enumerator()
			noexcept(nothrow_destructibles<Rng>) = default;

		template<enumerable Sng>
			requires constructible_from<Rng, Sng>
		constexpr Enumerator(Enumerator<Sng>&& other)
			noexcept(nothrow_constructibles<Rng, Sng>)
			: myHandle(move(other.myHandle))
			, underlyingRange(move(other.underlyingRange))
			, underlyingIt(move(other.underlyingIt))
		{}

		template<enumerable Sng>
			requires constructible_from<Rng, Sng, internal_iterator>&& copy_constructibles<Rng, Sng, internal_iterator>
		constexpr Enumerator(const Enumerator<Sng>& other)
			noexcept(nothrow_constructibles<Rng, Sng, internal_iterator>)
			: myHandle(other.myHandle)
			, underlyingRange(other.underlyingRange)
			, underlyingIt(other.underlyingIt)
		{}

		explicit constexpr Enumerator(const handle_type& handle)
			noexcept(nothrow_copy_constructibles<handle_type>)
			requires copy_constructibles<Rng, handle_type>
		: myHandle(handle)
			, underlyingIt(handle.promise().value())
		{}

		explicit constexpr Enumerator(handle_type&& handle)
			noexcept(nothrow_move_constructibles<handle_type>)
			: myHandle(move(handle))
			, underlyingIt(move(handle.promise()).value())
		{}

		[[nodiscard]]
		inline iterator begin() noexcept
		{
			return iterator{ myHandle };
		}

		[[nodiscard]]
		constexpr default_sentinel_t end() noexcept
		{
			return {};
		}

		[[nodiscard]]
		inline iterator begin() const noexcept
		{
			return iterator{ myHandle };
		}

		[[nodiscard]]
		constexpr default_sentinel_t end() const noexcept
		{
			return {};
		}

		[[nodiscard]]
		constexpr auto data()
			noexcept(detail::noexcept_data<Rng>::value)
			requires detail::available_data<Rng>
		{
			return (*underlyingRange).data();
		}

		[[nodiscard]]
		constexpr auto data() const
			noexcept(detail::noexcept_data<Rng>::value)
			requires detail::available_data<const Rng>
		{
			return (*underlyingRange).data();
		}

		Enumerator(const Enumerator& other) = delete;
		constexpr Enumerator(Enumerator&& other)
			noexcept(nothrow_move_constructibles<Rng>) = default;
		Enumerator& operator=(const Enumerator& other) = delete;
		constexpr Enumerator& operator=(Enumerator&& other)
			noexcept(nothrow_move_assignables<Rng>) = default;

	private:
		handle_type myHandle;
		Monad<Rng> underlyingRange;
		Monad<internal_iterator> underlyingIt;
	};

	template<enumerable Rng>
		requires movable<Rng>
	class Enumerator<Rng>::iterator
	{
	public:
		using super_type = Enumerator<Rng>;

		iterator() noexcept = default;
		~iterator() noexcept = default;

		constexpr iterator(handle_type coroutine) noexcept
			: coHandle(coroutine)
		{}

		inline iterator& operator++()
		{
			if (!coHandle.done())
			{
				coHandle.resume();
			}

			return *this;
		}

		inline void operator++(int)
		{
			if (!coHandle.done())
			{
				coHandle.resume();
			}
		}

		inline const iterator& operator++() const
		{
			if (!coHandle.done())
			{
				coHandle.resume();
			}

			return *this;
		}

		inline void operator++(int) const
		{
			if (!coHandle.done())
			{
				coHandle.resume();
			}
		}

		inline reference operator*() & noexcept
		{
			return *(coHandle.promise().value());
		}

		inline const_reference operator*() const& noexcept
		{
			return *(coHandle.promise().value());
		}

		inline rvalue_reference operator*() &&
			noexcept(nothrow_move_constructibles<value_type>)
		{
			return *(move(coHandle.promise()).value());
		}

		inline const_rvalue_reference operator*() const&&
			noexcept(nothrow_move_constructibles<value_type>)
		{
			return *(move(coHandle.promise()).value());
		}

		[[nodiscard]]
		inline bool operator==(default_sentinel_t) const
		{
			return !coHandle || coHandle.done();
		}

	private:
		handle_type coHandle;
	};

	template<std::ranges::forward_range Rng>
	Enumerator(Rng) -> Enumerator<Rng>;
}

export template<typename Rng>
inline constexpr bool std::ranges::enable_borrowed_range<util::coroutine::Enumerator<Rng>> = std::ranges::enable_borrowed_range<Rng>;

export namespace util
{
	template<std::ranges::forward_range Rng>
	inline coroutine::Enumerator<Rng> coenumerate(Rng&& range) noexcept
	{
		for (auto&& it = range.begin(); it != range.end(); ++it)
		{
			co_yield *it;
		}
	}
}
