export module Utility.Coroutine;
import <algorithm>;
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
