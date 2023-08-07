export module Utility.Promise;
import Utility;
import Utility.Constraints;

export namespace util
{
	template<typename Fn, typename U>
	struct noexcept_t
	{
		template<typename... Args>
		static consteval bool Eval() noexcept
		{
			if constexpr (!same_as<U, void>)
			{
				return nothrow_invocables<Fn>;
			}
			else
			{
				return nothrow_invocables<Fn, Args...>;
			}
		}

		template<typename... Args>
			requires (same_as<U, void>)
		static constexpr auto Execute(Fn&& functor, [[maybe_unused]] Args...) noexcept(noexcept(forward<Fn>(functor)()))
		{
			return forward<Fn>(functor)();
		}

		template<typename... Args>
			requires (!same_as<U, void>)
		static constexpr auto Execute(Fn&& functor, Args&&... args) noexcept(noexcept(forward<Fn>(functor)(forward<Args>(args)...)))
		{
			return forward<Fn>(functor)(forward<Args>(args)...);
		}
	};

	/// <summary>
	/// 
	/// </summary>
	/// <typeparam name="T">Sucess</typeparam>
	/// <typeparam name="E">Error</typeparam>
	/// <typeparam name="C">Cause of Defer</typeparam>
	template<movable T>
	class Promise
	{

	};
}

#pragma warning(push, 1)
namespace test
{
#if false
	void test_promise() noexcept
	{
		constexpr auto fnl0 = [](const int& v) -> int {
			return 300;
		};

		constexpr auto fnr0 = [](int&&) -> int {
			return 300;
		};

		Promise<int> vpromise0{};
		const auto r0 = vpromise0 >> fnl0;
		Promise<long long> vpromise1{};

		constexpr Promise<int, void> cvpromise0{};

		constexpr Promise<long long, void> cvpromise1{};

		constexpr Proxy proxy0{};
	}
#endif // false
}
#pragma warning(pop)
