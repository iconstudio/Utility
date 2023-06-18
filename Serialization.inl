#pragma once
#include <type_traits>
#include <utility>
#include <bit>

import Utility.Constraints;
import Utility.Array;

namespace util
{
	static inline constexpr unsigned char Byte = 0XFFU;

	template<basic_arithmeticals T>
	[[nodiscard]]
	constexpr char RShift(const T& value, const size_t& times) noexcept
	{
		return static_cast<char>(static_cast<unsigned char>(value >> (times * 8)) & Byte);
	}
}

namespace util::serialization
{
	/// <summary>
	/// 논리 값을 직렬화합니다.
	/// </summary>
	constexpr Array<char, 1> Serialize(const bool& value) noexcept
	{
		return Array<char, 1>{ value };
	}

	/// <summary>
	/// 문자를 직렬화합니다.
	/// </summary>
	constexpr Array<char, 1> Serialize(const char& value) noexcept
	{
		return Array<char, 1>{ value };
	}

	/// <summary>
	/// 부호 없는 8비트 정수를 직렬화합니다.
	/// </summary>
	constexpr Array<char, 1> Serialize(const unsigned char& value) noexcept
	{
		return Array<char, 1>{ static_cast<char>(value) };
	}

	/// <summary>
	/// UTF-8 문자를 직렬화합니다.
	/// </summary>
	constexpr Array<char, 1> Serialize(const char8_t& value) noexcept
	{
		return Array<char, 1>{ static_cast<char>(value) };
	}

	/// <summary>
	/// 부호 없는 16비트 정수를 직렬화합니다.
	/// </summary>
	constexpr Array<char, 2> Serialize(const unsigned short& value) noexcept
	{
		const unsigned longer = static_cast<unsigned>(value);

		return Array<char, 2>
		{
			RShift(longer, 0), RShift(longer, 1)
		};
	}

	/// <summary>
	/// 부호 없는 16비트 정수를 직렬화합니다.
	/// </summary>
	constexpr Array<char, 2> Serialize(unsigned short&& value) noexcept
	{
		const unsigned longer = static_cast<unsigned>(value);

		return Array<char, 2>
		{
			RShift(longer, 0), RShift(longer, 1)
		};
	}

	/// <summary>
	/// 16비트 정수를 직렬화합니다.
	/// </summary>
	constexpr Array<char, 2> Serialize(const short& value) noexcept
	{
		return Serialize(static_cast<unsigned short>(value));
	}

	/// <summary>
	/// 와이드 문자를 직렬화합니다.
	/// </summary>
	constexpr Array<char, 2> Serialize(const wchar_t& value) noexcept
	{
		return Serialize(static_cast<unsigned short>(value));
	}

	/// <summary>
	/// UTF-16 문자를 직렬화합니다.
	/// </summary>
	constexpr Array<char, 2> Serialize(const char16_t& value) noexcept
	{
		return Serialize(static_cast<unsigned short>(value));
	}

	/// <summary>
	/// 부호 없는 32비트 정수를 직렬화합니다.
	/// </summary>
	constexpr Array<char, 4> Serialize(const unsigned int& value) noexcept
	{
		const unsigned long long longer = static_cast<unsigned long long>(value);

		return Array<char, 4>
		{
			RShift(longer, 0),
			RShift(longer, 1),
			RShift(longer, 2),
			RShift(longer, 3),
		};
	}

	/// <summary>
	/// 부호 없는 32비트 정수를 직렬화합니다.
	/// </summary>
	constexpr Array<char, 4> Serialize(unsigned int&& value) noexcept
	{
		const unsigned long long longer = static_cast<unsigned long long>(value);

		return Array<char, 4>
		{
			RShift(longer, 0),
			RShift(longer, 1),
			RShift(longer, 2),
			RShift(longer, 3),
		};
	}

	/// <summary>
	/// 32비트 정수를 직렬화합니다.
	/// </summary>
	constexpr Array<char, 4> Serialize(const int& value) noexcept
	{
		return Serialize(static_cast<unsigned int>(value));
	}

