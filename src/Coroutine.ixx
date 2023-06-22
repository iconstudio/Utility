export module Utility.Coroutine;
import <memory>;
import <ranges>;
export import <coroutine>;
export import Utility.Traits;
export import Utility.Constraints;
import Utility.Monad;

export namespace util::coroutine
{
	using ::std::coroutine_traits;
	using ::std::coroutine_handle;
	using ::std::noop_coroutine_promise;
	using ::std::noop_coroutine_handle;
	using ::std::noop_coroutine;
	using ::std::is_corresponding_member;
	using ::std::operator==;
	using ::std::operator<=>;
	using ::std::hash;
	using ::std::suspend_never;
	using ::std::suspend_always;
	using ::std::default_sentinel_t;

	template<typename T>
	concept awaitable = requires(T t)
	{
		t.await_ready();
		t.await_suspend();
		t.await_resume();
	};

	template<typename Coroutine>
	class [[nodiscard]] default_promise
	{
	public:
		using coro_type = Coroutine;
		using handle_type = std::coroutine_handle<default_promise<Coroutine>>;

		using value_type = coro_type::value_type;
		using reference = coro_type::reference;
		using const_reference = coro_type::const_reference;
		using rvalue_reference = coro_type::rvalue_reference;
		using const_rvalue_reference = coro_type::const_rvalue_reference;
		using size_type = coro_type::size_type;
		using difference_type = coro_type::difference_type;

		constexpr default_promise()
			noexcept(nothrow_default_constructibles<value_type>)
			requires default_initializables<value_type> = default;
		constexpr ~default_promise() noexcept(nothrow_destructibles<value_type>) = default;

		[[nodiscard]]
		coro_type get_return_object() noexcept
		{
			return coro_type{ handle_type::from_promise(*this) };
		}

		static suspend_always initial_suspend() noexcept
		{
			return {};
		}

		static suspend_always final_suspend() noexcept
		{
			return {};
		}

		suspend_always yield_value(rvalue_reference value)
			noexcept(nothrow_move_constructibles<value_type>)
		{
			currentValue = move(value);
			return {};
		}

		suspend_always yield_value(const_rvalue_reference value)
			noexcept(nothrow_move_constructibles<const value_type>)
		{
			currentValue = move(value);
			return {};
		}

		suspend_always yield_value(const_reference value)
			noexcept(nothrow_copy_constructibles<value_type>)
			requires move_constructibles<value_type>
		{
			currentValue = value;
			return {};
		}

		[[noreturn]]
		void return_void() const noexcept {}

		[[noreturn]]
		static void unhandled_exception()
		{
			throw;
		}

		// Disallow co_await
		void await_transform() = delete;

		Monad<value_type> currentValue;
	};

	template<typename T>
	class Task
	{
	public:
		using type = Task<T>;
		struct promise_type
		{
			Task get_return_object()
			{
				return Task{ coroutine_handle<promise_type>::from_promise(*this) };
			}

			static suspend_always initial_suspend() noexcept
			{
				return {};
			}

			static suspend_never final_suspend() noexcept
			{
				return {};
			}

			[[noreturn]]
			void return_void() const noexcept {}

			[[noreturn]]
			static void unhandled_exception()
			{
				throw;
			}
		};
		using handle_type = coroutine_handle<promise_type>;

		explicit constexpr Task(const handle_type& handle) noexcept
			: myHandle(handle)
		{}

		explicit constexpr Task(handle_type&& handle) noexcept
			: myHandle(move(handle))
		{}

		[[nodiscard]]
		bool Done() const noexcept
		{
			return myHandle.done();
		}

		void Resume() const noexcept
		{
			myHandle.resume();
		}

		void operator()() const noexcept
		{
			myHandle.resume();
		}

		~Task() noexcept
		{
			if (myHandle.done())
			{
				myHandle.destroy();
			}
		}

		handle_type myHandle;
	};

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
			: coHandle(move(coroutine))
		{}

