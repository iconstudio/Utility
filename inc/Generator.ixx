export module Utility.Coroutine.Generator;
import <tuple>;
import Utility.Constraints;
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
		using promise_type = DeferredValuePromise<type, value_type>;
		using handle_type = promise_type::handle_type;

		using iterator = CoConstIterator<type>;
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

namespace util
{
	struct cogenerate_fn
	{
		template<movable T>
		inline coroutine::Generator<T>
			operator()(T first)
			const
			noexcept(nothrow_incrementable<T>&& nothrow_copy_constructibles<T>&& nothrow_move_constructibles<T>)
		{
			while (true)
			{
				co_yield first++;
			}
		}

		template<movable T, std::equality_comparable_with<T> Last>
			requires movable<Last>
		inline coroutine::Generator<T>
			operator()(T first, const Last& last)
			const
			noexcept(nothrow_incrementable<T>&& nothrow_constructibles<T, T&&>&& nothrow_copy_constructibles<T>)
		{
			while (last != first)
			{
				co_yield first++;
			}
		}

		template<typename Fn, typename... Args>
			requires invocables<Fn, Args...>
		inline auto
			operator()(Fn&& fn, Args&&... args)
			const
			noexcept(nothrow_invocables<Fn, Args...>)
			-> coroutine::Generator<invoke_result_t<Fn, Args...>>
		{
			auto&& functor = forward<Fn>(fn);

			if constexpr (0 < sizeof...(Args))
			{
				const std::tuple<Args&&...> arguments = std::forward_as_tuple(forward<Args>(args)...);
				do
				{
					co_yield std::apply(functor, arguments);
				}
				while (true);
			}
			else
			{
				do
				{
					co_yield functor();
				}
				while (true);
			}
		}
	};

	export extern "C" inline constexpr cogenerate_fn cogenerate{};
}

export template<typename T>
inline constexpr bool std::ranges::enable_borrowed_range<util::coroutine::Generator<T>> = true;

#pragma warning(push, 1)
namespace util::test
{
	void test_gen_coroutine()
	{
		const coroutine::Generator aa = cogenerate(1);

		for (auto&& val : aa)
		{
		}

		int first = 40;
		for (auto&& val : cogenerate([&]() noexcept -> int { return first++; }))
		{
		}
	}
}
#pragma warning(pop)