	/// <summary>
	/// 부호 없는 32비트 정수를 직렬화합니다.
	/// </summary>
	constexpr Array<char, 4> Serialize(const unsigned long& value) noexcept
	{
		const unsigned long long longer = static_cast<unsigned long long>(value);

		return Array<char, 4>
		{
			RShift(longer, 0),
			RShift(longer, 1),
			RShift(longer, 2),
			RShift(longer, 3),
		};
	}

	/// <summary>
	/// 부호 없는 32비트 정수를 직렬화합니다.
	/// </summary>
	constexpr Array<char, 4> Serialize(unsigned long&& value) noexcept
	{
		const unsigned long long longer = static_cast<unsigned long long>(value);

		return Array<char, 4>
		{
			RShift(longer, 0),
			RShift(longer, 1),
			RShift(longer, 2),
			RShift(longer, 3),
		};
	}

	/// <summary>
	/// 32비트 정수를 직렬화합니다.
	/// </summary>
	constexpr Array<char, 4> Serialize(const long& value) noexcept
	{
		return Serialize(static_cast<unsigned long>(value));
	}

	/// <summary>
	/// UTF-32 문자를 직렬화합니다.
	/// </summary>
	constexpr Array<char, 4> Serialize(const char32_t& value) noexcept
	{
		return Serialize(static_cast<unsigned>(value));
	}

	/// <summary>
	/// 부호 없는 64비트 정수를 직렬화합니다.
	/// </summary>
	constexpr Array<char, 8> Serialize(const unsigned long long& value) noexcept
	{
		return Array<char, 8>
		{
			static_cast<char>((value >> 56ULL) & 0xFF),
				static_cast<char>((value >> 48ULL) & 0xFF),
				static_cast<char>((value >> 40ULL) & 0xFF),
				static_cast<char>((value >> 32ULL) & 0xFF),
				static_cast<char>((value >> 24ULL) & 0xFF),
				static_cast<char>((value >> 16ULL) & 0xFF),
				static_cast<char>((value >> 8ULL) & 0xFF),
				static_cast<char>((value) & 0xFF)
		};
	}

	/// <summary>
	/// 64비트 정수를 직렬화합니다.
	/// </summary>
	constexpr Array<char, 8> Serialize(const long long& value) noexcept
	{
		return Serialize(static_cast<unsigned long long>(value));
	}

	/// <summary>
	/// 32비트 부동 소수점을 직렬화합니다.
	/// </summary>
	constexpr Array<char, 4> Serialize(const float& value) noexcept
	{
		return Serialize(std::bit_cast<unsigned int>(value));
	}

	/// <summary>
	/// 64비트 부동 소수점을 직렬화합니다.
	/// </summary>
	constexpr Array<char, 8> Serialize(const double& value) noexcept
	{
		return Serialize(std::bit_cast<unsigned long long>(value));
	}

	/// <summary>
	/// 64비트 부동 소수점을 직렬화합니다.
	/// </summary>
	constexpr Array<char, 8> Serialize(const long double& value) noexcept
	{
		return Serialize(std::bit_cast<unsigned long long>(value));
	}

	/// <summary>
	/// 비트 배열을 직렬화합니다.
	/// </summary>
	template<size_t Length>
	[[nodiscard]]
	constexpr auto Serialize(const bool(&buffer)[Length]) noexcept(0 < Length)
	{
		constexpr size_t bytes_count = std::max(1ULL, Length / 8);

		Array<char, bytes_count> result{};
		for (size_t i = 0; i < bytes_count; ++i)
		{
			char& element = result[i];
			for (size_t j = 0; j < 8; ++j)
			{
				const bool& bit = buffer[i * 8 + j];
				element |= static_cast<char>(bit) << j;
			}
		}

		return result;
	}

	/// <summary>
	/// 16비트 정수 배열을 직렬화합니다.
	/// </summary>
	template<size_t Length>
	[[nodiscard]]
	constexpr Array<char, Length * 2> Serialize(const short(&buffer)[Length]) noexcept(0 < Length)
	{
		Array<char, Length * 2> result{};

		size_t index = 0;
		for (short& item : buffer)
		{
			const Array<char, 2> mid = Serialize(item);

			result[index] = mid[0];
			result[index + 1] = mid[1];

			index += sizeof(short);
		}

		return result;
	}

