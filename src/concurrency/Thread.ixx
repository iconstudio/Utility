module;
#include <utility>
#include <chrono>
#include <thread>
#include <stop_token>
#include <string_view>
#include <format>
export module Utility.Concurrency.Thread;

export extern "C++" namespace util
{
	extern "C++" namespace this_thread
	{
		using ::std::chrono::nanoseconds;
		using ::std::chrono::microseconds;
		using ::std::chrono::milliseconds;
		using ::std::chrono::seconds;
		using ::std::chrono::minutes;
		using ::std::chrono::hours;
		using ::std::chrono::nanoseconds;
		using ::std::chrono::nanoseconds;

		using ::std::this_thread::yield;
		using ::std::this_thread::get_id;
		using ::std::this_thread::sleep_until;
		using ::std::this_thread::sleep_for;
	}

	using ::std::operator<=>;
	using ::std::operator==;
	using ::std::swap;
	using ::std::invoke;
	using ::std::ref;
	using ::std::cref;
	using ::std::as_const;
	using ::std::unwrap_reference;
	using ::std::unwrap_reference_t;
	using ::std::decay;
	using ::std::decay_t;

	using ::std::reference_wrapper;
	using ::std::invoke_result;
	using ::std::invoke_result_t;
	using ::std::thread;
	using ::std::jthread;

	using CancellationSource = ::std::stop_source;
	using CancellationToken = ::std::stop_token;
	template<typename Fn>
	using CancellationCallback = ::std::stop_callback<Fn>;

	using ::std::stop_source;
	using ::std::stop_token;
	using ::std::stop_callback;
	using ::std::nostopstate_t;
	using ::std::nostopstate;

	using ::std::semiregular;
	using ::std::regular;
	using ::std::invocable;
	using ::std::regular_invocable;
	using ::std::predicate;
	using ::std::equivalence_relation;
	using ::std::strict_weak_order;

	template<typename Fn, typename... Args>
		requires invocable<Fn, decay_t<Args>...>
	[[nodiscard]]
	inline thread make_thread(Fn&& fn, Args&&... args)
	{
		return thread{ ::std::forward<Fn>(fn), ::std::forward<Args>(args)... };
	}

	template<typename Fn, typename... Args>
		requires invocable<Fn, decay_t<Args>...>
	[[nodiscard]]
	inline jthread make_jthread(Fn&& fn, Args&&... args)
	{
		return jthread{ ::std::forward<Fn>(fn), ::std::forward<Args>(args)... };
	}

	template<typename Fn, typename... Args>
		requires invocable<Fn, CancellationToken, decay_t<Args>...>
	[[nodiscard]]
	inline jthread make_jthread(Fn&& fn, Args&&... args)
	{
		return jthread{ ::std::forward<Fn>(fn), ::std::forward<Args>(args)... };
	}
}

export template<>
struct std::default_delete<std::jthread>
{
	constexpr default_delete() noexcept = default;
	constexpr ~default_delete() noexcept = default;

	template <std::convertible_to<std::jthread> U>
	constexpr default_delete(const default_delete<U>&) noexcept
	{}

	inline void operator()(std::jthread* const handle) const noexcept
	{
		try
		{
			handle->request_stop();
			handle->join();
		}
		catch (...)
		{}

		delete handle;
	}
};
