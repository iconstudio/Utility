export module Utility.Coroutine;
export import <algorithm>;
export import <stack>;
export import <ranges>;
export import <coroutine>;
export import Utility.Memory;
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
	using ::std::default_sentinel_t;

	template<typename T>
	concept awaitable = requires(T t)
	{
		t.await_ready();
		t.await_suspend();
		t.await_resume();
	};

	/// <summary>
	/// A Co-working Forward list
	/// (Pre-implement the C++23 standard class)
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <typeparam name="R"></typeparam>
	template<movable T, typename R = T&>
	class [[nodiscard]] generator : public std::ranges::view_interface<generator<T, R>>
	{
	public:
		using value_type = clean_t<T>;
		using reference = T&;
		using const_reference = const T&;
		using rvalue_reference = T&&;
		using const_rvalue_reference = const T&&;
		using yielded = R;

		class iterator;
		class promise_type;

		generator(generator&& other) noexcept;
		generator& operator=(generator&& other) noexcept;

		~generator() noexcept
		{
			if (myHandle.done())
			{
				myHandle.destroy();
			}
		}

		[[nodiscard]]
		iterator begin() noexcept
		{

		}

		[[nodiscard]]
		default_sentinel_t end() const noexcept
		{
			return {};
		}

		generator(const generator& other) = delete;
		generator& operator=(const generator& other) = delete;

	private:
		coroutine_handle<promise_type> myHandle = nullptr;
		unique_ptr<std::stack<coroutine_handle<>>> active_;
	};

	template<movable T, typename U>
	class [[nodiscard]] generator<T, U>::promise_type
	{
	public:
		using coro_t = generator<T, U>;
		using value_type = coro_t::yielded;

		generator get_return_object() noexcept;

		suspend_never initial_suspend() const noexcept
		{
			return {};
		}

		suspend_never final_suspend() noexcept
		{
			return {};
		}

		suspend_always yield_value(yielded val) noexcept;

		suspend_always yield_value(const remove_reference_t<yielded>& lval)
			requires std::is_rvalue_reference_v<yielded>&& constructible_from<remove_cvref_t<yielded>, const remove_reference_t<yielded>&>;

		void return_void() const noexcept {}

		void unhandled_exception();

		void await_transform() = delete;

	private:
		add_pointer_t<yielded> myValue = nullptr;
		std::exception_ptr lastError;
	};

	template<movable T, typename U>
	class [[nodiscard]] generator<T, U>::iterator
	{
	public:
		using coro_t = generator<T, U>;
		using value_type = coro_t::value_type;
		using reference = coro_t::reference;
		using const_reference = coro_t::const_reference;
		using rvalue_reference = coro_t::rvalue_reference;
		using rvalue_const_reference = coro_t::const_rvalue_reference;
		using difference_type = ptrdiff_t;

		[[nodiscard]]
		coro_t::reference operator*() const noexcept(nothrow_copy_constructibles<reference>)
		{
			return myCoroutine->promise().myValue;
		}

		iterator& operator++()
		{
			return *this;
		}

		void operator++(int)
		{

		}

		[[nodiscard]]
		friend bool operator==(iterator it, default_sentinel_t)
		{
			return true;
		}

		iterator(iterator&& other) noexcept = default;
		iterator& operator=(iterator&& other) noexcept = default;

	private:
		coro_t* myCoroutine = nullptr;
		coroutine_handle<promise_type> refHandle;
	};
}