	/// <summary>
	/// 부호 없는 16비트 정수 배열을 직렬화합니다.
	/// </summary>
	template<size_t Length>
	[[nodiscard]]
	constexpr Array<char, Length * 2> Serialize(const unsigned short(&buffer)[Length]) noexcept(0 < Length)
	{
		Array<char, Length * 2> result{};

		size_t index = 0;
		for (unsigned short& item : buffer)
		{
			const Array<char, 2> mid = Serialize(item);

			result[index] = mid[0];
			result[index + 1] = mid[1];

			index += sizeof(unsigned short);
		}

		return result;
	}

	/// <summary>
	/// 32비트 정수 배열을 직렬화합니다.
	/// </summary>
	template<size_t Length>
	[[nodiscard]]
	constexpr Array<char, Length * 4> Serialize(const int(&buffer)[Length]) noexcept(0 < Length)
	{
		Array<char, Length * 4> result{};

		size_t index = 0;
		for (int& item : buffer)
		{
			const Array<char, 4> mid = Serialize(item);

			result[index] = mid[0];
			result[index + 1] = mid[1];
			result[index + 2] = mid[2];
			result[index + 3] = mid[3];

			index += sizeof(int);
		}

		return result;
	}

	/// <summary>
	/// 부호 없는 32비트 정수 배열을 직렬화합니다.
	/// </summary>
	template<size_t Length>
	[[nodiscard]]
	constexpr Array<char, Length * 4> Serialize(const unsigned int(&buffer)[Length]) noexcept(0 < Length)
	{
		Array<char, Length * 4> result{};

		size_t index = 0;
		for (unsigned int& item : buffer)
		{
			const Array<char, 4> mid = Serialize(item);

			result[index] = mid[0];
			result[index + 1] = mid[1];
			result[index + 2] = mid[2];
			result[index + 3] = mid[3];

			index += sizeof(unsigned int);
		}

		return result;
	}

	/// <summary>
	/// 32비트 정수 배열을 직렬화합니다.
	/// </summary>
	template<size_t Length>
	[[nodiscard]]
	constexpr Array<char, Length * 4> Serialize(const long(&buffer)[Length]) noexcept(0 < Length)
	{
		Array<char, Length * 4> result{};

		size_t index = 0;
		for (long& item : buffer)
		{
			const Array<char, 4> mid = Serialize(item);

			result[index] = mid[0];
			result[index + 1] = mid[1];
			result[index + 2] = mid[2];
			result[index + 3] = mid[3];

			index += sizeof(long);
		}

		return result;
	}

	/// <summary>
	/// 부호 없는 32비트 정수 배열을 직렬화합니다.
	/// </summary>
	template<size_t Length>
	[[nodiscard]]
	constexpr Array<char, Length * 4> Serialize(const unsigned long(&buffer)[Length]) noexcept(0 < Length)
	{
		Array<char, Length * 4> result{};

		size_t index = 0;
		for (unsigned long& item : buffer)
		{
			const Array<char, 4> mid = Serialize(item);

			result[index] = mid[0];
			result[index + 1] = mid[1];
			result[index + 2] = mid[2];
			result[index + 3] = mid[3];

			index += sizeof(unsigned long);
		}

		return result;
	}

	/// <summary>
	/// 32비트 정수 배열을 직렬화합니다.
	/// </summary>
	template<size_t Length>
	[[nodiscard]]
	constexpr Array<char, Length * 8> Serialize(const long long(&buffer)[Length]) noexcept(0 < Length)
	{
		Array<char, Length * 8> result{};

		size_t index = 0;
		for (long long& item : buffer)
		{
			const Array<char, 8> mid = Serialize(item);

			result[index] = mid[0];
			result[index + 1] = mid[1];
			result[index + 2] = mid[2];
			result[index + 3] = mid[3];
			result[index + 4] = mid[4];
			result[index + 5] = mid[5];
			result[index + 6] = mid[6];
			result[index + 7] = mid[7];

			index += sizeof(long long);
		}

		return result;
	}

