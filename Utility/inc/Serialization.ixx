module;
#include "Serialization.inl"

export module Utility.Datagram.Serialization;
import Utility;
import Utility.String;
import Utility.Array;
import Utility.Constraints;

export namespace util
{
	template<typename T>
	struct [[nodiscard]] Serializer
	{
		Serializer() = delete;
		~Serializer() = delete;
	};

	template<typename T, size_t Length>
	struct [[nodiscard]] Serializer<T[Length]>
	{
		Serializer() = delete;
		~Serializer() = delete;
	};

	template<typename T>
	concept serializable = requires(const T & value, Serializer<T>&parser)
	{
		parser.Parse(value);
	};

	template<typename... Ts>
	struct test_serializables : true_type
	{};

	template<typename T, typename... Ts>
	struct test_serializables<T, Ts...> : conditional_t<serializable<T>, test_serializables<Ts...>, false_type>
	{};

	template<typename... Ts>
	inline constexpr bool test_serializables_v = test_serializables<Ts...>::value;

	template<typename... Ts>
	concept serializables = test_serializables_v<Ts...>;

	template<typename T>
	constexpr
		decltype(Serializer<clean_t<T>>::Parse(declval<clean_t<T>>()))
		Serialize(const T& value)
		noexcept(noexcept(Serializer<clean_t<T>>::template Parse(declval<clean_t<T>>())))
	{
		static_assert(serializables<clean_t<T>>, "T Cannot be serialized.");

		return Serializer<clean_t<T>>::template Parse(value);
	}

	template<typename T, size_t Length>
		requires (serializables<clean_t<T>[]>)
	constexpr
		decltype(Serializer<clean_t<T>[]>::Parse(declval<clean_t<T>[Length]>()))
		Serialize(const T(&buffer)[Length])
		noexcept(noexcept(Serializer<clean_t<T>[]>::template Parse(declval<clean_t<T>[Length]>())))
	{
		return Serializer<clean_t<T>[]>::template Parse(buffer);
	}

	template<typename T, size_t Length>
		requires (!serializables<clean_t<T>[]>)
	constexpr
		Array<char, sizeof(T)* Length>
		Serialize(const T(&buffer)[Length])
		noexcept(noexcept(Serializer<clean_t<T>>::template Parse(declval<clean_t<T>>())))
	{
		if constexpr (serializables<clean_t<T>>)
		{
			Array<char, sizeof(T)* Length> result{};

			size_t offset = 0;
			for (const T& element : buffer)
			{
				const Array<char, sizeof(T)> mid = Serializer<clean_t<T>>::template Parse(element);

				for (const char& character : mid)
				{
					result[offset++] = character;
				}
			}

			return result;
		}
		else
		{
			static_assert(always_false<clean_t<T>>, "T[] Cannot be serialized.");
		}
	}

	template<enumerations E>
	constexpr
		decltype(Serializer<underlying_type_t<clean_t<E>>>::Parse(declval<underlying_type_t<clean_t<E>>>()))
		Serialize(const E& value) noexcept
	{
		return Serializer<underlying_type_t<E>>::template Parse(static_cast<underlying_type_t<clean_t<E>>>(value));
	}
}

export template<> struct
util::Serializer<bool>
{
	[[nodiscard]]
	static constexpr util::Array<char, 1> Parse(const bool& value) noexcept
	{
		return serialization::Serialize(value);
	}

	[[nodiscard]]
	static constexpr util::Array<char, 1> Parse(bool&& value) noexcept
	{
		return serialization::Serialize(static_cast<bool&&>(value));
	}
};

export template<> struct
util::Serializer<bool[]>
{
	template<size_t Length>
	[[nodiscard]]
	static constexpr auto Parse(const bool(&buffer)[Length]) noexcept
	{
		static_assert(0 < Length, "The length of the array must be greater than zero.");

		return serialization::SerializeArray<Length>(buffer);
	}
};

export template<> struct
util::Serializer<char>
{
	[[nodiscard]]
	static constexpr util::Array<char, 1> Parse(const char& value) noexcept
	{
		return serialization::Serialize(value);
	}

	[[nodiscard]]
	static constexpr util::Array<char, 1> Parse(char&& value) noexcept
	{
		return serialization::Serialize(static_cast<char&&>(value));
	}
};

