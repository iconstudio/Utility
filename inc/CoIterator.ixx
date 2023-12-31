export module Utility.Coroutine:CoIterator;
export import <iterator>;
export import Utility;
export import Utility.Constraints;

export namespace util::coroutine
{
	using ::std::default_sentinel_t;

	template<typename Coroutine>
	class CoConstIterator
	{
	public:
		using coro_type = Coroutine;
		using handle_type = coro_type::handle_type;

		using iterator_concept = std::input_iterator_tag;
		using iterator_category = std::input_iterator_tag;
		using value_type = coro_type::value_type;
		using reference = coro_type::reference;
		using const_reference = coro_type::const_reference;
		using rvalue_reference = coro_type::rvalue_reference;
		using const_rvalue_reference = coro_type::const_rvalue_reference;
		using size_type = coro_type::size_type;
		using difference_type = coro_type::difference_type;

		constexpr CoConstIterator()
			noexcept(nothrow_default_constructibles<handle_type>)
			requires default_initializables<handle_type> = default;
		constexpr ~CoConstIterator()
			noexcept(nothrow_destructibles<handle_type>) = default;

		explicit constexpr CoConstIterator(const handle_type& coroutine) noexcept
			: coHandle(coroutine)
		{}

		explicit constexpr CoConstIterator(handle_type&& coroutine) noexcept
			: coHandle(util::move(coroutine))
		{}

		inline CoConstIterator& operator++() noexcept
		{
			Resume();

			return *this;
		}

		inline void operator++(int) noexcept
		{
			Resume();
		}

		inline const CoConstIterator& operator++() const noexcept
		{
			Resume();

			return *this;
		}

		inline void operator++(int) const noexcept
		{
			Resume();
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

		[[nodiscard]]
		inline bool operator==(const CoConstIterator& other) const noexcept
		{
			return coHandle == other.coHandle;
		}

		constexpr CoConstIterator(const CoConstIterator& other)
			noexcept(nothrow_copy_constructibles<handle_type>)
			requires copyable<handle_type> = default;
		constexpr CoConstIterator(CoConstIterator&& other)
			noexcept(nothrow_move_constructibles<handle_type>)
			requires movable<handle_type> = default;
		constexpr CoConstIterator& operator=(const CoConstIterator& other)
			noexcept(nothrow_copy_assignables<handle_type>)
			requires copyable<handle_type> = default;
		constexpr CoConstIterator& operator=(CoConstIterator&& other)
			noexcept(nothrow_move_assignables<handle_type>)
			requires movable<handle_type> = default;

	protected:
		[[nodiscard]]
		constexpr const handle_type& Handle() const& noexcept
		{
			return coHandle;
		}

		[[nodiscard]]
		constexpr handle_type&& Handle() && noexcept
		{
			return move(coHandle);
		}

		inline void Resume() const noexcept
		{
			if (!coHandle.done())
			{
				coHandle.resume();
			}
		}

	private:
		handle_type coHandle;
	};

	template<typename Coroutine>
	class CoForwardIterator : private CoConstIterator<Coroutine>
	{
	public:
		using base = CoConstIterator<Coroutine>;
		using coro_type = base::coro_type;
		using handle_type = base::handle_type;
		using value_type = base::value_type;
		using reference = base::reference;
		using const_reference = base::const_reference;
		using rvalue_reference = base::rvalue_reference;
		using const_rvalue_reference = base::const_rvalue_reference;
		using size_type = base::size_type;
		using difference_type = base::difference_type;

		using iterator_concept = std::forward_iterator_tag;
		using iterator_category = std::forward_iterator_tag;

		constexpr CoForwardIterator()
			noexcept(nothrow_default_constructibles<handle_type>)
			requires default_initializables<handle_type> = default;
		constexpr ~CoForwardIterator()
			noexcept(nothrow_destructibles<handle_type>) = default;

		using base::base;
		using base::operator==;

		inline CoForwardIterator& operator++() noexcept
		{
			base::Resume();

			return *this;
		}

		inline void operator++(int) noexcept
		{
			base::Resume();
		}

		inline const CoForwardIterator& operator++() const noexcept
		{
			base::Resume();

			return *this;
		}

		inline void operator++(int) const noexcept
		{
			base::Resume();
		}

		inline reference operator*() & noexcept
		{
			return Handle().promise().value();
		}

		inline const_reference operator*() const& noexcept
		{
			return Handle().promise().value();
		}

		inline rvalue_reference operator*() &&
			noexcept(nothrow_move_constructibles<value_type>)
		{
			return move(Handle().promise()).value();
		}

		inline const_rvalue_reference operator*() const&&
			noexcept(nothrow_move_constructibles<const value_type>)
		{
			return move(Handle().promise()).value();
		}

	protected:
		using base::Handle;
		using base::Resume;
	};
}