		inline ConstCoIterator& operator++()
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

		inline const ConstCoIterator& operator++() const
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

		inline const_reference operator*() const&
		{
			return *(coHandle.promise().currentValue);
		}

		inline rvalue_reference operator*() &&
			noexcept(nothrow_move_constructibles<value_type>)
		{
			return *move(coHandle.promise().currentValue);
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
		using promise_type = default_promise<type>;
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

	template<movable T>
	class [[nodiscard]] Generator
	{
	public:
		using value_type = clean_t<T>;
		using reference = T&;
		using const_reference = const T&;
		using rvalue_reference = T&&;
		using const_rvalue_reference = const T&&;
		using size_type = size_t;
		using difference_type = ptrdiff_t;

		using type = Generator<T>;
		using promise_type = default_promise<type>;
		using handle_type = promise_type::handle_type;

		using iterator = ConstCoIterator<type>;
		using const_iterator = iterator;

		inline Generator() noexcept = default;
		inline ~Generator() noexcept
		{
			if (myHandle.done())
			{
				myHandle.destroy();
			}
		}

		explicit constexpr Generator(const handle_type& coroutine)
			noexcept(nothrow_copy_constructibles<handle_type>)
			: myHandle(coroutine)
		{}

		explicit constexpr Generator(handle_type&& coroutine)
			noexcept(nothrow_move_constructibles<handle_type>)
			: myHandle(move(coroutine))
		{}

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

		[[nodiscard]]
		inline const_iterator cbegin() const noexcept
		{
			if (!myHandle.done())
			{
				myHandle.resume();
			}

			return const_iterator{ myHandle };
		}

		[[nodiscard]]
		constexpr default_sentinel_t cend() const noexcept
		{
			return {};
		}

		[[nodiscard]]
		bool done() const noexcept
		{
			return myHandle.done();
		}

		[[nodiscard]]
		bool empty() const noexcept
		{
			return !myHandle || myHandle.done();
		}

		[[nodiscard]]
		explicit operator bool() const noexcept
		{
			return !empty();
		}

		Generator(const Generator& other) = delete;
		constexpr Generator(Generator&& other) noexcept = default;
		Generator& operator=(const Generator& other) = delete;
		constexpr Generator& operator=(Generator&& other) noexcept = default;

	private:
		handle_type myHandle;
	};
}

export namespace util
{
	template<typename Fn, typename Pred>
		requires invocables<Fn>&& invocables<Pred>&& convertible_to<invoke_result_t<Pred>, bool>
	inline
		coroutine::Task<invoke_result_t<Fn>>
		corepeat(Fn&& fn, Pred&& pred)
		noexcept(nothrow_invocables<Fn>&& nothrow_invocables<Pred>)
	{
		Fn functor = forward<Fn>(fn);
		Pred predicate = forward<Pred>(pred);

		while (predicate())
		{
			co_yield functor();
		}
	}

	template<typename Fn>
		requires invocables<Fn>
	inline
		coroutine::Task<void>
		corepeat(Fn&& fn)
		noexcept(nothrow_invocables<Fn>)
	{
		Fn functor = forward<Fn>(fn);

		while (true)
		{
			functor();

			co_await coroutine::suspend_always{};
		}
	}

	template<movable T, equality_comparable_with<T> Guard>
	inline coroutine::Generator<T> cogenerate(T first, const Guard last)
		noexcept(nothrow_copy_constructibles<T>)
	{
		while (first != last)
		{
			co_yield first++;
		}
	}

	template<std::ranges::forward_range Rng>
	inline coroutine::Enumerable<Rng> coenumerate(Rng&& range) noexcept
	{
		for (auto&& it = range.begin(); it != range.end(); ++it)
		{
			co_yield *it;
		}
	}
}

#pragma warning(push, 1)
namespace util::test
{
	void test_coroutines()
	{
		const coroutine::Generator aa = cogenerate(1, 10);

		for (auto&& val : aa)
		{
		}
	}
}
#pragma warning(pop)
