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
		class [[nodiscard]] Enumerable : public std::ranges::view_interface<Enumerable<Rng>>
	{
	public:
		using value_type = std::ranges::range_value_t<Rng>;
		using reference = std::ranges::range_reference_t<Rng>;
		using const_reference = std::ranges::range_reference_t<const Rng>;
		using rvalue_reference = std::ranges::range_rvalue_reference_t<Rng>;
		using const_rvalue_reference = std::ranges::range_rvalue_reference_t<const Rng>;
		using size_type = size_t;
		using difference_type = ptrdiff_t;

		using type = Enumerable<Rng>;
		using interface = std::ranges::view_interface<Enumerable<Rng>>;
		using promise_type = DeferredValuePromise<type, value_type>;
		using handle_type = promise_type::handle_type;

		class iterator;
		class const_iterator;

		constexpr Enumerable()
			noexcept(nothrow_default_constructibles<Rng>)
			requires(default_initializable<Rng>) = default;
		constexpr ~Enumerable()
			noexcept(nothrow_destructibles<Rng>) = default;

		template<std::ranges::forward_range Sng>
		constexpr Enumerable(Enumerable<Sng>&& other) noexcept
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

		Enumerable(const Enumerable& other) = delete;
		constexpr Enumerable(Enumerable&& other) noexcept = default;
		Enumerable& operator=(const Enumerable& other) = delete;
		constexpr Enumerable& operator=(Enumerable&& other) noexcept = default;

	private:
		handle_type myHandle;
		Rng underlyingRange;
	};

	template<std::ranges::forward_range Rng>
	Enumerable(Rng&&) -> Enumerable<Rng>;
}

export template<typename Rng>
inline constexpr bool std::ranges::enable_borrowed_range<util::coroutine::Enumerable<Rng>> = std::ranges::enable_borrowed_range<Rng>;

export namespace util
{
	template<std::ranges::forward_range Rng>
	inline coroutine::Enumerable<Rng> coenumerate(add_const_t<Rng&&> range) noexcept
	{
		for (auto&& it = range.begin(); it != range.end(); ++it)
		{
			co_yield *it;
		}
	}
}