export template<> struct
util::Serializer<char[]>
{
	template<size_t Length>
	[[nodiscard]]
	static constexpr util::Array<char, Length> Parse(const char(&buffer)[Length]) noexcept
	{
		static_assert(0 < Length, "The length of the array must be greater than zero.");

		return serialization::SerializeArray<Length>(buffer);
	}
};

export template<> struct
util::Serializer<unsigned char>
{
	[[nodiscard]]
	static constexpr util::Array<char, 1> Parse(const unsigned char& value) noexcept
	{
		return serialization::Serialize(value);
	}

	[[nodiscard]]
	static constexpr util::Array<char, 1> Parse(unsigned char&& value) noexcept
	{
		return serialization::Serialize(static_cast<unsigned char&&>(value));
	}
};

export template<> struct
util::Serializer<unsigned char[]>
{
	template<size_t Length>
	[[nodiscard]]
	static constexpr util::Array<char, Length> Parse(const unsigned char(&buffer)[Length]) noexcept
	{
		static_assert(0 < Length, "The length of the array must be greater than zero.");

		return serialization::SerializeArray<Length>(buffer);
	}
};

export template<> struct
util::Serializer<char8_t>
{
	[[nodiscard]]
	static constexpr util::Array<char, 1> Parse(const char8_t& value) noexcept
	{
		return serialization::Serialize(value);
	}

	[[nodiscard]]
	static constexpr util::Array<char, 1> Parse(char8_t&& value) noexcept
	{
		return serialization::Serialize(static_cast<char8_t&&>(value));
	}
};

export template<> struct
util::Serializer<char8_t[]>
{
	template<size_t Length>
	[[nodiscard]]
	static constexpr util::Array<char, Length> Parse(const char8_t(&buffer)[Length]) noexcept
	{
		static_assert(0 < Length, "The length of the array must be greater than zero.");

		return serialization::SerializeArray<Length>(buffer);
	}
};

export template<> struct
util::Serializer<short>
{
	[[nodiscard]]
	static constexpr util::Array<char, 2> Parse(const short& value) noexcept
	{
		return serialization::Serialize(value);
	}

	[[nodiscard]]
	static constexpr util::Array<char, 2> Parse(short&& value) noexcept
	{
		return serialization::Serialize(static_cast<short&&>(value));
	}
};

export template<> struct
util::Serializer<short[]>
{
	template<size_t Length>
	[[nodiscard]]
	static constexpr util::Array<char, Length * 2> Parse(const short(&buffer)[Length]) noexcept
	{
		static_assert(0 < Length, "The length of the array must be greater than zero.");

		return serialization::SerializeArray<Length>(buffer);
	}
};

export template<> struct
util::Serializer<unsigned short>
{
	[[nodiscard]]
	static constexpr util::Array<char, 2> Parse(const unsigned short& value) noexcept
	{
		return serialization::Serialize(value);
	}

	[[nodiscard]]
	static constexpr util::Array<char, 2> Parse(unsigned short&& value) noexcept
	{
		return serialization::Serialize(static_cast<unsigned short&&>(value));
	}
};

export template<> struct
util::Serializer<unsigned short[]>
{
	template<size_t Length>
	[[nodiscard]]
	static constexpr util::Array<char, Length * 2> Parse(const unsigned short(&buffer)[Length]) noexcept
	{
		static_assert(0 < Length, "The length of the array must be greater than zero.");

		return serialization::SerializeArray<Length>(buffer);
	}
};

export template<> struct
util::Serializer<char16_t>
{
	[[nodiscard]]
	static constexpr util::Array<char, 2> Parse(const char16_t& value) noexcept
	{
		return serialization::Serialize(value);
	}

	[[nodiscard]]
	static constexpr util::Array<char, 2> Parse(char16_t&& value) noexcept
	{
		return serialization::Serialize(static_cast<char16_t&&>(value));
	}
};

export template<> struct
util::Serializer<char16_t[]>
{
	template<size_t Length>
	[[nodiscard]]
	static constexpr util::Array<char, Length * 2> Parse(const char16_t(&buffer)[Length]) noexcept
	{
		static_assert(0 < Length, "The length of the array must be greater than zero.");

		return serialization::SerializeArray<Length>(buffer);
	}
};

export template<> struct
util::Serializer<wchar_t>
{
	[[nodiscard]]
	static constexpr util::Array<char, 2> Parse(const wchar_t& value) noexcept
	{
		return serialization::Serialize(value);
	}

	[[nodiscard]]
	static constexpr util::Array<char, 2> Parse(wchar_t&& value) noexcept
	{
		return serialization::Serialize(static_cast<wchar_t&&>(value));
	}
};

