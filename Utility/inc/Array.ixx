export module Utility.Array;
import <system_error>;
import <utility>;
export import <array>;
import <algorithm>;
import <ranges>;
import Utility;
import Utility.Constraints;

export namespace util
{
	using ::std::from_range_t;
	using ::std::from_range;
	using ::std::initializer_list;

	template<typename T, size_t Length>
	using array_iterator = std::_Array_iterator<T, Length>;

	template<typename T, size_t Length>
	using array_const_iterator = std::_Array_const_iterator<T, Length>;

	struct empty_array_elemement { constexpr explicit empty_array_elemement() = default; };

	template<typename T, size_t Length>
	class Array
	{
	public:
		static_assert(!std::same_as<T, void>, "Cannot use void type as array element.");

		using value_type = T;
		using const_value_type = const T;
		using pointer = T*;
		using const_pointer = const T*;
		using reference = T&;
		using const_reference = const T&;

		using size_type = size_t;
		using difference_type = ptrdiff_t;
		using iterator = array_iterator<value_type, Length>;
		using const_iterator = array_const_iterator<value_type, Length>;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

		constexpr Array()
			noexcept(nothrow_constructibles<value_type>) requires(default_initializable<value_type>) = default;
		constexpr ~Array()
			noexcept(nothrow_destructibles<value_type>) = default;
		constexpr Array(const Array& other)
			noexcept(nothrow_copy_assignables<value_type>) requires copyable<value_type> = default;
		constexpr Array(Array&& other)
			noexcept(nothrow_move_assignables<value_type>) requires movable<value_type> = default;

		explicit(!trivials<value_type>)
			constexpr Array(const initializer_list<value_type> elements)
			noexcept(nothrow_copy_assignables<value_type>) requires copyable<value_type>
		{
			std::copy(elements.begin(), elements.end(), begin());
		}

		template<size_t L2>
			requires (0 < L2)
		explicit constexpr Array(const value_type(&buffer)[L2])
			noexcept(nothrow_copy_assignables<value_type>)
			requires copyable<value_type>
		{
			iterator it = begin();

			for (const value_type& elemement : buffer)
			{
				*it = elemement;

				if (++it == end())
					break;
			}
		}

		template<size_t L2>
			requires (0 < L2)
		explicit constexpr Array(value_type(&& buffer)[L2])
			noexcept(nothrow_move_assignables<value_type>)
			requires movable<value_type>
		{
			iterator it = begin();

			for (value_type& elemement : buffer)
			{
				*it = static_cast<value_type&&>(elemement);

				if (++it == end())
					break;
			}
		}

		template<std::ranges::input_range R>
		explicit constexpr Array(std::from_range_t, R&& range)
			noexcept(nothrow_convertibles<std::iter_value_t<std::ranges::iterator_t<R>>, value_type>)
		{
			std::copy(range.begin(), range.end(), begin());
		}

		template<std::input_iterator It, std::sentinel_for<It> Guard>
		explicit constexpr Array(It ibegin, const Guard iend)
			noexcept(nothrow_convertibles<std::iter_value_t<It>, value_type>)
		{
			std::copy(ibegin, iend, begin());
		}

		constexpr Array& operator=(const Array& other)
			noexcept(nothrow_copy_assignables<value_type>) requires std::movable<value_type>
		{
			if (&other == static_cast<const Array* const&>(this))
			{
				return *this;
			}

			std::copy(other.cbegin(), other.cend(), begin());

			return *this;
		}

		constexpr Array& operator=(Array&& other)
			noexcept(nothrow_move_assignables<value_type>) requires std::movable<value_type>
		{
			std::move(other.cbegin(), other.cend(), begin());

			return *this;
		}

		template<invocables<reference> Fn>
		constexpr void Apply(Fn&& function)
			noexcept(noexcept(forward<Fn>(function)(declval<reference>())))
		{
			for (reference elem : *this)
			{
				std::forward<Fn>(function)(elem);
			}
		}

		template<invocables<const_reference> Fn>
		constexpr void Apply(Fn&& function) const
			noexcept(noexcept(forward<Fn>(function)(declval<const_reference>())))
		{
			for (const_reference elem : *this)
			{
				std::forward<Fn>(function)(elem);
			}
		}

		template<convertible_to<value_type> Uty, size_t L2>
			requires (0 < L2)
		constexpr void Assign(const Array<Uty, L2>& other)
			noexcept(nothrow_assignables<value_type, Uty>)
		{
			auto src = other.cbegin();

			for (iterator it = begin(); it != end() && src != other.cend(); (void) ++it)
			{
				*it = *(src++);
			}
		}

