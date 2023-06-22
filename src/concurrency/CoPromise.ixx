export module Utility.Coroutine:Promise;
import <coroutine>;

export namespace util::coroutine
{
	enum class [[nodiscard]] coexecution
	{
		Now, Later
	};

	class [[nodiscard]] BasicPromise
	{
	public:
		constexpr BasicPromise() noexcept = default;
		constexpr ~BasicPromise() noexcept = default;

		static std::suspend_always initial_suspend() noexcept
		{
			return {};
		}

		static std::suspend_always final_suspend() noexcept
		{
			return {};
		}

		[[noreturn]]
		static void unhandled_exception()
		{
			throw;
		}
	};

	template<typename Coroutine>
	class [[nodiscard]] DeferredPromise : public BasicPromise
	{
	public:
		constexpr DeferredPromise() noexcept = default;
		constexpr ~DeferredPromise() noexcept = default;

		Coroutine get_return_object()
		{
			return Coroutine{ std::coroutine_handle<DeferredPromise>::from_promise(*this) };
		}

		[[noreturn]]
		void return_void() const noexcept {}
	};

	template<typename Coroutine>
	class [[nodiscard]] RelaxedPromise : public BasicPromise
	{
	public:
		constexpr RelaxedPromise() noexcept = default;
		constexpr ~RelaxedPromise() noexcept = default;

		Coroutine get_return_object()
		{
			return Coroutine{ std::coroutine_handle<RelaxedPromise>::from_promise(*this) };
		}

		[[noreturn]]
		void return_void() const noexcept {}
	};

	template<typename Coroutine>
	using defer = DeferredPromise<Coroutine>;

	template<typename Coroutine>
	using relax = RelaxedPromise<Coroutine>;
}