	/// <summary>
	/// 부호 없는 32비트 정수 배열을 직렬화합니다.
	/// </summary>
	template<size_t Length>
	[[nodiscard]]
	constexpr Array<char, Length * 8> Serialize(const unsigned long long(&buffer)[Length]) noexcept(0 < Length)
	{
		Array<char, Length * 8> result{};

		size_t index = 0;
		for (unsigned long long& item : buffer)
		{
			const Array<char, 8> mid = Serialize(item);

			result[index] = mid[0];
			result[index + 1] = mid[1];
			result[index + 2] = mid[2];
			result[index + 3] = mid[3];
			result[index + 4] = mid[4];
			result[index + 5] = mid[5];
			result[index + 6] = mid[6];
			result[index + 7] = mid[7];

			index += sizeof(unsigned long long);
		}

		return result;
	}

	/// <summary>
	/// 문자열을 직렬화합니다.
	/// </summary>
	template<size_t Length>
	[[nodiscard]]
	constexpr Array<char, Length> Serialize(const char(&buffer)[Length]) noexcept(0 < Length)
	{
		return Array<char, Length>{ buffer };
	}

	/// <summary>
	/// 부호없는 문자열을 직렬화합니다.
	/// </summary>
	template<size_t Length>
	[[nodiscard]]
	constexpr Array<char, Length> Serialize(const unsigned char(&buffer)[Length]) noexcept(0 < Length)
	{
		return Array<char, Length>{ from_range, buffer };
	}

	/// <summary>
	/// UTF-8 문자열을 직렬화합니다.
	/// </summary>
	template<size_t Length>
	[[nodiscard]]
	constexpr Array<char, Length> Serialize(const char8_t(&buffer)[Length]) noexcept(0 < Length)
	{
		return Array<char, Length>{ from_range, buffer };
	}

	/// <summary>
	/// UTF-16 문자열을 직렬화합니다.
	/// </summary>
	template<size_t Length>
	[[nodiscard]]
	constexpr Array<char, Length * 2> Serialize(const char16_t(&buffer)[Length])
	{
		Array<char, Length * 2> result{};

		for (size_t i = 0; i < Length; ++i)
		{
			const char16_t& element = buffer[i];
			const auto mid = Serialize(element);
			result[i * 2] = mid[0];
			result[i * 2 + 1] = mid[1];
		}

		return result;
	}

	/// <summary>
	/// 와이드 문자열을 직렬화합니다.
	/// </summary>
	template<size_t Length>
	[[nodiscard]]
	constexpr Array<char, Length * 2> Serialize(const wchar_t(&buffer)[Length])
	{
		Array<char, Length * 2> result{};

		for (size_t i = 0; i < Length; ++i)
		{
			const wchar_t& element = buffer[i];
			const auto mid = Serialize(element);
			result[i * 2] = mid[0];
			result[i * 2 + 1] = mid[1];
		}

		return result;
	}

	/// <summary>
	/// UTF-32 문자열을 직렬화합니다.
	/// </summary>
	template<size_t Length>
	[[nodiscard]]
	constexpr Array<char, Length * 4> Serialize(const char32_t(&buffer)[Length])
	{
		Array<char, Length * 4> result{};

		for (size_t i = 0; i < Length; ++i)
		{
			const char32_t& element = buffer[i];
			const auto mid = Serialize(element);
			result[i * 4] = mid[0];
			result[i * 4 + 1] = mid[1];
			result[i * 4 + 2] = mid[2];
			result[i * 4 + 3] = mid[3];
		}

		return result;
	}

	template<size_t Length>
		requires (0 < Length)
	[[nodiscard]]
	constexpr auto SerializeArray(const bool* const& buffer) noexcept
	{
		constexpr size_t bytes_count = std::max(1ULL, Length / 8);

		Array<char, bytes_count> result{};
		for (size_t i = 0; i < bytes_count; ++i)
		{
			char& element = result[i];
			for (size_t j = 0; j < 8; ++j)
			{
				const bool& bit = buffer[i * 8 + j];
				element |= static_cast<char>(bit) << j;
			}
		}

		return result;
	}

	template<size_t Length>
		requires (0 < Length)
	[[nodiscard]]
	constexpr auto SerializeArray(const short* const& buffer) noexcept
	{
		Array<char, Length * 2> result{};

		for (size_t i = 0; i < Length; ++i)
		{
			const short& element = buffer[i];
			const auto mid = Serialize(element);
			result[i * 2] = mid[0];
			result[i * 2 + 1] = mid[1];
		}

		return result;
	}

