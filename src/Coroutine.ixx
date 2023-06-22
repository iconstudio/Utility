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
		using size_type = size_t;
		using difference_type = ptrdiff_t;

		class promise_type;
		using handle_type = std::coroutine_handle<promise_type>;

		class iterator
		{
		public:
			using coro_type = Generator<T>;
			using handle_type = coro_type::handle_type;

			using iterator_category = std::forward_iterator_tag;
			using value_type = coro_type::value_type;
			using reference = coro_type::reference;
			using const_reference = coro_type::const_reference;
			using rvalue_reference = coro_type::rvalue_reference;
			using const_rvalue_reference = coro_type::const_rvalue_reference;
			using size_type = coro_type::size_type;
			using difference_type = coro_type::difference_type;

			constexpr iterator()
				noexcept(nothrow_default_constructibles<handle_type>)
				requires default_initializables<handle_type> = default;
			constexpr ~iterator() noexcept(nothrow_destructibles<handle_type>) = default;

			explicit constexpr iterator(const handle_type& coroutine) noexcept
				: coHandle(coroutine)
			{}

			explicit constexpr iterator(handle_type&& coroutine) noexcept
				: coHandle(move(coroutine))
			{}

			inline iterator& operator++()
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

			inline reference operator*()&
			{
				return *(coHandle.promise().currentValue);
			}

			inline const_reference operator*() const&
			{
				return *(coHandle.promise().currentValue);
			}

			inline rvalue_reference operator*() &&
				noexcept(nothrow_move_constructibles<value_type>)
			{
				return *move(coHandle.promise().currentValue);
			}

			inline const_rvalue_reference operator*() const&&
				noexcept(nothrow_move_constructibles<value_type>)
			{
				return *move(coHandle.promise().currentValue);
			}

			[[nodiscard]]
			inline bool operator==(default_sentinel_t) const
			{
				return !coHandle || coHandle.done();
			}

		private:
			handle_type coHandle;
		};
		using const_iterator = iterator;

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

		[[nodiscard]]
		iterator begin() noexcept
		{
			if (!myHandle.done())
			{
				myHandle.resume();
			}

			return iterator{ myHandle };
		}

		[[nodiscard]]
		constexpr default_sentinel_t end() noexcept
		{
			return {};
		}

		[[nodiscard]]
		const_iterator begin() const noexcept
		{
			if (!myHandle.done())
			{
				myHandle.resume();
			}

			return const_iterator{ myHandle };
		}

		[[nodiscard]]
		constexpr default_sentinel_t end() const noexcept
		{
			return {};
		}

		[[nodiscard]]
		const_iterator cbegin() const noexcept
		{
			if (!myHandle.done())
			{
				myHandle.resume();
			}

			return const_iterator{ myHandle };
		}

		[[nodiscard]]
		constexpr default_sentinel_t cend() const noexcept
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
		Generator<T> get_return_object() noexcept
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
			noexcept(nothrow_move_constructibles<const value_type>)
		{
			currentValue = move(value);
			return {};
		}

		suspend_always yield_value(const_reference value)
			noexcept(nothrow_copy_constructibles<value_type>)
			requires move_constructibles<value_type>
		{
			currentValue = value;
			return {};
		}

		void return_void() const noexcept {}

		// Disallow co_await
		void await_transform() = delete;

		[[noreturn]]
		static void unhandled_exception()
		{
			throw;
		}

		Monad<T> currentValue;
	};

	template<movable T, typename Sentinel>
	Generator<T> range(T first, const Sentinel last)
		noexcept(nothrow_copy_constructibles<T>)
	{
		while (first != last)
		{
			co_yield first++;
		}
	}
}

#pragma warning(push, 1)
namespace util::test
{
	void test_coroutines()
	{
		for (auto&& val : range('A', 'Z' + 1))
		{

		}
	}
}
#pragma warning(pop)
