export module Utility.Coroutine;
export import <ranges>;
export import <coroutine>;
export import Utility.Traits;
import Utility.Constraints;
import Utility.Monad;

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

	template<movable T>
	class [[nodiscard]] Generator
	{
	public:
		using value_type = clean_t<T>;
		using reference = T&;
		using const_reference = const T&;
		using rvalue_reference = T&&;
		using const_rvalue_reference = const T&&;

		class promise_type;
		using handle_type = std::coroutine_handle<promise_type>;

		inline Generator() noexcept = default;
		inline ~Generator() noexcept
		{
			if (myHandle.done())
			{
				myHandle.destroy();
			}
		}

		explicit constexpr Generator(const handle_type& coroutine)
			noexcept(nothrow_copy_constructibles<handle_type>)
			: myHandle(coroutine)
		{}

		explicit constexpr Generator(handle_type&& coroutine)
			noexcept(nothrow_move_constructibles<handle_type>)
			: myHandle(move(coroutine))
		{}

		class CoIterator
		{
		public:
			using coro_type = Generator<T>;
			using handle_type = coro_type::handle_type;
			friend class coro_type::promise_type;

			using iterator_category = std::forward_iterator_tag;
			using difference_type = ptrdiff_t;
			using value_type = coro_type::value_type;
			using reference = coro_type::reference;
			using const_reference = coro_type::const_reference;
			using rvalue_reference = coro_type::rvalue_reference;
			using const_rvalue_reference = coro_type::const_rvalue_reference;

			explicit CoIterator(const handle_type& coroutine) noexcept
				: coHandle(coroutine)
			{}

			explicit CoIterator(handle_type&& coroutine) noexcept
				: coHandle(move(coroutine))
			{}

			inline CoIterator& operator++()
			{
				if (!coHandle.done())
				{
					coHandle.resume();
				}

				return *this;
			}

			inline void operator++(int)
			{
				if (!coHandle.done())
				{
					coHandle.resume();
				}
			}

			[[nodiscard]]
			inline const_reference operator*() const&
			{
				return coHandle.promise().return_value();
			}

			[[nodiscard]]
			inline const_rvalue_reference operator*() const&&
			{
				return move(coHandle.promise()).return_value();
			}

			[[nodiscard]]
			inline bool operator==(default_sentinel_t) const
			{
				return !coHandle || coHandle.done();
			}

		private:
			handle_type coHandle;
		};

		[[nodiscard]]
		CoIterator begin() noexcept
		{
			if (!myHandle.done())
			{
				myHandle.resume();
			}

			return CoIterator{ myHandle };
		}

		[[nodiscard]]
		default_sentinel_t end() noexcept
		{
			return {};
		}

		Generator(const Generator& other) = delete;
		Generator(Generator&& other) noexcept = default;
		Generator& operator=(const Generator& other) = delete;
		Generator& operator=(Generator&& other) noexcept = default;

	private:
		handle_type myHandle;
	};

	template<movable T>
	class [[nodiscard]] Generator<T>::promise_type
	{
	public:
		constexpr promise_type()
			noexcept(nothrow_default_constructibles<T>)
			requires default_initializables<T> = default;
		constexpr ~promise_type() noexcept(nothrow_destructibles<T>) = default;

		[[nodiscard]]
		Generator<T> get_return_object()
		{
			return Generator{ handle_type::from_promise(*this) };
		}

		static suspend_always initial_suspend() noexcept
		{
			return {};
		}

		static suspend_always final_suspend() noexcept
		{
			return {};
		}

		suspend_always yield_value(rvalue_reference value)
			noexcept(nothrow_move_constructibles<T>)
		{
			currentValue = move(value);
			return {};
		}

		suspend_always yield_value(const_rvalue_reference value)
			noexcept(nothrow_move_constructibles<const T>)
		{
			currentValue = move(value);
			return {};
		}

		suspend_always yield_value(const_reference value)
			noexcept(nothrow_copy_constructibles<T>)
			requires move_constructibles<T>
		{
			currentValue = value;
			return {};
		}

		reference return_value() &
			noexcept
		{
			return *currentValue;
		}

		const_reference return_value() const&
			noexcept
		{
			return *currentValue;
		}

		rvalue_reference return_value() &&
			noexcept(nothrow_move_constructibles<T>)
		{
			return *move(currentValue);
		}

		const_rvalue_reference return_value() const&&
			noexcept(nothrow_move_constructibles<T>)
		{
			return *move(currentValue);
		}

		// Disallow co_await
		void await_transform() = delete;

		[[noreturn]]
		static void unhandled_exception()
		{
			throw;
		}

	private:
		Monad<T> currentValue;
	};

	template<movable T, typename Sentinel>
	Generator<T> range(T first, const Sentinel last)
		noexcept(nothrow_copy_constructibles<T>)
	{
		while (first < last)
		{
			co_yield first++;
		}
	}
}
