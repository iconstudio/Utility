export module Utility.Coroutine:Promise;
import <coroutine>;
import Utility.Constraints;
import Utility.Monad;

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
	concept awaitable = default_initializable<T> && requires(T t)
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

		auto get_return_object() noexcept(noexcept(_Cast()->acquire_coroutine()))
		{
			return _Cast()->acquire_coroutine();
		}

		[[noreturn]]
		static constexpr void return_void() noexcept {}

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

	template<awaitable Init, awaitable Final>
	class Suspender
	{
	public:
		static constexpr std::decay_t<Init> initial_suspend() noexcept
		{
			return {};
		}

		static constexpr std::decay_t<Final> final_suspend() noexcept
		{
			return {};
		}
	};

	template<typename Coroutine
		, awaitable Init = std::suspend_always, awaitable Final = std::suspend_always
		, typename V = void>
	class [[nodiscard]] PromiseTemplate;

	template<typename Coroutine, awaitable Init, awaitable Final>
	class [[nodiscard]] PromiseTemplate<Coroutine, Init, Final, void>
		: public BasicPromise<PromiseTemplate<Coroutine, Init, Final, void>>
		, public Suspender<Init, Final>
	{
	public:
		using type = PromiseTemplate<Coroutine, Init, Final, void>;
		using coro_type = Coroutine;
		using handle_type = std::coroutine_handle<type>;

		// Disallow co_await
		void await_transform() = delete;
	};

	template<typename Coroutine, awaitable Init, awaitable Final, notvoids Value>
	class [[nodiscard]] PromiseTemplate<Coroutine, Init, Final, Value>
		: public BasicPromise<PromiseTemplate<Coroutine, Init, Final, Value>>
		, public Suspender<Init, Final>
	{
	public:
		using type = PromiseTemplate<Coroutine, Init, Final, Value>;
		using coro_type = Coroutine;
		using handle_type = std::coroutine_handle<type>;

		using value_type = clean_t<Value>;
		using reference = value_type&;
		using const_reference = const value_type&;
		using rvalue_reference = value_type&&;
		using const_rvalue_reference = const value_type&&;

		constexpr PromiseTemplate() noexcept = default;
		constexpr ~PromiseTemplate() noexcept = default;

		Final yield_value(rvalue_reference value)
			noexcept(nothrow_move_constructibles<value_type>)
		{
			currentValue = static_cast<rvalue_reference>(value);
			return {};
		}

		Final yield_value(const_rvalue_reference value)
			noexcept(nothrow_move_constructibles<const value_type>)
			requires move_constructibles<const value_type>
		{
			currentValue = static_cast<const_rvalue_reference>(value);
			return {};
		}

		Final yield_value(const_reference value)
			noexcept(nothrow_copy_constructibles<value_type>)
			requires move_constructibles<value_type>
		{
			currentValue = value;
			return {};
		}

		Monad<value_type> currentValue;
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
