module;
#include <ranges>
export module Utility.Coroutine.Enumerator;
import Utility.Coroutine;

namespace util::coroutine
{
	template<typename Rng>
	concept enumerable = std::ranges::forward_range<Rng> && notvoids<std::ranges::range_value_t<Rng>>;

	namespace detail
	{
		template<typename Rng>
		concept available_data = std::ranges::range<Rng> && std::contiguous_iterator<std::ranges::iterator_t<Rng>>;
	}

	export template<std::ranges::forward_range Rng>
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

		using type = Enumerator<Rng>;
		using interface = std::ranges::view_interface<Enumerator<Rng>>;
		using promise_type = DeferredValuePromise<type, value_type>;
		using handle_type = promise_type::handle_type;

		class iterator;
		class const_iterator;

		constexpr Enumerator()
			noexcept(nothrow_default_constructibles<Rng>)
			requires(default_initializable<Rng>) = default;
		constexpr ~Enumerator()
			noexcept(nothrow_destructibles<Rng>) = default;

		template<std::ranges::forward_range Sng>
		constexpr Enumerator(Enumerator<Sng>&& other) noexcept
		{

		}

		[[nodiscard]]
		inline iterator begin() noexcept
		{
			if (!myHandle.done())
			{
				myHandle.resume();
			}

			return iterator{ myHandle };
		}

		[[nodiscard]]
		constexpr default_sentinel_t end() noexcept
		{
			return {};
		}

		[[nodiscard]]
		inline const_iterator begin() const noexcept
		{
			if (!myHandle.done())
			{
				myHandle.resume();
			}

			return const_iterator{ myHandle };
		}

		[[nodiscard]]
		constexpr default_sentinel_t end() const noexcept
		{
			return {};
		}

		constexpr auto data()
			requires detail::available_data<Rng>;

		constexpr auto data() const
			requires detail::available_data<const Rng>;

		Enumerator(const Enumerator& other) = delete;
		constexpr Enumerator(Enumerator&& other) noexcept = default;
		Enumerator& operator=(const Enumerator& other) = delete;
		constexpr Enumerator& operator=(Enumerator&& other) noexcept = default;

	private:
		handle_type myHandle;
		Rng underlyingRange;
	};

	template<std::ranges::forward_range Rng>
	Enumerator(Rng&&) -> Enumerator<Rng>;
}

export template<typename Rng>
inline constexpr bool std::ranges::enable_borrowed_range<util::coroutine::Enumerator<Rng>> = std::ranges::enable_borrowed_range<Rng>;

export namespace util
{
	template<std::ranges::forward_range Rng>
	inline coroutine::Enumerator<Rng> coenumerate(add_const_t<Rng&&> range) noexcept
	{
		for (auto&& it = range.begin(); it != range.end(); ++it)
		{
			co_yield *it;
		}
	}
}
