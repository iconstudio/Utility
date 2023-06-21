export module Utility.Coroutine;
export import <coroutine>;
export import Utility.Traits;
import Utility.Constraints;

export namespace util
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

	template<typename T>
	concept awaitable = requires(T t)
	{
		t.await_ready();
		t.await_suspend();
		t.await_resume();
	};
}
