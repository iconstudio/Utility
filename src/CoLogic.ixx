export module Utility.Coroutine.Logic;
import <algorithm>;
import <ranges>;
import <vector>;
import Utility;
import Utility.Coroutine;
import Utility.Coroutine.Enumerator;
import Utility.Coroutine.Generator;

namespace util
{
}

export namespace util
{
	using coroutine::coexecution;

	template<coexecution Policy, coroutine::enumerable Rng, typename Pred>
	inline
		coroutine::Cowork<Policy>
		co_each_as(Rng&& rng, Pred&& predicate)
		noexcept
	{
		auto&& pred = forward<Pred>(predicate);

		for (auto&& value : util::coenumerate(forward<Rng>(rng)))
		{
			co_await pred(value);
		}
	}

	template<coroutine::enumerable Rng, typename Pred>
	inline
		auto
		co_each(Rng&& rng, Pred&& predicate)
		noexcept
	{
		return co_each_as<coexecution::Later>(forward<Rng>(rng), forward<Pred>(predicate));
	}

	template<coexecution Policy, invocables Fn, invocables Pred>
		requires convertible_to<invoke_result_t<Pred>, bool>
	inline
		coroutine::Cowork<Policy>
		corepeat_as_if(Fn&& fn, Pred&& pred)
		noexcept(nothrow_invocables<Fn>&& nothrow_invocables<Pred>)
	{
		Fn&& functor = forward<Fn>(fn);
		Pred&& predicate = forward<Pred>(pred);

		while (predicate())
		{
			functor();

			co_await coroutine::suspend_always{};
		}
	}