		template<convertible_to<value_type> Uty, size_t L2>
			requires (0 < L2)
		constexpr void Assign(Array<Uty, L2>&& other)
			noexcept(nothrow_assignables<value_type, Uty>)
		{
			auto src = other.begin();

			for (iterator it = begin(); it != end() && src != other.end(); (void) ++it)
			{
				*it = static_cast<Uty&&>(*(src++));
			}
		}

		constexpr void Fill(const value_type& fill_value)
			noexcept(nothrow_copy_assignables<value_type>)
		{
			std::fill_n(myData, Length, fill_value);
		}

		template<size_t L2>
			requires (0 < L2)
		constexpr void CopyTo(std::array<value_type, L2>& target) const
			noexcept(nothrow_copy_assignables<value_type>)
		{
			array_iterator<value_type, L2> oit = target.begin();

			for (const_iterator it = cbegin(); it != cend(); ++it, (void) ++oit)
			{
				*oit = *it;
			}
		}

		template<size_t L2>
			requires (0 < L2)
		constexpr void CopyTo(Array<value_type, L2>& target) const
			noexcept(nothrow_copy_assignables<value_type>)
		{
			iterator oit = target.begin();

			for (const_iterator it = cbegin(); it != cend(); ++it, (void) ++oit)
			{
				*oit = *it;
			}
		}

		template<size_t L2>
			requires (0 < L2)
		constexpr void CopyTo(value_type(&target)[L2]) const
			noexcept(nothrow_copy_assignables<value_type>)
		{
			value_type* oit = target;

			for (const_iterator it = cbegin(); it != cend(); ++it, (void) ++oit)
			{
				*oit = *it;
			}
		}

		constexpr void CopyTo(value_type* const& target, const size_t& length) const
		{
			value_type* oit = target;

			for (const_iterator it = cbegin();
				it != cend() && oit < target + length;
				++it, (void) ++oit)
			{
				*oit = *it;
			}
		}

		constexpr void swap(std::array<value_type, Length>& other_data)
			noexcept(nothrow_swappables<value_type>)
		{
			auto oit = other_data.begin();

			for (iterator it = begin(); it != end(); ++it, (void) ++oit)
			{
				std::iter_swap(it, oit);
			}
		}

		constexpr void swap(Array& other)
			noexcept(nothrow_swappables<value_type>)
		{
			auto oit = other.begin();

			for (iterator it = begin(); it != end(); ++it, (void) ++oit)
			{
				std::iter_swap(it, oit);
			}
		}

		[[nodiscard]]
		constexpr iterator begin() noexcept
		{
			return iterator{ myData, 0 };
		}

		[[nodiscard]]
		constexpr iterator end() noexcept
		{
			return iterator{ myData, Length };
		}

		[[nodiscard]]
		constexpr const_iterator begin() const noexcept
		{
			return const_iterator{ myData, 0 };
		}

		[[nodiscard]]
		constexpr const_iterator end() const noexcept
		{
			return const_iterator{ myData, Length };
		}

		[[nodiscard]]
		constexpr reverse_iterator rbegin() noexcept
		{
			return reverse_iterator{ end() };
		}

		[[nodiscard]]
		constexpr const_reverse_iterator rbegin() const noexcept
		{
			return const_reverse_iterator{ end() };
		}

		[[nodiscard]]
		constexpr reverse_iterator rend() noexcept
		{
			return reverse_iterator{ begin() };
		}

		[[nodiscard]]
		constexpr const_reverse_iterator rend() const noexcept
		{
			return const_reverse_iterator{ begin() };
		}

		[[nodiscard]]
		constexpr const_iterator cbegin() const noexcept
		{
			return begin();
		}

		[[nodiscard]]
		constexpr const_iterator cend() const noexcept
		{
			return end();
		}

		[[nodiscard]]
		constexpr const_reverse_iterator crbegin() const noexcept
		{
			return rbegin();
		}

		[[nodiscard]]
		constexpr const_reverse_iterator crend() const noexcept
		{
			return rend();
		}

		[[nodiscard]]
		constexpr
			reference
			at(const size_type& index)&
		{
			if (Length <= index)
			{
				throw std::length_error{ "배열의 크기 참조 오류" };
			}

			return myData[index];
		}

		[[nodiscard]]
		constexpr
			const_reference
			at(const size_type& index) const&
		{
			if (Length <= index)
			{
				throw std::length_error{ "배열의 크기 참조 오류" };
			}

			return myData[index];
		}

		[[nodiscard]]
		constexpr
			value_type&&
			at(const size_type& index)&&
		{
			if (Length <= index)
			{
				throw std::length_error{ "배열의 크기 참조 오류" };
			}

			return std::move(myData[index]);
		}

