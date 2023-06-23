export module Utility.Coroutine;
import <tuple>;
import <algorithm>;
export import <coroutine>;
export import Utility.Constraints;
import Utility.Monad;
export import :Promise;
export import :Task;

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
			return coHandle.promise().value();
		}

		inline rvalue_reference operator*() &&
			noexcept(nothrow_move_constructibles<value_type>)
		{
			return move(coHandle.promise()).value();
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
			: coHandle(move(coroutine))
		{}

		inline CoIterator& operator++()
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

		inline const CoIterator& operator++() const
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
			return coHandle.promise().value();
		}

		inline const_reference operator*() const& noexcept
		{
			return coHandle.promise().value();
		}

		inline rvalue_reference operator*() &&
			noexcept(nothrow_move_constructibles<value_type>)
		{
			return move(coHandle.promise()).value();
		}

		inline const_rvalue_reference operator*() const&&
			noexcept(nothrow_move_constructibles<const value_type>)
		{
			return move(coHandle.promise()).value();
		}

		[[nodiscard]]
		inline bool operator==(default_sentinel_t) const
		{
			return !coHandle || coHandle.done();
		}

	private:
		handle_type coHandle;
	};
}

export namespace util
{
	using coroutine::coexecution;

	template<coexecution Policy>
	using Cowork = conditional_t<Policy == coexecution::Now, coroutine::RelaxedTask, coroutine::DeferredTask>;

	template<coexecution Policy, invocables Fn, invocables Pred>
		requires convertible_to<invoke_result_t<Pred>, bool>
	inline
		coroutine::RelaxedTask
		corepeat_as_if(Fn&& fn, Pred&& pred)
		noexcept(nothrow_invocables<Fn>&& nothrow_invocables<Pred>)
	{
		Fn&& functor = forward<Fn>(fn);
		Pred&& predicate = forward<Pred>(pred);

		while (predicate())
		{
			if constexpr (Policy == coexecution::Now)
			{
				co_await coroutine::suspend_never{};
			}
			else
			{
				co_await coroutine::suspend_always{};
			}

			functor();
		}
	}

	template<coexecution Policy, functions Fn, typename Pred, typename... Args>
	inline
		coroutine::RelaxedTask
		corepeat_as_if(Fn&& fn, Pred&& pred, Args&&... args)
		noexcept(noexcept(forward<Pred>(pred)()) && noexcept(forward<Fn>(fn)(forward<Args>(args)...)))
	{
		Fn&& functor = forward<Fn>(fn);
		Pred&& predicate = forward<Pred>(pred);
		const std::tuple<Args&&...> arguments = std::forward_as_tuple(forward<Args>(args)...);

		while (predicate())
		{
			if constexpr (Policy == coexecution::Now)
			{
				co_await coroutine::suspend_never{};
			}
			else
			{
				co_await coroutine::suspend_always{};
			}

			std::apply(functor, arguments);
		}
	}

	template<invocables Fn, invocables Pred>
		requires convertible_to<invoke_result_t<Pred>, bool>
	inline
		auto
		corepeat_if(Fn&& fn, Pred&& pred)
		noexcept(nothrow_invocables<Fn>&& nothrow_invocables<Pred>)
	{
		return corepeat_as_if<coexecution::Later>(forward<Fn>(fn), forward<Pred>(pred));
	}

	template<functions Fn, functions Pred, typename... Args>
	inline
		auto
		corepeat_if(Fn&& fn, Pred&& pred, Args&&... args)
		noexcept(noexcept(forward<Pred>(pred)()) && noexcept(forward<Fn>(fn)(forward<Args>(args)...)))
	{
		return corepeat_as_if<coexecution::Later>(forward<Fn>(fn), forward<Pred>(pred), forward<Args>(args)...);
	}

	template<functions Fn, r_invocables<bool> Pred, typename... Args>
	inline
		auto
		corepeat_if(Fn&& fn, Pred&& pred, Args&&... args)
		noexcept(nothrow_invocables<Pred> && noexcept(forward<Fn>(fn)(forward<Args>(args)...)))
	{
		return corepeat_as_if<coexecution::Later>(forward<Fn>(fn), forward<Pred>(pred), forward<Args>(args)...);
	}

	template<coexecution Policy, r_invocables<bool> Fn>
	inline
		coroutine::RelaxedTask
		corepeat_as(Fn&& fn)
		noexcept(nothrow_invocables<Fn>)
	{
		Fn&& functor = forward<Fn>(fn);

		while (true)
		{
			if constexpr (Policy == coexecution::Now)
			{
				co_await coroutine::suspend_never{};
			}
			else
			{
				co_await coroutine::suspend_always{};
			}

			if (!functor())
			{
				co_return;
			}
		}
	}

	template<coexecution Policy, functions Fn, typename... Args>
	inline
		coroutine::RelaxedTask
		corepeat_as(Fn&& fn, Args&&... args)
		noexcept(noexcept(forward<Fn>(fn)(forward<Args>(args)...)))
	{
		Fn&& functor = forward<Fn>(fn);
		const std::tuple<Args&&...> arguments = std::forward_as_tuple(forward<Args>(args)...);

		while (true)
		{
			if constexpr (Policy == coexecution::Now)
			{
				co_await coroutine::suspend_never{};
			}
			else
			{
				co_await coroutine::suspend_always{};
			}

			if (!std::apply(functor, arguments))
			{
				co_return;
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

	template<functions Fn, typename... Args>
	inline
		coroutine::RelaxedTask
		corepeat(Fn&& fn, Args&&... args)
		noexcept(noexcept(forward<Fn>(fn)(forward<Args>(args)...)))
	{
		return corepeat_as<coexecution::Later>(forward<Fn>(fn), forward<Args>(args)...);
	}
}

#pragma warning(push, 1)
namespace util::test
{
	void test_coroutines()
	{}
}
#pragma warning(pop)
