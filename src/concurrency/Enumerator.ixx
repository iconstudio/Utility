export module Utility.Coroutine.Enumerator;
import <ranges>;
import Utility.Coroutine;

export namespace util::coroutine
{
	template<std::ranges::forward_range Rng>
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

		template<std::ranges::forward_range Sng>
		constexpr Enumerable(Enumerable<Sng>&& other) noexcept
		{

		}

		Enumerable(const Enumerable& other) = delete;
		constexpr Enumerable(Enumerable&& other) noexcept = default;
		Enumerable& operator=(const Enumerable& other) = delete;
		constexpr Enumerable& operator=(Enumerable&& other) noexcept = default;

	private:
		handle_type myHandle;
	};

	template<std::ranges::forward_range Rng>
	Enumerable(Rng&&) -> Enumerable<Rng>;
}

export template<typename Rng>
inline constexpr bool std::ranges::enable_borrowed_range<util::coroutine::Enumerable<Rng>> = std::ranges::enable_borrowed_range<Rng>;

export namespace util
{
	template<std::ranges::forward_range Rng>
	inline coroutine::Enumerable<Rng> coenumerate(Rng&& range) noexcept
	{
		std::ranges::borrowed_range<Rng>;

		for (auto&& it = range.begin(); it != range.end(); ++it)
		{
			co_yield *it;
		}
	}
}
