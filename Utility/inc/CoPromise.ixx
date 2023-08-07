export module Utility.Coroutine:Promise;
import <iterator>;
import <coroutine>;
import Utility;
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
		{ t.await_ready() } -> same_as<bool>;
		t.await_suspend(coroutine_handle<>{});
		t.await_resume();
	};

	template<typename Promise>
	class [[nodiscard]] BasicPromise
	{
	public:
		using handle_type = coroutine_handle<BasicPromise<Promise>>;

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
		using handle_type = coroutine_handle<type>;

		[[nodiscard]]
		Coroutine acquire_coroutine() noexcept
		{
			return Coroutine{ handle_type::from_promise(*this) };
		}
	};

	template<typename Coroutine
		, awaitable Init, awaitable Final
		, notvoids Value>
	class [[nodiscard]] PromiseTemplate<Coroutine, Init, Final, Value>
		: public BasicPromise<PromiseTemplate<Coroutine, Init, Final, Value>>
		, public Suspender<Init, Final>
	{
	public:
		using type = PromiseTemplate<Coroutine, Init, Final, Value>;
		using coro_type = Coroutine;
		using handle_type = coroutine_handle<type>;

		using value_type = Value;
		using reference = remove_reference_t<Value>&;
		using const_reference = const remove_reference_t<Value>&;
		using rvalue_reference = remove_reference_t<Value>&&;
		using const_rvalue_reference = const remove_reference_t<Value>&&;

		constexpr PromiseTemplate()
			noexcept(nothrow_default_constructibles<value_type>)
			requires default_initializable<Value&&>
		: myValue()
		{}

		constexpr PromiseTemplate()
			noexcept(nothrow_default_constructibles<value_type>)
			requires (!default_initializable<Value&&>)
		{}

		constexpr ~PromiseTemplate()
			noexcept(nothrow_destructibles<value_type>) = default;

		template<typename U>
		constexpr Final yield_value(U&& value)
			noexcept(nothrow_assignables<Value, U&&>)
		{
			if constexpr (!assignable_from<Value, U&&> && std::indirectly_writable<reference, U&&>)
			{
				*myValue = forward<U>(value);
			}
			else
			{
				myValue = forward<U>(value);
			}

			return {};
		}

		[[nodiscard]]
		constexpr reference value() & noexcept
			requires (!std::is_const_v<value_type>)
		{
			return myValue;
		}

		[[nodiscard]]
		constexpr const_reference value() const& noexcept
		{
			return myValue;
		}

		[[nodiscard]]
		constexpr rvalue_reference value() && noexcept
			requires (!std::is_const_v<value_type>)
		{
			return move(myValue);
		}

		[[nodiscard]]
		constexpr const_rvalue_reference value() const&& noexcept
		{
			return move(myValue);
		}

		[[nodiscard]]
		Coroutine acquire_coroutine() noexcept
		{
			return Coroutine{ handle_type::from_promise(*this) };
		}

		Value myValue;
	};

	template<classes Coroutine>
	using DeferredPromise = PromiseTemplate<Coroutine, std::suspend_always, std::suspend_always>;

	template<classes Coroutine>
	using RelaxedPromise = PromiseTemplate<Coroutine, std::suspend_never, std::suspend_always>;

	template<classes Coroutine, awaitable Init, awaitable Final, movable Value>
	using LvaluePromise = PromiseTemplate<Coroutine, Init, Final, Value&>;

	template<classes Coroutine, awaitable Init, awaitable Final, movable Value>
	using ValuePromise = PromiseTemplate<Coroutine, Init, Final, Value>;

	template<classes Coroutine, movable Value>
	using DeferredValuePromise = ValuePromise<Coroutine, std::suspend_always, std::suspend_always, Value>;

	template<classes Coroutine, movable Value>
	using RelaxedValuePromise = ValuePromise<Coroutine, std::suspend_never, std::suspend_always, Value>;
}