		[[nodiscard]]
		constexpr
			const_value_type&&
			at(const size_type& index) const&&
		{
			if (Length <= index)
			{
				throw std::length_error{ "배열의 크기 참조 오류" };
			}

			return std::move(myData[index]);
		}

		[[nodiscard]]
		constexpr reference
			operator[](_In_range_(0, Length - 1) const size_type& index) & noexcept
		{
			return myData[index];
		}

		[[nodiscard]]
		constexpr
			const_reference
			operator[](_In_range_(0, Length - 1) const size_type& index) const& noexcept
		{
			return myData[index];
		}

		[[nodiscard]]
		constexpr
			value_type&&
			operator[](_In_range_(0, Length - 1) const size_type& index) && noexcept
		{
			return std::move(myData[index]);
		}

		[[nodiscard]]
		constexpr
			const_value_type&&
			operator[](_In_range_(0, Length - 1) const size_type& index) const&& noexcept
		{
			return std::move(myData[index]);
		}

		[[nodiscard]]
		constexpr reference front() & noexcept
		{
			return myData[0];
		}

		[[nodiscard]]
		constexpr const_reference front() const& noexcept
		{
			return myData[0];
		}

		[[nodiscard]]
		constexpr value_type&& front() && noexcept
		{
			return std::move(myData[0]);
		}

		[[nodiscard]]
		constexpr const value_type&& front() const&& noexcept
		{
			return std::move(myData[0]);
		}

		[[nodiscard]]
		constexpr reference back() & noexcept
		{
			return myData[Length - 1];
		}

		[[nodiscard]]
		constexpr const_reference back() const& noexcept
		{
			return myData[Length - 1];
		}

		[[nodiscard]]
		constexpr value_type&& back() && noexcept
		{
			return std::move(myData[Length - 1]);
		}

		[[nodiscard]]
		constexpr const value_type&& back() const&& noexcept
		{
			return std::move(myData[Length - 1]);
		}

		[[nodiscard]]
		constexpr pointer data() & noexcept
		{
			return myData;
		}

		[[nodiscard]]
		constexpr const_pointer data() const& noexcept
		{
			return myData;
		}

		[[nodiscard]]
		constexpr pointer&& data() && noexcept
		{
			return std::move(myData);
		}

		[[nodiscard]]
		constexpr size_type size() const noexcept
		{
			return Length;
		}

		[[nodiscard]]
		static inline consteval size_type max_size() noexcept
		{
			return Length;
		}

		[[nodiscard]]
		static inline consteval bool empty() noexcept
		{
			return false;
		}

	private:
		T myData[Length];
	};

	template<typename T>
	class Array<T, 0>
	{
	public:
		using value_type = T;
		using const_value_type = const T;
		using pointer = T*;
		using const_pointer = const T*;
		using reference = T&;
		using const_reference = const T&;

		using size_type = size_t;
		using difference_type = ptrdiff_t;
		using iterator = array_iterator<T, 0>;
		using const_iterator = array_const_iterator<T, 0>;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

		constexpr Array() noexcept = default;
		constexpr ~Array() noexcept = default;

		template<size_t OLength>
		constexpr Array([[maybe_unused]] const T(&raw_data)[OLength]) noexcept
		{}

		template<std::convertible_to<T> Elem2>
		explicit constexpr Array([[maybe_unused]] const std::initializer_list<Elem2> elements) noexcept
		{}

		constexpr Array(const Array& other) noexcept = default;
		constexpr Array(Array&& other) noexcept = default;
		constexpr Array& operator=(const Array& other) noexcept = default;
		constexpr Array& operator=(Array&& other) noexcept = default;

		template<std::invocable<value_type&> Fn>
		constexpr void apply([[maybe_unused]] Fn&& function) noexcept
		{}

		template<std::invocable<const_value_type&> Fn>
		constexpr void apply([[maybe_unused]] Fn&& function) const noexcept
		{}

		template<std::convertible_to<value_type> OElem, size_t OLength>
		constexpr void assign([[maybe_unused]] const Array<OElem, OLength>& other) noexcept
		{}

		template<std::convertible_to<value_type> OElem, size_t OLength>
		constexpr void assign([[maybe_unused]] Array<OElem, OLength>&& other) noexcept
		{}

		constexpr void fill([[maybe_unused]] const value_type& fill_value) noexcept
		{}

		constexpr void fill([[maybe_unused]] value_type&& fill_value) noexcept
		{}

		constexpr void swap([[maybe_unused]] Array& other) noexcept
		{}

		[[nodiscard]]
		constexpr iterator begin() noexcept
		{
			return iterator{ nullptr, 0 };
		}

		[[nodiscard]]
		constexpr iterator end() noexcept
		{
			return iterator{ nullptr, 1 };
		}

