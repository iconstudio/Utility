export module Utility.Coroutine:CoIterator;
export import <algorithm>;
export import <iterator>;
export import Utility.Constraints;

export namespace util::coroutine
{
	using ::std::default_sentinel_t;

	template<typename Coroutine>
	class ConstCoIterator
	{
	public:
		using coro_type = Coroutine;
		using handle_type = coro_type::handle_type;

		using iterator_concept = std::forward_iterator_tag;
		using iterator_category = std::forward_iterator_tag;
		using value_type = coro_type::value_type;
		using reference = coro_type::reference;
		using const_reference = coro_type::const_reference;
		using rvalue_reference = coro_type::rvalue_reference;
		using const_rvalue_reference = coro_type::const_rvalue_reference;
		using size_type = coro_type::size_type;
		using difference_type = coro_type::difference_type;

		constexpr ConstCoIterator()
			noexcept(nothrow_default_constructibles<handle_type>)
			requires default_initializables<handle_type> = default;
		constexpr ~ConstCoIterator() noexcept(nothrow_destructibles<handle_type>) = default;

		explicit constexpr ConstCoIterator(const handle_type& coroutine) noexcept
			: coHandle(coroutine)
		{}

		explicit constexpr ConstCoIterator(handle_type&& coroutine) noexcept
			: coHandle(std::move(coroutine))
		{}

		inline ConstCoIterator& operator++() noexcept
		{
			if (!coHandle.done())
			{
				coHandle.resume();
			}

			return *this;
		}

		inline void operator++(int) noexcept
		{
			if (!coHandle.done())
			{
				coHandle.resume();
			}
		}

		inline const ConstCoIterator& operator++() const noexcept
		{
			if (!coHandle.done())
			{
				coHandle.resume();
			}

			return *this;
		}

		inline void operator++(int) const noexcept
		{
			if (!coHandle.done())
			{
				coHandle.resume();
			}
		}

		inline const_reference operator*() const& noexcept
		{
			return coHandle.promise().value();
		}

		inline rvalue_reference operator*() &&
			noexcept(nothrow_move_constructibles<value_type>)
		{
			return std::move(coHandle.promise()).value();
		}

		[[nodiscard]]
		inline bool operator==(default_sentinel_t) const noexcept
		{
			return !coHandle || coHandle.done();
		}

	private:
		handle_type coHandle;
	};

	template<typename Coroutine>
	class CoIterator
	{
	public:
		using coro_type = Coroutine;
		using handle_type = coro_type::handle_type;

		using iterator_concept = std::forward_iterator_tag;
		using iterator_category = std::forward_iterator_tag;
		using value_type = coro_type::value_type;
		using reference = coro_type::reference;
		using const_reference = coro_type::const_reference;
		using rvalue_reference = coro_type::rvalue_reference;
		using const_rvalue_reference = coro_type::const_rvalue_reference;
		using size_type = coro_type::size_type;
		using difference_type = coro_type::difference_type;

		constexpr CoIterator()
			noexcept(nothrow_default_constructibles<handle_type>)
			requires default_initializables<handle_type> = default;
		constexpr ~CoIterator() noexcept(nothrow_destructibles<handle_type>) = default;

		explicit constexpr CoIterator(const handle_type& coroutine) noexcept
			: coHandle(coroutine)
		{}

		explicit constexpr CoIterator(handle_type&& coroutine) noexcept
			: coHandle(std::move(coroutine))
		{}

		inline CoIterator& operator++() noexcept
		{
			if (!coHandle.done())
			{
				coHandle.resume();
			}

			return *this;
		}

		inline void operator++(int) noexcept
		{
			if (!coHandle.done())
			{
				coHandle.resume();
			}
		}

		inline const CoIterator& operator++() const noexcept
		{
			if (!coHandle.done())
			{
				coHandle.resume();
			}

			return *this;
		}

		inline void operator++(int) const noexcept
		{
			if (!coHandle.done())
			{
				coHandle.resume();
			}
		}

		inline reference operator*() & noexcept
		{
			return coHandle.promise().value();
		}

		inline const_reference operator*() const& noexcept
		{
			return coHandle.promise().value();
		}

		inline rvalue_reference operator*() &&
			noexcept(nothrow_move_constructibles<value_type>)
		{
			return std::move(coHandle.promise()).value();
		}

		inline const_rvalue_reference operator*() const&&
			noexcept(nothrow_move_constructibles<const value_type>)
		{
			return std::move(coHandle.promise()).value();
		}

		[[nodiscard]]
		inline bool operator==(default_sentinel_t) const noexcept
		{
			return !coHandle || coHandle.done();
		}

	private:
		handle_type coHandle;
	};
}
