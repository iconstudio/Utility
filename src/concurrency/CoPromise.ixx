export module Utility.Coroutine:Promise;
import <iterator>;
import <coroutine>;
import Utility;
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
	concept awaitable = requires(T t)
	{
		t.await_ready();
		t.await_suspend(coroutine_handle<>{});
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

	template<movable Storage>
	class Borrower;

	template<std::indirectly_readable Storage>
	class Borrower<Storage>
	{
	public:
		constexpr Borrower()
			noexcept(nothrow_default_constructibles<Storage>)
			requires default_initializable<Storage> = default;
		constexpr ~Borrower()
			noexcept(nothrow_destructibles<Storage>) = default;

		template<std::indirectly_readable S>
			requires (constructible_from<Storage, S&&> || constructible_from<Storage, std::indirect_result_t<S&&>>)
		explicit(!is_trivially_constructible_v<Storage, S&&>)
			constexpr Borrower(S&& storage)
			noexcept(nothrow_constructibles<Storage, S&&>)
			: currentValue(static_cast<S&&>(storage))
		{}

		template<movable V>
			requires (constructible_from<Storage, V&&> && !std::indirectly_readable<V>)
		explicit(!is_trivially_constructible_v<Storage, V&&>)
			constexpr Borrower(V&& value)
			noexcept(nothrow_constructibles<Storage, V&&>)
			: currentValue(static_cast<V&&>(value))
		{}

		Storage& Get() &
			noexcept(nothrow_copy_constructibles<Storage>)
			requires(copy_constructible<Storage>)
		{ return currentValue; }

		const Storage& Get() const&
			noexcept(nothrow_copy_constructibles<Storage>)
			requires(copy_constructible<Storage>)
		{ return currentValue; }

		Storage&& Get() &&
			noexcept(nothrow_move_constructibles<Storage>)
			requires(move_constructible<Storage>)
		{ return move(currentValue); }

		const Storage&& Get() const&&
			noexcept(nothrow_move_constructibles<Storage>)
			requires(move_constructible<Storage>)
		{ return move(currentValue); }

	protected:
		Storage currentValue;
	};

	template<typename T>
	class Yielder
	{
	public:
		using value_type = conditional_t<std::is_lvalue_reference_v<T>, clean_t<T>&, clean_t<T>>;
		using reference = value_type&;
		using const_reference = const value_type&;
		using rvalue_reference = value_type&&;
		using const_rvalue_reference = const value_type&&;

		constexpr Yielder()
			noexcept(nothrow_default_constructibles<value_type>)
			requires default_initializable<value_type> = default;

		constexpr Yielder()
			requires (!default_initializable<value_type>)
		{}

		constexpr ~Yielder()
			noexcept(nothrow_destructibles<value_type>) = default;

		template<typename W>
			requires constructible_from<value_type, W&&>
		constexpr Yielder(W&& value)
			noexcept(nothrow_constructibles<value_type, W&&>)
			: current(forward<W>(value))
		{}

		constexpr Yielder& operator=(const_reference ref)
			noexcept(nothrow_copy_assignables<T>)
		{
			current = ref;
			return *this;
		}

		constexpr Yielder& operator=(rvalue_reference value)
			noexcept(nothrow_move_assignables<T>)
		{
			current = move(value);
			return *this;
		}

		[[nodiscard]]
		constexpr reference value() & noexcept
			requires (!std::is_const_v<T>)
		{
			return current;
		}

		[[nodiscard]]
		constexpr const_reference value() const& noexcept
		{
			return current;
		}

		[[nodiscard]]
		constexpr rvalue_reference value() && noexcept
			requires (!std::is_const_v<T>)
		{
			return move(current);
		}

		[[nodiscard]]
		constexpr const_rvalue_reference value() const&& noexcept
		{
			return move(current);
		}

		value_type current;
	};

	template<typename T>
	Yielder(T&&) -> Yielder<T&&>;

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
		, public Yielder<Value>
	{
	public:
		using type = PromiseTemplate<Coroutine, Init, Final, Value>;
		using coro_type = Coroutine;
		using handle_type = std::coroutine_handle<type>;

		using value_type = Yielder<Value>;
		using reference = remove_reference_t<Value>&;
		using const_reference = const remove_reference_t<Value>&;
		using rvalue_reference = remove_reference_t<Value>&&;
		using const_rvalue_reference = const remove_reference_t<Value>&&;

		constexpr PromiseTemplate()
			noexcept(nothrow_default_constructibles<value_type>)
			requires default_initializable<Value&&>
		: value_type()
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
			Yielder<Value>::operator=(static_cast<U&&>(value));
			return {};
		}

		[[nodiscard]]
		Coroutine acquire_coroutine() noexcept
		{
			return Coroutine{ handle_type::from_promise(*this) };
		}
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
