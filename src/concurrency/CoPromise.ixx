export module Utility.Coroutine:Promise;
import <coroutine>;
import Utility.Constraints;

export namespace util::coroutine
{
	using ::std::coroutine_handle;
	using ::std::suspend_never;
	using ::std::suspend_always;

	enum class [[nodiscard]] coexecution
	{
		Now, Later
	};

	template<typename T>
	concept awaitable = requires(T t)
	{
		t.await_ready();
		t.await_suspend();
		t.await_resume();
	};

	template<typename Promise>
	class [[nodiscard]] BasicPromise
	{
	public:
		using handle_type = std::coroutine_handle<BasicPromise<Promise>>;

		constexpr BasicPromise() noexcept = default;
		constexpr ~BasicPromise() noexcept = default;

		auto get_return_object() noexcept
		{
			return _Cast()->acquire_coroutine();
		}

		[[noreturn]]
		static void unhandled_exception()
		{
			throw;
		}

	protected:
		[[nodiscard]]
		constexpr Promise* _Cast() noexcept
		{
			return static_cast<Promise*>(this);
		}

		[[nodiscard]]
		constexpr const Promise* _Cast() const noexcept
		{
			return static_cast<const Promise*>(this);
		}
	};

	template<typename Coroutine, awaitable Init, awaitable Final, typename V = void>
	class [[nodiscard]] PromiseTemplate;

	template<typename Coroutine, awaitable Init, awaitable Final, notvoids Value>
	class [[nodiscard]] PromiseTemplate<Coroutine, Init, Final, Value>
		: public BasicPromise<PromiseTemplate<Coroutine, Init, Final, Value>>
	{
	public:
		using type = PromiseTemplate<Coroutine, Init, Final, Value>;
		using coro_type = Coroutine;
		using handle_type = std::coroutine_handle<type>;

		constexpr PromiseTemplate() noexcept = default;
		constexpr ~PromiseTemplate() noexcept = default;

		static constexpr Init initial_suspend() noexcept
		{
			return {};
		}

		static constexpr Final final_suspend() noexcept
		{
			return {};
		}

	};

	template<typename Coroutine, awaitable Init, awaitable Final>
	class [[nodiscard]] PromiseTemplate<Coroutine, Init, Final, void>
		: public BasicPromise<PromiseTemplate<Coroutine, Init, Final, void>>
	{
	public:
		using type = PromiseTemplate<Coroutine, Init, Final, void>;
		using coro_type = Coroutine;
		using handle_type = std::coroutine_handle<type>;
	};

	template<typename Coroutine>
	class [[nodiscard]] DeferredPromise : public BasicPromise<DeferredPromise<Coroutine>>
	{
	public:
		using coro_type = Coroutine;
		using handle_type = std::coroutine_handle<DeferredPromise<Coroutine>>;

		constexpr DeferredPromise() noexcept = default;
		constexpr ~DeferredPromise() noexcept = default;

		static std::suspend_always initial_suspend() noexcept
		{
			return {};
		}

		static std::suspend_always final_suspend() noexcept
		{
			return {};
		}

		[[noreturn]]
		void return_void() const noexcept {}

		[[nodiscard]]
		Coroutine acquire_coroutine() noexcept
		{
			return Coroutine{ handle_type::from_promise(*this) };
		}
	};

	template<typename Coroutine>
	class [[nodiscard]] RelaxedPromise : public BasicPromise<RelaxedPromise<Coroutine>>
	{
	public:
		using coro_type = Coroutine;
		using handle_type = std::coroutine_handle<RelaxedPromise<Coroutine>>;

		constexpr RelaxedPromise() noexcept = default;
		constexpr ~RelaxedPromise() noexcept = default;

		static std::suspend_never initial_suspend() noexcept
		{
			return {};
		}

		static std::suspend_always final_suspend() noexcept
		{
			return {};
		}

		[[noreturn]]
		void return_void() const noexcept {}

		[[nodiscard]]
		Coroutine acquire_coroutine() noexcept
		{
			return Coroutine{ handle_type::from_promise(*this) };
		}
	};

	template<typename Coroutine>
	using defer = DeferredPromise<Coroutine>;

	template<typename Coroutine>
	using relax = RelaxedPromise<Coroutine>;
}