export template<> struct
util::Serializer<wchar_t[]>
{
	template<size_t Length>
	[[nodiscard]]
	static constexpr util::Array<char, Length * 2> Parse(const wchar_t(&buffer)[Length]) noexcept
	{
		static_assert(0 < Length, "The length of the array must be greater than zero.");

		return serialization::SerializeArray<Length>(buffer);
	}
};

export template<> struct
util::Serializer<int>
{
	[[nodiscard]]
	static constexpr util::Array<char, 4> Parse(const int& value) noexcept
	{
		return serialization::Serialize(value);
	}

	[[nodiscard]]
	static constexpr util::Array<char, 4> Parse(int&& value) noexcept
	{
		return serialization::Serialize(static_cast<int&&>(value));
	}

	template<size_t Offset, size_t Length>
	static constexpr void ParseTo(const int& value, util::Array<char, Length>& dest)
		noexcept(Offset + sizeof(int) <= Length)
	{
		if constexpr (Length < Offset + sizeof(int))
		{
			static_assert(util::always_false<util::Array<char, Length>>, "The buffer is too small to contain an int.");
		}

		const util::Array<char, 4> result = Parse(value);
		dest[Offset] = result[0];
		dest[Offset + 1] = result[1];
		dest[Offset + 2] = result[2];
		dest[Offset + 3] = result[3];
	}
};

export template<> struct
util::Serializer<int[]>
{
	template<size_t Length>
	[[nodiscard]]
	static constexpr util::Array<char, Length * 4> Parse(const int(&buffer)[Length]) noexcept
	{
		static_assert(0 < Length, "The length of the array must be greater than zero.");

		return serialization::SerializeArray<Length>(buffer);
	}
};

export template<> struct
util::Serializer<unsigned int>
{
	[[nodiscard]]
	static constexpr util::Array<char, 4> Parse(const unsigned int& value) noexcept
	{
		return serialization::Serialize(value);
	}

	[[nodiscard]]
	static constexpr util::Array<char, 4> Parse(unsigned int&& value) noexcept
	{
		return serialization::Serialize(static_cast<unsigned int&&>(value));
	}
};

export template<> struct
util::Serializer<unsigned int[]>
{
	template<size_t Length>
	[[nodiscard]]
	static constexpr util::Array<char, Length * 4> Parse(const unsigned int(&buffer)[Length]) noexcept
	{
		static_assert(0 < Length, "The length of the array must be greater than zero.");

		return serialization::SerializeArray<Length>(buffer);
	}
};

export template<> struct
util::Serializer<long>
{
	[[nodiscard]]
	static constexpr util::Array<char, 4> Parse(const long& value) noexcept
	{
		return serialization::Serialize(value);
	}

	[[nodiscard]]
	static constexpr util::Array<char, 4> Parse(long&& value) noexcept
	{
		return serialization::Serialize(static_cast<long&&>(value));
	}
};

export template<> struct
util::Serializer<long[]>
{
	template<size_t Length>
	[[nodiscard]]
	static constexpr util::Array<char, Length * 4> Parse(const long(&buffer)[Length]) noexcept
	{
		static_assert(0 < Length, "The length of the array must be greater than zero.");

		return serialization::SerializeArray<Length>(buffer);
	}
};

export template<> struct
util::Serializer<unsigned long>
{
	[[nodiscard]]
	static constexpr util::Array<char, 4> Parse(const unsigned long& value) noexcept
	{
		return serialization::Serialize(value);
	}

	[[nodiscard]]
	static constexpr util::Array<char, 4> Parse(unsigned long&& value) noexcept
	{
		return serialization::Serialize(static_cast<unsigned long&&>(value));
	}
};

export template<> struct
util::Serializer<unsigned long[]>
{
	template<size_t Length>
	[[nodiscard]]
	static constexpr util::Array<char, Length * 4> Parse(const unsigned long(&buffer)[Length]) noexcept
	{
		static_assert(0 < Length, "The length of the array must be greater than zero.");

		return serialization::SerializeArray<Length>(buffer);
	}
};

export template<> struct
util::Serializer<char32_t>
{
	[[nodiscard]]
	static constexpr util::Array<char, 4> Parse(const char32_t& value) noexcept
	{
		return serialization::Serialize(value);
	}

	[[nodiscard]]
	static constexpr util::Array<char, 4> Parse(char32_t&& value) noexcept
	{
		return serialization::Serialize(static_cast<char32_t&&>(value));
	}
};