	template<coexecution Policy, functions Fn, typename Pred, typename... Args>
	inline
		coroutine::Cowork<Policy>
		corepeat_as_if(Fn&& fn, Pred&& pred, Args&&... args)
		noexcept(noexcept(forward<Pred>(pred)()) && noexcept(forward<Fn>(fn)(forward<Args>(args)...)))
	{
		Fn&& functor = forward<Fn>(fn);
		Pred&& predicate = forward<Pred>(pred);
		const std::tuple<Args&&...> arguments = std::forward_as_tuple(forward<Args>(args)...);

		while (predicate())
		{
			std::apply(functor, arguments);

			co_await coroutine::suspend_always{};
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
		coroutine::Cowork<Policy>
		corepeat_as(Fn&& fn)
		noexcept(nothrow_invocables<Fn>)
	{
		Fn&& functor = forward<Fn>(fn);

		while (true)
		{
			if (!functor())
			{
				co_return;
			}

			co_await coroutine::suspend_always{};
		}
	}

	template<coexecution Policy, classes Host, typename Method>
	inline
		coroutine::Cowork<Policy>
		corepeat_as(Host&& host, Method&& fn)
		noexcept(CheckMethodException<Host&&, Method&&>())
	{
		static_assert(method_by<Method, Host&&>, "The method does not take proper specifier");

		while (true)
		{
			if (!(forward<Host>(host).*forward<Method>(fn))())
			{
				co_return;
			}

			co_await coroutine::suspend_always{};
		}
	}

	template<coexecution Policy, functions Fn, typename... Args>
	inline
		coroutine::Cowork<Policy>
		corepeat_as(Fn&& fn, Args&&... args)
		noexcept(noexcept(forward<Fn>(fn)(forward<Args>(args)...)))
	{
		Fn&& functor = forward<Fn>(fn);
		const std::tuple<Args&&...> arguments = std::forward_as_tuple(forward<Args>(args)...);

		while (true)
		{
			if (!std::apply(functor, arguments))
			{
				co_return;
			}

			co_await coroutine::suspend_always{};
		}
	}

	template<r_invocables<bool> Fn>
	inline
		auto
		corepeat(Fn&& fn)
		noexcept(nothrow_invocables<Fn>)
	{
		return corepeat_as<coexecution::Later>(forward<Fn>(fn));
	}

	template<functions Fn, typename... Args>
	inline
		auto
		corepeat(Fn&& fn, Args&&... args)
		noexcept(noexcept(forward<Fn>(fn)(forward<Args>(args)...)))
	{
		return corepeat_as<coexecution::Later>(forward<Fn>(fn), forward<Args>(args)...);
	}

	template<movable T>
	inline coroutine::Generator<T>
		coiota(T&& first)
		noexcept(nothrow_constructibles<T, T&&>)
	{
		T val = forward<T>(first);

		co_yield val++;
	}

	template<movable T, movable Last>
	inline coroutine::Generator<T>
		coiota(T&& first, const Last& last)
		noexcept(nothrow_constructibles<T, T&&>&& nothrow_copy_constructibles<Last>)
	{
		T val = forward<T>(first);

		while (last != val)
		{
			co_yield val++;
		}
	}

	template<movable T, typename Fn, typename... Args>
		requires invocables<Fn, T, Args...>
	inline coroutine::Generator<T>
		coiota(T&& first, Fn&& fn, Args&&... args)
		noexcept(nothrow_constructibles<T, T&&>&& nothrow_invocables<Fn, T, Args...>)
	{
		auto&& pred = forward<Fn>(fn);
		T val = forward<T>(first);

		if constexpr (0 < sizeof...(Args))
		{
			const std::tuple<Args&&...> arguments = std::forward_as_tuple(forward<Args>(args)...);
			do
			{
				co_yield val;

				val = std::apply(pred, std::tuple_cat(std::forward_as_tuple(val), arguments));
			}
			while (true);
		}
		else
		{
			do
			{
				co_yield val;

				val = pred(val);
			}
			while (true);
		}
	}
}

#pragma warning(push, 1)
namespace util::test
{
	struct test_coclass
	{
		constexpr bool test_memfn1() { return false; }
		constexpr bool test_memfn2() const { return false; }
		constexpr bool test_memfn3() noexcept { return false; }
		constexpr bool test_memfn4() const noexcept { return false; }
	};

	void test_coroutines()
	{
		constexpr auto ptr1 = &test_coclass::test_memfn1;
		constexpr auto ptr2 = &test_coclass::test_memfn2;
		constexpr auto ptr3 = &test_coclass::test_memfn3;
		constexpr auto ptr4 = &test_coclass::test_memfn4;

		using ftype1 = decltype(ptr1);
		using ftype2 = decltype(ptr2);
		using ftype3 = decltype(ptr3);
		using ftype4 = decltype(ptr4);

		test_coclass cocl1{};
		corepeat_as<coexecution::Now>(cocl1, &test_coclass::test_memfn1);
		corepeat_as<coexecution::Now>(cocl1, &test_coclass::test_memfn2);
		corepeat_as<coexecution::Now>(cocl1, &test_coclass::test_memfn3);
		corepeat_as<coexecution::Now>(cocl1, &test_coclass::test_memfn4);

		corepeat_as<coexecution::Now>(std::move(cocl1), &test_coclass::test_memfn1);
		corepeat_as<coexecution::Now>(std::move(cocl1), &test_coclass::test_memfn2);
		corepeat_as<coexecution::Now>(std::move(cocl1), &test_coclass::test_memfn3);
		corepeat_as<coexecution::Now>(std::move(cocl1), &test_coclass::test_memfn4);

		corepeat_as<coexecution::Now>(test_coclass{}, &test_coclass::test_memfn1);
		corepeat_as<coexecution::Now>(test_coclass{}, &test_coclass::test_memfn2);
		corepeat_as<coexecution::Now>(test_coclass{}, &test_coclass::test_memfn3);
		corepeat_as<coexecution::Now>(test_coclass{}, &test_coclass::test_memfn4);

		const test_coclass cocl2{};
		//corepeat_as<coexecution::Now>(cocl2, &test_coclass::test_memfn1); //!
		corepeat_as<coexecution::Now>(cocl2, &test_coclass::test_memfn2);
		//corepeat_as<coexecution::Now>(cocl2, &test_coclass::test_memfn3); //!
		corepeat_as<coexecution::Now>(cocl2, &test_coclass::test_memfn4);

		constexpr test_coclass cocl3{};
		//corepeat_as<coexecution::Now>(cocl3, &test_coclass::test_memfn1); //!
		corepeat_as<coexecution::Now>(cocl3, &test_coclass::test_memfn2);
		//corepeat_as<coexecution::Now>(cocl3, &test_coclass::test_memfn3); //!
		corepeat_as<coexecution::Now>(cocl3, &test_coclass::test_memfn4);

		//corepeat_as<coexecution::Now>(std::move(cocl3), &test_coclass::test_memfn1); //!
		corepeat_as<coexecution::Now>(std::move(cocl3), &test_coclass::test_memfn2);
		//corepeat_as<coexecution::Now>(std::move(cocl3), &test_coclass::test_memfn3); //!
		corepeat_as<coexecution::Now>(std::move(cocl3), &test_coclass::test_memfn4);

		const std::vector vb{ 0, 2, 34, 54, 56, 654, 75 };

		auto io_range1 = coiota(0, 15);
		auto io_range2 = coiota(0, [](const int& curr) {
			return curr * 2;
		});

		for (auto&& val : io_range1)
		{}

		for (auto&& val : io_range2)
		{}

		auto bb = io_range1 | std::views::take(10);
	}
}
#pragma warning(pop)

