export module Utility.Coroutine.Enumerator;
import <algorithm>;
import <ranges>;
import <vector>;
import <coroutine>;
import Utility;
import Utility.Constraints;
import Utility.Monad;
import Utility.Coroutine;

namespace util::coroutine
{
	export template<typename Rng>
		concept enumerable = std::ranges::forward_range<Rng>;

	export template<enumerable Rng, typename Ref>
		class [[nodiscard]] Enumerator
		: public std::ranges::view_interface<Enumerator<Rng, Ref>>
	{
	public:
		static constexpr bool IsConstant = std::is_const_v<remove_reference_t<Ref>>;
		static constexpr bool IsBorrowed = std::is_lvalue_reference_v<Ref&&>;
		static constexpr bool IsTemporary = std::is_rvalue_reference_v<Ref&&>;

		static constexpr bool IsLReference = IsBorrowed && !IsConstant;
		static constexpr bool IsConstLReference = IsBorrowed && IsConstant;
		static constexpr bool IsRReference = IsTemporary && !IsConstant;
		static constexpr bool IsConstRReference = IsTemporary && IsConstant;

		using value_type = conditional_t<IsBorrowed
			, std::ranges::range_reference_t<Rng>
			, std::ranges::range_value_t<Rng>>;

		using reference = conditional_t<IsTemporary, std::ranges::range_rvalue_reference_t<Rng>, std::ranges::range_reference_t<Rng>>;
		using const_reference = conditional_t<IsTemporary, std::ranges::range_rvalue_reference_t<const Rng>, std::ranges::range_reference_t<const Rng>>;

		using rvalue_reference = std::ranges::range_rvalue_reference_t<Rng>;
		using const_rvalue_reference = std::ranges::range_rvalue_reference_t<const Rng>;
		using size_type = size_t;
		using difference_type = ptrdiff_t;

		using iterator = CoForwardIterator<Enumerator<Rng, Ref>>;
		using const_iterator = CoConstIterator<Enumerator<Rng, Ref>>;

		using internal_iterator = std::ranges::iterator_t<Rng>;
		using internal_const_iterator = std::ranges::const_iterator_t<Rng>;
		using type = Enumerator<Rng, Ref>;
		using interface = std::ranges::view_interface<Enumerator<Rng, Ref>>;

		class promise_type;
		using handle_type = std::coroutine_handle<promise_type>;

		class promise_type
			: public BasicPromise<promise_type>
			, public Suspender<std::suspend_always, std::suspend_always>
		{
		public:
			using coro_t = Enumerator<Rng, Ref>;

			template<typename U>
			constexpr std::suspend_always yield_value(U&& it)
				noexcept(nothrow_assignables<internal_iterator, U&&>)
			{
				static_assert(!assignable_from<internal_iterator, U&&>, "Cannot assign value to iterator");

				myValue = forward<U>(it);

				return {};
			}

			[[nodiscard]]
			constexpr reference value() & noexcept
				requires (!IsConstant)
			{
				if constexpr (IsTemporary)
				{
					return static_cast<reference>(*myValue);
				}
				else
				{
					return *myValue;
				}
			}

			[[nodiscard]]
			constexpr const_reference value() const& noexcept
			{
				if constexpr (IsTemporary)
				{
					return static_cast<const_reference>(*myValue);
				}
				else
				{
					return *myValue;
				}
			}

			[[nodiscard]]
			coro_t acquire_coroutine() noexcept
			{
				return coro_t{ handle_type::from_promise(*this) };
			}

			internal_iterator myValue;
		};

		constexpr Enumerator()
			noexcept(nothrow_default_constructibles<handle_type, Rng>)
			requires(default_initializable<handle_type>) = default;
		constexpr ~Enumerator()
			noexcept(nothrow_destructibles<handle_type>) = default;

		template<enumerable Sng, typename Sref>
			requires constructible_from<Rng, Sng>
		explicit constexpr Enumerator(Enumerator<Sng, Sref>&& other)
			noexcept(nothrow_constructibles<Rng, Sng>)
			: myHandle(other.myHandle)
		{
			myHandle.promise().value() = move(other.myHandle.promise().value());
		}

		template<enumerable Sng, typename Sref>
			requires constructible_from<Rng, Sng>&& copy_constructibles<Rng, Sng>
		explicit constexpr Enumerator(const Enumerator<Sng, Sref>& other)
			noexcept(nothrow_constructibles<Rng, Sng>)
			: myHandle(other.myHandle)
		{
			myHandle.promise().value() = other.myHandle.promise().value();
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
		const_iterator cbegin() const noexcept
		{
			if (!myHandle.done())
			{
				myHandle.resume();
			}

			return const_iterator{ myHandle };
		}

		[[nodiscard]]
		constexpr std::default_sentinel_t cend() const noexcept
		{
			return {};
		}

		[[nodiscard]]
		bool done() const noexcept
		{
			return myHandle.done();
		}

		Enumerator(const Enumerator& other) = delete;
		constexpr Enumerator(Enumerator&& other)
			noexcept(nothrow_move_constructibles<handle_type, Rng>) = default;
		Enumerator& operator=(const Enumerator& other) = delete;
		constexpr Enumerator& operator=(Enumerator&& other)
			noexcept(nothrow_move_assignables<handle_type, Rng>) = default;

	private:
		handle_type myHandle;
	};

	template<std::ranges::forward_range Rng>
	Enumerator(Rng&&) -> Enumerator<Rng, Rng&&>;
}

export template<typename Rng, typename Ref>
inline constexpr bool std::ranges::enable_borrowed_range<util::coroutine::Enumerator<Rng, Ref>> = std::ranges::enable_borrowed_range<Rng>;

namespace util
{
	export using coroutine::enumerable;

	struct coenumerate_fn
	{
		template<enumerable Rng>
			inline auto
			operator()(Rng && rng)
			const
			noexcept(nothrow_incrementable<std::ranges::iterator_t<Rng>> && nothrow_assignables<Rng&&, Rng>)
			-> coroutine::Enumerator<Rng, Rng&&>
		{
			auto&& range = forward<Rng>(rng);
			auto it = range.begin();

			static_assert(std::forward_iterator<decltype(it)>);

			while (it != range.end())
			{
				co_yield it;
				it++;
			}
		}

		template<std::forward_iterator It, std::sentinel_for<It> End>
			inline auto
			operator()(It it, const End & end)
			const
			noexcept(nothrow_incrementable<It>)
		{
			return (*this)(std::ranges::subrange{ it, end });
		}
	};

	export extern "C" inline constexpr coenumerate_fn coenumerate{};
}

#pragma warning(push, 1)
namespace util::test
{
	void test_enum_coroutine()
	{
#if 0
		const std::vector vb{ 0, 2, 34, 54, 56, 654, 75 };

		auto aa = coenumerate(vb);
		for (auto&& val : aa)
		{
			//val = 450;
		}

		for (auto&& val : coenumerate(vb))
		{
		}
#endif
	}
}
#pragma warning(pop)