export template<> struct
util::Serializer<char32_t[]>
{
	template<size_t Length>
	[[nodiscard]]
	static constexpr util::Array<char, Length * 4> Parse(const char32_t(&buffer)[Length]) noexcept
	{
		static_assert(0 < Length, "The length of the array must be greater than zero.");

		return serialization::SerializeArray<Length>(buffer);
	}
};

export template<> struct
util::Serializer<long long>
{
	[[nodiscard]]
	static constexpr util::Array<char, 8> Parse(const long long& value) noexcept
	{
		return serialization::Serialize(value);
	}

	[[nodiscard]]
	static constexpr util::Array<char, 8> Parse(long long&& value) noexcept
	{
		return serialization::Serialize(static_cast<long long&&>(value));
	}
};

export template<> struct
util::Serializer<long long[]>
{
	template<size_t Length>
	[[nodiscard]]
	static constexpr util::Array<char, Length * 8> Parse(const long long(&buffer)[Length]) noexcept
	{
		static_assert(0 < Length, "The length of the array must be greater than zero.");

		return serialization::SerializeArray<Length>(buffer);
	}
};

export template<> struct
util::Serializer<unsigned long long>
{
	[[nodiscard]]
	static constexpr util::Array<char, 8> Parse(const unsigned long long& value) noexcept
	{
		return serialization::Serialize(value);
	}

	[[nodiscard]]
	static constexpr util::Array<char, 8> Parse(unsigned long long&& value) noexcept
	{
		return serialization::Serialize(static_cast<unsigned long long&&>(value));
	}
};

export template<> struct
util::Serializer<unsigned long long[]>
{
	template<size_t Length>
	[[nodiscard]]
	static constexpr util::Array<char, Length * 8> Parse(const unsigned long long(&buffer)[Length]) noexcept
	{
		static_assert(0 < Length, "The length of the array must be greater than zero.");

		return serialization::SerializeArray<Length>(buffer);
	}
};

export template<> struct
util::Serializer<float>
{
	[[nodiscard]]
	static constexpr util::Array<char, 4> Parse(const float& value) noexcept
	{
		return serialization::Serialize(value);
	}

	[[nodiscard]]
	static constexpr util::Array<char, 4> Parse(float&& value) noexcept
	{
		return serialization::Serialize(static_cast<float&&>(value));
	}
};

export template<> struct
util::Serializer<float[]>
{
	template<size_t Length>
	[[nodiscard]]
	static constexpr util::Array<char, Length * 4> Parse(const float(&buffer)[Length]) noexcept
	{
		static_assert(0 < Length, "The length of the array must be greater than zero.");

		return serialization::SerializeArray<Length>(buffer);
	}
};

export template<> struct
util::Serializer<double>
{
	[[nodiscard]]
	static constexpr util::Array<char, 8> Parse(const double& value) noexcept
	{
		return serialization::Serialize(value);
	}

	[[nodiscard]]
	static constexpr util::Array<char, 8> Parse(double&& value) noexcept
	{
		return serialization::Serialize(static_cast<double&&>(value));
	}
};

export template<> struct
util::Serializer<double[]>
{
	template<size_t Length>
	[[nodiscard]]
	static constexpr util::Array<char, Length * 8> Parse(const double(&buffer)[Length]) noexcept
	{
		static_assert(0 < Length, "The length of the array must be greater than zero.");

		return serialization::SerializeArray<Length>(buffer);
	}
};

export template<> struct
util::Serializer<long double>
{
	[[nodiscard]]
	static constexpr util::Array<char, 8> Parse(const long double& value) noexcept
	{
		return serialization::Serialize(value);
	}

	[[nodiscard]]
	static constexpr util::Array<char, 8> Parse(long double&& value) noexcept
	{
		return serialization::Serialize(static_cast<long double&&>(value));
	}
};

export template<> struct
util::Serializer<long double[]>
{
	template<size_t Length>
	[[nodiscard]]
	static constexpr util::Array<char, Length * 8> Parse(const long double(&buffer)[Length]) noexcept
	{
		static_assert(0 < Length, "The length of the array must be greater than zero.");

		return serialization::SerializeArray<Length>(buffer);
	}
};

#pragma warning(push, 0)
namespace util::test
{
#if true
	struct test_struct
	{
		int a;
		int b;
		int c;
	};

