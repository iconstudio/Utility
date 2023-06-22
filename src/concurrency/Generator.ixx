export module Utility.Coroutine.Generator;
import Utility.Coroutine;

export namespace util::coroutine
{
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

		using type = Generator<T>;
		using promise_type = ValuePromise<type>;
		using handle_type = promise_type::handle_type;

		using iterator = ConstCoIterator<type>;
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
		inline iterator begin() noexcept
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
		inline const_iterator begin() const noexcept
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
		inline const_iterator cbegin() const noexcept
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

		[[nodiscard]]
		bool done() const noexcept
		{
			return myHandle.done();
		}

		[[nodiscard]]
		bool empty() const noexcept
		{
			return !myHandle || myHandle.done();
		}

		[[nodiscard]]
		explicit operator bool() const noexcept
		{
			return !empty();
		}

		Generator(const Generator& other) = delete;
		constexpr Generator(Generator&& other) noexcept = default;
		Generator& operator=(const Generator& other) = delete;
		constexpr Generator& operator=(Generator&& other) noexcept = default;

	private:
		handle_type myHandle;
	};
}

export namespace util
{
	template<movable T, equality_comparable_with<T> Guard>
	inline coroutine::Generator<T> cogenerate(T first, const Guard last)
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
	void test_gen_coroutine()
	{
		const coroutine::Generator aa = cogenerate(1, 10);

		for (auto&& val : aa)
		{
		}
	}
}
#pragma warning(pop)