	template<size_t Length>
		requires (0 < Length)
	[[nodiscard]]
	constexpr auto SerializeArray(const unsigned short* const& buffer) noexcept
	{
		Array<char, Length * 2> result{};

		for (size_t i = 0; i < Length; ++i)
		{
			const unsigned short& element = buffer[i];
			const auto mid = Serialize(element);
			result[i * 2] = mid[0];
			result[i * 2 + 1] = mid[1];
		}

		return result;
	}

	template<size_t Length>
		requires (0 < Length)
	[[nodiscard]]
	constexpr Array<char, Length * 4> SerializeArray(const int* const& buffer) noexcept
	{
		Array<char, Length * 4> result{};

		for (size_t i = 0; i < Length; ++i)
		{
			const int& element = buffer[i];
			const auto mid = Serialize(element);
			result[i * 4] = mid[0];
			result[i * 4 + 1] = mid[1];
			result[i * 4 + 2] = mid[2];
			result[i * 4 + 3] = mid[3];
		}

		return result;
	}

	template<size_t Length>
		requires (0 < Length)
	[[nodiscard]]
	constexpr Array<char, Length * 4> SerializeArray(const unsigned int* const& buffer) noexcept
	{
		Array<char, Length * 4> result{};

		for (size_t i = 0; i < Length; ++i)
		{
			const unsigned int& element = buffer[i];
			const auto mid = Serialize(element);
			result[i * 4] = mid[0];
			result[i * 4 + 1] = mid[1];
			result[i * 4 + 2] = mid[2];
			result[i * 4 + 3] = mid[3];
		}

		return result;
	}

	template<size_t Length>
		requires (0 < Length)
	[[nodiscard]]
	constexpr Array<char, Length * 4> SerializeArray(const long* const& buffer) noexcept
	{
		Array<char, Length * 4> result{};

		for (size_t i = 0; i < Length; ++i)
		{
			const long& element = buffer[i];
			const auto mid = Serialize(element);
			result[i * 4] = mid[0];
			result[i * 4 + 1] = mid[1];
			result[i * 4 + 2] = mid[2];
			result[i * 4 + 3] = mid[3];
		}

		return result;
	}

	template<size_t Length>
		requires (0 < Length)
	[[nodiscard]]
	constexpr Array<char, Length * 4> SerializeArray(const unsigned long* const& buffer) noexcept
	{
		Array<char, Length * 4> result{};

		for (size_t i = 0; i < Length; ++i)
		{
			const unsigned long& element = buffer[i];
			const auto mid = Serialize(element);
			result[i * 4] = mid[0];
			result[i * 4 + 1] = mid[1];
			result[i * 4 + 2] = mid[2];
			result[i * 4 + 3] = mid[3];
		}

		return result;
	}

	template<size_t Length>
		requires (0 < Length)
	[[nodiscard]]
	constexpr Array<char, Length * 8> SerializeArray(const long long* const& buffer) noexcept
	{
		Array<char, Length * 8> result{};

		for (size_t i = 0; i < Length; ++i)
		{
			const long long& element = buffer[i];
			const auto mid = Serialize(element);
			result[i * 8] = mid[0];
			result[i * 8 + 1] = mid[1];
			result[i * 8 + 2] = mid[2];
			result[i * 8 + 3] = mid[3];
			result[i * 8 + 4] = mid[4];
			result[i * 8 + 5] = mid[5];
			result[i * 8 + 6] = mid[6];
			result[i * 8 + 7] = mid[7];
		}

		return result;
	}

	template<size_t Length>
		requires (0 < Length)
	[[nodiscard]]
	constexpr Array<char, Length * 8> SerializeArray(const unsigned long long* const& buffer) noexcept
	{
		Array<char, Length * 8> result{};

		for (size_t i = 0; i < Length; ++i)
		{
			const unsigned long long& element = buffer[i];
			const auto mid = Serialize(element);
			result[i * 8] = mid[0];
			result[i * 8 + 1] = mid[1];
			result[i * 8 + 2] = mid[2];
			result[i * 8 + 3] = mid[3];
			result[i * 8 + 4] = mid[4];
			result[i * 8 + 5] = mid[5];
			result[i * 8 + 6] = mid[6];
			result[i * 8 + 7] = mid[7];
		}

		return result;
	}

