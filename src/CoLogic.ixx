export module Utility.Coroutine.Logic;
import <tuple>;
import <vector>;
import <algorithm>;
import <ranges>;
import Utility;
import Utility.Constraints;
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

		while (invoke(predicate))
		{
			invoke(functor);

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

		while (invoke(predicate))
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
			if (!invoke(functor))
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

	template<movable V>
	inline coroutine::Generator<V>
		coiota(V init)
		noexcept(nothrow_incrementable<V>&& nothrow_constructibles<V, V&&>)
	{
		while (true)
		{
			co_yield init++;
		}
	}

	/// <summary>
	/// For iterators
	/// </summary>
	template<movable V, std::forward_iterator It, typename Fn>
	inline auto
		coccumulate(V init, It it, Fn&& op)
		noexcept(nothrow_incrementable<It>&& nothrow_constructibles<V, V&&>&& nothrow_invocables<Fn, V, V>)
		-> coroutine::Generator<V>
	{
		Fn&& fn = forward<Fn>(op);

		while (true)
		{
			co_yield init;

			init = fn(move(init), *it++);
		}
	}

	/// <summary>
	/// For non-iterators
	/// </summary>
	template<movable T, typename Fn, typename... Args>
		requires invocables<Fn, T, Args...>
	inline coroutine::Generator<T>
		coccumulate(T first, Fn&& fn, Args&&... args)
		noexcept(nothrow_incrementable<T>&& nothrow_constructibles<T, T&&>&& nothrow_invocables<Fn, T, Args...>)
	{
		auto&& pred = forward<Fn>(fn);

		if constexpr (0 < sizeof...(Args))
		{
			std::tuple<T&, Args&&...> arguments = std::forward_as_tuple(first, forward<Args>(args)...);
			do
			{
				co_yield first;

				std::get<0>(arguments) = std::apply(pred, arguments);
			}
			while (true);
		}
		else
		{
			do
			{
				co_yield first;

				first = pred(first);
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

		auto io_range1 = cogenerate(0, 15);
		auto io_range2 = coccumulate(0, [](const int& curr) {
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

