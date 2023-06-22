export module Utility.Coroutine;
import <memory>;
import <coroutine>;
export import Utility.Constraints;
import Utility.Monad;
export import :Promise;

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

	class [[nodiscard]] DeferredTask
	{
	public:
		using promise_type = defer<DeferredTask>;
		using handle_type = coroutine_handle<promise_type>;

		explicit constexpr DeferredTask(const handle_type& handle) noexcept
			: myHandle(handle)
		{}

		explicit constexpr DeferredTask(handle_type&& handle) noexcept
			: myHandle(move(handle))
		{}

		[[nodiscard]]
		bool Done() const noexcept
		{
			return myHandle.done();
		}

		void Resume() const noexcept
		{
			if (!myHandle.done())
			{
				myHandle.resume();
			}
		}

		void operator()() const noexcept
		{
			Resume();
		}

		~DeferredTask() noexcept
		{
			if (myHandle.done())
			{
				myHandle.destroy();
			}
		}

		handle_type myHandle;
	};

	class [[nodiscard]] RelaxedTask
	{
	public:
		using promise_type = relax<RelaxedTask>;
		using handle_type = coroutine_handle<promise_type>;

		explicit constexpr RelaxedTask(const handle_type& handle) noexcept
			: myHandle(handle)
		{}

		explicit constexpr RelaxedTask(handle_type&& handle) noexcept
			: myHandle(move(handle))
		{}

		[[nodiscard]]
		bool Done() const noexcept
		{
			return myHandle.done();
		}

		void Resume() const noexcept
		{
			if (!myHandle.done())
			{
				myHandle.resume();
			}
		}

		void operator()() const noexcept
		{
			Resume();
		}

		~RelaxedTask() noexcept
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

	template<typename Coroutine>
	class [[nodiscard]] ValuePromise
	{
	public:
		using coro_type = Coroutine;
		using handle_type = std::coroutine_handle<ValuePromise<Coroutine>>;

		using value_type = coro_type::value_type;
		using reference = coro_type::reference;
		using const_reference = coro_type::const_reference;
		using rvalue_reference = coro_type::rvalue_reference;
		using const_rvalue_reference = coro_type::const_rvalue_reference;
		using size_type = coro_type::size_type;
		using difference_type = coro_type::difference_type;

		constexpr ValuePromise()
			noexcept(nothrow_default_constructibles<value_type>)
			requires default_initializables<value_type> = default;
		constexpr ~ValuePromise() noexcept(nothrow_destructibles<value_type>) = default;

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
}

export namespace util
{
	using coroutine::coexecution;

	template<coexecution Policy>
	using Cowork = conditional_t<Policy == coexecution::Now, coroutine::RelaxedTask, coroutine::DeferredTask>;

	template<coexecution Policy, typename Fn, typename Pred>
		requires invocables<Fn>&& invocables<Pred>&& convertible_to<invoke_result_t<Pred>, bool>
	inline
		Cowork<Policy>
		corepeat_as_if(Fn&& fn, Pred&& pred)
		noexcept(nothrow_invocables<Fn>&& nothrow_invocables<Pred>)
	{
		Fn functor = forward<Fn>(fn);
		Pred predicate = forward<Pred>(pred);

		while (predicate())
		{
			functor();

			if constexpr (Policy == coexecution::Now)
			{
				co_await coroutine::suspend_never{};
			}
			else
			{
				co_await coroutine::suspend_always{};
			}
		}
	}

	template<typename Fn, typename Pred>
		requires invocables<Fn>&& invocables<Pred>&& convertible_to<invoke_result_t<Pred>, bool>
	inline
		auto
		corepeat_if(Fn&& fn, Pred&& pred)
		noexcept(nothrow_invocables<Fn>&& nothrow_invocables<Pred>)
	{
		return corepeat_as_if<coexecution::Later>(forward<Fn>(fn), forward<Pred>(pred));
	}

	template<coexecution Policy, r_invocables<bool> Fn>
	inline
		coroutine::RelaxedTask
		corepeat_as(Fn&& fn)
		noexcept(nothrow_invocables<Fn>)
	{
		Fn functor = forward<Fn>(fn);

		while (true)
		{
			if (!functor())
			{
				co_return;
			}

			if constexpr (Policy == coexecution::Now)
			{
				co_await coroutine::suspend_never{};
			}
			else
			{
				co_await coroutine::suspend_always{};
			}
		}
	}

	template<r_invocables<bool> Fn>
	inline
		coroutine::RelaxedTask
		corepeat(Fn&& fn)
		noexcept(nothrow_invocables<Fn>)
	{
		return corepeat_as<coexecution::Later>(forward<Fn>(fn));
	}
}

#pragma warning(push, 1)
namespace util::test
{
	void test_coroutines()
	{
	}
}
#pragma warning(pop)