	template<size_t Length>
		requires (0 < Length)
	[[nodiscard]]
	constexpr Array<char, Length * 4> SerializeArray(const float* const& buffer) noexcept
	{
		Array<char, Length * 4> result{};

		for (size_t i = 0; i < Length; ++i)
		{
			const float& element = buffer[i];
			const auto mid = Serialize(element);
			result[i * 4] = mid[0];
			result[i * 4 + 1] = mid[1];
			result[i * 4 + 2] = mid[2];
			result[i * 4 + 3] = mid[3];
		}

		return result;
	}

	template<size_t Length>
		requires (0 < Length)
	[[nodiscard]]
	constexpr Array<char, Length * 8> SerializeArray(const double* const& buffer) noexcept
	{
		Array<char, Length * 8> result{};

		for (size_t i = 0; i < Length; ++i)
		{
			const double& element = buffer[i];
			const auto mid = Serialize(element);
			result[i * 8] = mid[0];
			result[i * 8 + 1] = mid[1];
			result[i * 8 + 2] = mid[2];
			result[i * 8 + 3] = mid[3];
			result[i * 8 + 4] = mid[4];
			result[i * 8 + 5] = mid[5];
			result[i * 8 + 6] = mid[6];
			result[i * 8 + 7] = mid[7];
		}

		return result;
	}

	template<size_t Length>
		requires (0 < Length)
	[[nodiscard]]
	constexpr Array<char, Length * 8> SerializeArray(const long double* const& buffer) noexcept
	{
		Array<char, Length * 8> result{};

		for (size_t i = 0; i < Length; ++i)
		{
			const long double& element = buffer[i];
			const auto mid = Serialize(element);
			result[i * 8] = mid[0];
			result[i * 8 + 1] = mid[1];
			result[i * 8 + 2] = mid[2];
			result[i * 8 + 3] = mid[3];
			result[i * 8 + 4] = mid[4];
			result[i * 8 + 5] = mid[5];
			result[i * 8 + 6] = mid[6];
			result[i * 8 + 7] = mid[7];
		}

		return result;
	}

	template<size_t Length>
		requires (0 < Length)
	[[nodiscard]]
	constexpr Array<char, Length> SerializeArray(const char* const& buffer) noexcept
	{
		return Array<char, Length>{ buffer, buffer + Length };
	}

	template<size_t Length>
		requires (0 < Length)
	[[nodiscard]]
	constexpr Array<char, Length> SerializeArray(const unsigned char* const& buffer) noexcept
	{
		return Array<char, Length>{ buffer, buffer + Length };
	}

	template<size_t Length>
		requires (0 < Length)
	[[nodiscard]]
	constexpr Array<char, Length> SerializeArray(const char8_t* const& buffer) noexcept
	{
		return Array<char, Length>{ buffer, buffer + Length };
	}

	template<size_t Length>
		requires (0 < Length)
	[[nodiscard]]
	constexpr Array<char, Length * 2> SerializeArray(const char16_t* const& buffer) noexcept
	{
		Array<char, Length * 2> result{};

		for (size_t i = 0; i < Length; ++i)
		{
			const char16_t& element = buffer[i];
			const auto mid = Serialize(element);
			result[i * 2] = mid[0];
			result[i * 2 + 1] = mid[1];
		}

		return result;
	}

	template<size_t Length>
		requires (0 < Length)
	[[nodiscard]]
	constexpr Array<char, Length * 2> SerializeArray(const wchar_t* const& buffer) noexcept
	{
		Array<char, Length * 2> result{};

		for (size_t i = 0; i < Length; ++i)
		{
			const wchar_t& element = buffer[i];
			const auto mid = Serialize(element);
			result[i * 2] = mid[0];
			result[i * 2 + 1] = mid[1];
		}

		return result;
	}

	template<size_t Length>
		requires (0 < Length)
	[[nodiscard]]
	constexpr Array<char, Length * 4> SerializeArray(const char32_t* const& buffer)
	{
		Array<char, Length * 4> result{};

		for (size_t i = 0; i < Length; ++i)
		{
			const char32_t& element = buffer[i];
			const auto mid = Serialize(element);
			result[i * 2] = mid[0];
			result[i * 2 + 1] = mid[1];
			result[i * 2 + 2] = mid[2];
			result[i * 2 + 3] = mid[3];
		}

		return result;
	}
}