	struct test_struct2
	{};

	enum test_enum1
	{
		aa = 100, bb = 200, cc = 300
	};

	enum class test_enum2
	{
		aaa, bbb, ccc
	};

	enum class test_enum3 : long long
	{
		a = 8239017, b = 612014, c = 5195206
	};
}

template<>
struct util::Serializer<util::test::test_struct>
{
	[[nodiscard]]
	static constexpr util::Array<char, 12> Parse(const util::test::test_struct& value)
	//	noexcept
	{
		util::Array<char, 12> result{};

		util::Serializer<int>::ParseTo<0>(value.a, result);
		util::Serializer<int>::ParseTo<4>(value.b, result);
		util::Serializer<int>::ParseTo<8>(value.c, result);

		return result;
	}
};

namespace util::test
{
	void test_serializer() noexcept
	{
		static_assert(util::serializables<test_struct>);
		//static_assert(util::serializables<test_struct2>);

		constexpr test_struct test1{ 1, 2, 3 };
		constexpr auto rs1 = Serializer<test_struct>::Parse(test1);
		static_assert(rs1[0] == 1);

		constexpr test_struct test2{ 52534430, 68212049, 1205071 };
		constexpr auto rs2 = Serializer<test_struct>::Parse(test2);

		constexpr test_struct test3{ 255, 256, 512 };
		constexpr auto rs3 = Serializer<test_struct>::Parse(test3);

		constexpr auto rs4 = Serializer<float>::Parse(123.1023f);
		constexpr auto rs5 = Serializer<double>::Parse(0129578210.605827891059);
		constexpr auto rs6 = Serializer<int>::Parse(65535);
		constexpr auto rs7 = Serializer<int>::Parse(65536);

		constexpr auto rs8 = Serializer<char[]>::Parse("asdfgh");
		constexpr auto rs9 = Serializer<wchar_t[]>::Parse(L"asdfgh");

		constexpr auto test_str1 = serialization::Serialize("¤¡¤¤¤§");
		constexpr auto test_str2 = serialization::Serialize(L"¤¡");
		constexpr auto test_str3 = serialization::Serialize("¤·³Ä¤¡¤µ¹õÁ¹¿î¤·¤©¿õ³¯¤§¤·À¸¤Àµå¤À¤ÑÃ÷¾Ö¤ÌÃ¤¾ßÃß¾Ö¤Ì¤º´©¤Å¤Ì¤º¤§Àú¤Ã¤¸¤§");
		constexpr auto test_str4 = serialization::Serialize(L"¤·³Ä¤¡¤µ¹õÁ¹¿î¤·¤©¿õ³¯¤§¤·À¸¤Àµå¤À¤ÑÃ÷¾Ö¤ÌÃ¤¾ßÃß¾Ö¤Ì¤º´©¤Å¤Ì¤º¤§Àú¤Ã¤¸¤§");

		constexpr test_struct2 test4{};

		constexpr auto vs1 = util::Serialize(3198983);
		constexpr auto vs2 = util::Serialize(test1);
		//constexpr auto vs3 = util::Serialize(test4);

		constexpr auto es1 = util::Serialize(test_enum1::aa);
		constexpr auto es2 = util::Serialize(test_enum1::bb);
		constexpr auto es3 = util::Serialize(test_enum1::cc);

		constexpr auto es4 = util::Serialize(test_enum2::aaa);
		constexpr auto es5 = util::Serialize(test_enum2::bbb);
		constexpr auto es6 = util::Serialize(test_enum2::ccc);

		constexpr auto es7 = util::Serialize(test_enum3::a);
		constexpr auto es8 = util::Serialize(test_enum3::b);
		constexpr auto es9 = util::Serialize(test_enum3::c);

		constexpr int arr1[] = { 1, 2, 3, 4, 5 };
		constexpr float arr2[] = { 400.0591012f, 30.0951234f, 60841.0915f, 206167.013133f };
		constexpr double arr3[] = { 034909588321004938.10344953894192120303958, 40286231204.063489153 };
		constexpr test_struct arr4[] = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 } };

		constexpr auto fs1 = util::Serialize(arr1);
		constexpr auto fs2 = util::Serialize(arr2);
		constexpr auto fs3 = util::Serialize(arr3);
		constexpr auto fs4 = util::Serialize(arr4);
	}
#endif // false
}
#pragma warning(pop)
