module;
#include <ranges>
#include <vector>
#include <stack>
#include <coroutine>

export module Utility.Coroutine.Enumerator;
import Utility;
import Utility.Constraints;
import Utility.Monad;
import Utility.Coroutine;

namespace util::coroutine
{
	export template<typename Rng>
		concept enumerable = std::ranges::forward_range<Rng> && notvoids<std::ranges::range_value_t<Rng>>;

	export template<enumerable Rng, typename Ref>
		class [[nodiscard]] Enumerator
		: public std::ranges::view_interface<Enumerator<Rng, Ref>>
	{
	public:
		using value_type = std::ranges::range_value_t<Rng>;
		using reference = std::ranges::range_reference_t<Rng>;
		using const_reference = std::ranges::range_reference_t<const Rng>;
		using rvalue_reference = std::ranges::range_rvalue_reference_t<Rng>;
		using const_rvalue_reference = std::ranges::range_rvalue_reference_t<const Rng>;
		using size_type = size_t;
		using difference_type = ptrdiff_t;

		using internal_iterator = std::ranges::iterator_t<Rng>;
		using internal_const_iterator = std::ranges::const_iterator_t<Rng>;
		using type = Enumerator<Rng, Ref>;
		using interface = std::ranges::view_interface<Enumerator<Rng, Ref>>;

		class promise_type : public BasicPromise<Enumerator<Rng>::promise_type>
		{
		public:
			using coro_t = Enumerator<Rng>;
			using handle_type = typename coro_t::handle_type;
			using iterator = typename coro_t::internal_iterator;

			static constexpr std::suspend_always initial_suspend() noexcept
			{
				return {};
			}

			static constexpr std::suspend_always final_suspend() noexcept
			{
				return {};
			}

			//constexpr std::suspend_always yield_value(iterator it) noexcept
			constexpr std::suspend_always yield_value(const coro_t::value_type& value)
				noexcept(nothrow_copy_assignables<coro_t::value_type>)
				requires(copy_assignables<coro_t::value_type>)
			{
				//underlyingIter = it;
				current = value;

				return {};
			}

			constexpr std::suspend_always yield_value(coro_t::value_type&& value)
				noexcept(nothrow_move_assignables<coro_t::value_type>)
				requires(move_assignables<coro_t::value_type>)
			{
				current = move(value);

				return {};
			}

			coro_t::value_type& value() & noexcept
			{
				return current;
			}

			[[nodiscard]]
			const coro_t::value_type& value() const& noexcept
			{
				return current;
			}

			[[nodiscard]]
			coro_t::value_type&& value() && noexcept
			{
				return move(current);
			}

			[[nodiscard]]
			const coro_t::value_type&& value() const&& noexcept
			{
				return move(current);
			}

			[[nodiscard]]
			coro_t acquire_coroutine() noexcept
			{
				return coro_t{ handle_type::from_promise(*this) };
			}

			//iterator underlyingIter;
			coro_t::value_type current;
		};
		using handle_type = std::coroutine_handle<promise_type>;

		using iterator = CoIterator<Enumerator<Rng, Ref>>;
		using const_iterator = ConstCoIterator<Enumerator<Rng, Ref>>;
		//class iterator;
		//using const_iterator = const iterator;

		constexpr Enumerator()
			noexcept(nothrow_default_constructibles<Rng>)
			requires(default_initializable<Rng>) = default;
		constexpr ~Enumerator()
			noexcept(nothrow_destructibles<Rng>) = default;

		template<enumerable Sng, typename Sref>
			requires constructible_from<Rng, Sng>
		constexpr Enumerator(Enumerator<Sng, Sref>&& other)
			noexcept(nothrow_constructibles<Rng, Sng>)
			: myHandle(move(other.myHandle))
		{
			myHandle.promise().current = move(other.myHandle.promise().current);
		}

		template<enumerable Sng, typename Sref>
			requires constructible_from<Rng, Sng>&& copy_constructibles<Rng, Sng>
		constexpr Enumerator(const Enumerator<Sng, Sref>& other)
			noexcept(nothrow_constructibles<Rng, Sng>)
			: myHandle(other.myHandle)
		{
			myHandle.promise().current = other.myHandle.promise().current;
		}

		explicit constexpr Enumerator(const handle_type& handle)
			noexcept(nothrow_copy_constructibles<handle_type>)
			requires copy_constructibles<Rng, handle_type>
		: myHandle(handle)
		{}

		explicit constexpr Enumerator(handle_type&& handle)
			noexcept(nothrow_move_constructibles<handle_type>)
			requires move_constructibles<Rng, handle_type>
		: myHandle(move(handle))
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
		constexpr std::default_sentinel_t end() noexcept
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
		constexpr std::default_sentinel_t end() const noexcept
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

		Enumerator(const Enumerator& other) = delete;
		constexpr Enumerator(Enumerator&& other)
			noexcept(nothrow_move_constructibles<Rng>) = default;
		Enumerator& operator=(const Enumerator& other) = delete;
		constexpr Enumerator& operator=(Enumerator&& other)
			noexcept(nothrow_move_assignables<Rng>) = default;

	private:
		handle_type myHandle;
	};

	template<std::ranges::forward_range Rng>
	Enumerator(Rng&&) -> Enumerator<Rng, Rng&&>;
}

export template<typename Rng, typename Ref>
inline constexpr bool std::ranges::enable_borrowed_range<util::coroutine::Enumerator<Rng, Ref>> = std::ranges::enable_borrowed_range<Rng>;

export namespace util
{
	template<std::ranges::forward_range Rng>
	inline coroutine::Enumerator<Rng, Rng&&> coenumerate(Rng&& rng) noexcept
	{
		auto&& range = forward<Rng>(rng);
		auto it = std::ranges::begin(range);

		while (it != range.end())
		{
			co_yield (it++);
		}
	}
}

#pragma warning(push, 1)

namespace util::test
{
	void test_enum_coroutine()
	{
		auto aa = coenumerate(std::vector{ 0, 2, 34, 54, 56, 654, 75 });

		for (auto&& val : aa)
		{
		}

		std::vector vb{ 0, 2, 34, 54, 56, 654, 75 };
		auto bb = coenumerate(vb);

		for (auto&& val : vb)
		{
		}
	}
}
#pragma warning(pop)
