export module Utility.Coroutine;
import <tuple>;
export import <coroutine>;
export import :CoIterator;
export import :Promise;
export import :Task;
export import Utility;
export import Utility.Constraints;

export namespace util::coroutine
{
	template<coexecution Policy>
	using Cowork = conditional_t<Policy == coexecution::Now, RelaxedTask, DeferredTask>;
}

export namespace util
{
	using coroutine::coexecution;

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

	template<coexecution Policy, classes Host, typename Method>
	inline
		coroutine::RelaxedTask
		corepeat_as_by(Host& host, Method&& fn)
		noexcept(method_noexcept<Method, Host&>::value)
	{
		static_assert(method_by<Method, Host&>, "The method does not take Host& as its first parameter.");

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

			if (!(host.*fn)())
			{
				co_return;
			}
		}
	}

	template<coexecution Policy, classes Host, typename Method>
	inline
		coroutine::RelaxedTask
		corepeat_as_by(const Host& host, Method&& fn)
		noexcept(method_noexcept<Method, const Host&>::value)
	{
		static_assert(method_by<Method, const Host&>, "The method does not take const Host& as its first parameter.");

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

			if (!(host.*fn)())
			{
				co_return;
			}
		}
	}

	template<coexecution Policy, classes Host, typename Method>
	inline
		coroutine::RelaxedTask
		corepeat_as_by(Host&& host, Method&& fn)
		noexcept(method_noexcept<Method, Host&&>::value)
	{
		static_assert(method_by<Method, Host&&>, "The method does not take Host&& as its first parameter.");

		Host localhost = static_cast<Host&&>(host);

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

			if (!(localhost.*fn)())
			{
				co_return;
			}
		}
	}

	template<coexecution Policy, classes Host, typename Method>
	inline
		coroutine::RelaxedTask
		corepeat_as_by(const remove_reference_t<Host>&& host, Method&& fn)
		noexcept(method_noexcept<Method, const Host&&>::value)
	{
		static_assert(method_by<Method, const Host&&>, "The method does not take const Host&& as its first parameter.");

		const Host localhost = static_cast<const Host&&>(host);

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

			if (!(localhost.*fn)())
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
	struct test_coclass
	{
		constexpr bool test_memfn1() noexcept { return false; }
		constexpr bool test_memfn2() const noexcept { return false; }
	};

	void test_coroutines()
	{
		constexpr auto ptr1 = &test_coclass::test_memfn1;
		constexpr auto ptr2 = &test_coclass::test_memfn2;

		using ftype1 = decltype(ptr1);
		using ftype2 = decltype(ptr2);

		test_coclass cocl1{};
		corepeat_as_by<coexecution::Now>(cocl1, &test_coclass::test_memfn1);
		corepeat_as_by<coexecution::Now>(cocl1, &test_coclass::test_memfn2);
		corepeat_as_by<coexecution::Now>(std::move(cocl1), &test_coclass::test_memfn1);
		corepeat_as_by<coexecution::Now>(std::move(cocl1), &test_coclass::test_memfn2);
		corepeat_as_by<coexecution::Now>(test_coclass{}, &test_coclass::test_memfn1);
		corepeat_as_by<coexecution::Now>(test_coclass{}, &test_coclass::test_memfn2);

		const test_coclass cocl2{};
		corepeat_as_by<coexecution::Now>(cocl2, &test_coclass::test_memfn1);
		corepeat_as_by<coexecution::Now>(cocl2, &test_coclass::test_memfn2);

		constexpr test_coclass cocl3{};
		corepeat_as_by<coexecution::Now>(cocl3, &test_coclass::test_memfn1); //
		corepeat_as_by<coexecution::Now>(cocl3, &test_coclass::test_memfn2);

		corepeat_as_by<coexecution::Now>(std::move(cocl3), &test_coclass::test_memfn1); //
		corepeat_as_by<coexecution::Now>(std::move(cocl3), &test_coclass::test_memfn2);
	}
}
#pragma warning(pop)