		[[nodiscard]]
		constexpr const_iterator begin() const noexcept
		{
			return const_iterator{ nullptr, 0 };
		}

		[[nodiscard]]
		constexpr const_iterator end() const noexcept
		{
			return const_iterator{ nullptr, 1 };
		}

		[[nodiscard]]
		constexpr reverse_iterator rbegin() noexcept
		{
			return reverse_iterator{ end() };
		}

		[[nodiscard]]
		constexpr const_reverse_iterator rbegin() const noexcept
		{
			return const_reverse_iterator{ end() };
		}

		[[nodiscard]]
		constexpr reverse_iterator rend() noexcept
		{
			return reverse_iterator{ begin() };
		}

		[[nodiscard]]
		constexpr const_reverse_iterator rend() const noexcept
		{
			return const_reverse_iterator{ begin() };
		}

		[[nodiscard]]
		constexpr const_iterator cbegin() const noexcept
		{
			return begin();
		}

		[[nodiscard]]
		constexpr const_iterator cend() const noexcept
		{
			return end();
		}

		[[nodiscard]]
		constexpr const_reverse_iterator crbegin() const noexcept
		{
			return rbegin();
		}

		[[nodiscard]]
		constexpr const_reverse_iterator crend() const noexcept
		{
			return rend();
		}

		[[nodiscard]]
		constexpr
			reference
			at([[maybe_unused]] const size_type& index)&
		{
			return *begin();
		}

		[[nodiscard]]
		constexpr
			const_reference
			at([[maybe_unused]] const size_type& index) const&
		{
			return *cbegin();
		}

		[[nodiscard]]
		constexpr
			value_type&&
			at([[maybe_unused]] const size_type& index)&&
		{
			return std::move(*begin());
		}

		[[nodiscard]]
		constexpr
			const_value_type&&
			at([[maybe_unused]] const size_type& index) const&&
		{
			return std::move(*cbegin());
		}

		[[nodiscard]]
		constexpr reference
			operator[]([[maybe_unused]] const size_type& index) & noexcept
		{
			return *begin();
		}

		[[nodiscard]]
		constexpr
			const_reference
			operator[]([[maybe_unused]] const size_type& index) const& noexcept
		{
			return *cbegin();
		}

		[[nodiscard]]
		constexpr
			value_type&&
			operator[]([[maybe_unused]] const size_type& index) && noexcept
		{
			return std::move(*begin());
		}

		[[nodiscard]]
		constexpr
			const_value_type&&
			operator[]([[maybe_unused]] const size_type& index) const&& noexcept
		{
			return std::move(*begin());
		}

		[[nodiscard]]
		constexpr reference front() & noexcept
		{
			return *begin();
		}

		[[nodiscard]]
		constexpr const_reference front() const& noexcept
		{
			return *cbegin();
		}

		[[nodiscard]]
		constexpr value_type&& front() && noexcept
		{
			return std::move(*begin());
		}

		[[nodiscard]]
		constexpr const value_type&& front() const&& noexcept
		{
			return std::move(*cbegin());
		}

		[[nodiscard]]
		constexpr reference back() & noexcept
		{
			return *begin();
		}

		[[nodiscard]]
		constexpr const_reference back() const& noexcept
		{
			return *begin();
		}

		[[nodiscard]]
		constexpr value_type&& back() && noexcept
		{
			return std::move(*begin());
		}

		[[nodiscard]]
		constexpr const value_type&& back() const&& noexcept
		{
			return std::move(*cbegin());
		}

		[[nodiscard]]
		constexpr pointer data() & noexcept
		{
			return nullptr;
		}

		[[nodiscard]]
		constexpr const_pointer data() const& noexcept
		{
			return nullptr;
		}

		[[nodiscard]]
		constexpr pointer&& data() && noexcept
		{
			return nullptr;
		}

		[[nodiscard]]
		constexpr size_type size() const noexcept
		{
			return 0;
		}

		[[nodiscard]]
		static inline consteval size_type max_size() noexcept
		{
			return 0;
		}

		[[nodiscard]]
		static inline consteval bool empty() noexcept
		{
			return true;
		}

	private:
		[[maybe_unused]] empty_array_elemement _unused[1];
	};

	using ::std::copy;
	using ::std::transform;
	using ::std::swap;
}

export template<typename T, size_t Length>
constexpr bool std::ranges::enable_borrowed_range<util::Array<T, Length>> = true;

export namespace std
{
	template<typename T, size_t Length>
	constexpr void swap(::util::Array<T, Length>& lhs, ::util::Array<T, Length>& rhs) noexcept
	{
		lhs.swap(rhs);
	}
}
