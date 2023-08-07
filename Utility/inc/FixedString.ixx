export module Utility.FixedString;
import <stdexcept>;
import <type_traits>;
import <string_view>;

export namespace util
{
	template <typename Char, size_t N>
	struct basic_fixed_string
	{
		static_assert(std::is_trivially_copyable_v<Char>, "Char must be trivially copyable.");
		static_assert(std::is_standard_layout_v<Char>, "Char must be standard layout.");
		static_assert(std::is_integral_v<Char>, "Char must be an integral type.");
		static_assert(0 < N, "Fixed string must have at least one element.");

		using value_type = Char;
		using reference = std::add_lvalue_reference_t<value_type>;
		using const_reference = std::add_lvalue_reference_t<const value_type>;
		using rvalue_reference = std::add_rvalue_reference_t<value_type>;
		using const_rvalue_reference = std::add_rvalue_reference_t<value_type>;
		using size_type = size_t;
		using difference_type = ptrdiff_t;

		using iterator = const Char*;
		using const_iterator = const Char* const;

		consteval basic_fixed_string() noexcept = default;
		constexpr ~basic_fixed_string() noexcept = default;

		consteval basic_fixed_string(const std::basic_string_view<Char>& view) noexcept
			: intBuffer()
		{
			for (size_t i = 0; i < N; ++i)
			{
				if (0 == view[i])
				{
					break;
				}

				intBuffer[i] = view[i];
			}
		}

		consteval basic_fixed_string(const Char* const& buffer) noexcept
			: intBuffer()
		{
			for (size_t i = 0; i < N; ++i)
			{
				if (0 == buffer[i])
				{
					break;
				}

				intBuffer[i] = buffer[i];
			}
		}

		template <size_t Length>
		consteval basic_fixed_string(const Char* (&buffer)[Length]) noexcept
			: intBuffer()
		{
			static_assert(Length <= N);

			for (size_t i = 0; i < Length; ++i)
			{
				intBuffer[i] = buffer[i];
			}
		}

		consteval basic_fixed_string(const Char* const& buffer, const size_t& length) noexcept
			: intBuffer()
		{
			for (size_t i = 0; i < length; ++i)
			{
				intBuffer[i] = buffer[i];
			}
		}

		consteval operator std::basic_string_view<Char>() const noexcept
		{
			return std::basic_string_view<Char>(intBuffer, N);
		}

		[[nodiscard]]
		constexpr iterator begin() noexcept
		{
			return intBuffer;
		}

		[[nodiscard]]
		constexpr const_iterator begin() const noexcept
		{
			return intBuffer;
		}

		[[nodiscard]]
		constexpr const_iterator end() const noexcept
		{
			return intBuffer + N;
		}

		[[nodiscard]]
		constexpr const_iterator cbegin() const noexcept
		{
			return intBuffer;
		}

		[[nodiscard]]
		constexpr const_iterator cend() const noexcept
		{
			return intBuffer + N;
		}

		[[nodiscard]]
		constexpr reference operator[](const size_t& index)
		{
			return intBuffer[index];
		}

		[[nodiscard]]
		constexpr const_reference operator[](const size_t& index) const
		{
			return intBuffer[index];
		}

		[[nodiscard]]
		constexpr reference at(const size_t& index)
		{
			if (N <= index)
			{
				throw std::out_of_range("Index out of range.");
			}

			return intBuffer[index];
		}

		[[nodiscard]]
		constexpr const_reference at(const size_t& index) const
		{
			if (N <= index)
			{
				throw std::out_of_range("Index out of range.");
			}

			return intBuffer[index];
		}

		[[nodiscard]]
		constexpr auto& (data)() noexcept
		{
			return intBuffer;
		}

		[[nodiscard]]
		constexpr const auto& (data)() const noexcept
		{
			return intBuffer;
		}

		[[nodiscard]]
		static consteval size_t size() noexcept
		{
			return N;
		}

		Char intBuffer[N];
	};

	template <typename Char, size_t N>
	basic_fixed_string(const Char(&str)[N]) -> basic_fixed_string<Char, N>;

	template <size_t N>
	using fixed_string = basic_fixed_string<char, N>;

	template <size_t N>
	using fixed_u8string = basic_fixed_string<char8_t, N>;

	template <size_t N>
	using fixed_u16string = basic_fixed_string<char16_t, N>;

	template <size_t N>
	using fixed_u32string = basic_fixed_string<char32_t, N>;

	template <size_t N>
	using fixed_wstring = basic_fixed_string<wchar_t, N>;
}
