export module Utility.Coroutine;
export import <coroutine>;
import Utility.Constraints;

export namespace util
{
	using namespace ::std;

	template<typename T, typename P = void>
	concept Awaitable = requires(T t)
	{
		{ t.await_ready() } -> convertible_to<bool>;
		{ t.await_suspend() } -> convertible_to<coroutine_handle<P>>;
		{ t.await_resume() } -> voids;
	};
}
