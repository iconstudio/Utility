export module Utility.Print;
import <cstdio>;
import <string>;
import <string_view>;
import <format>;

#define FORCEIN __forceinline

#if 1935 <= _MSC_VER
using std::basic_format_string;

template<typename... Args>
using format_string = std::format_string<Args...>;
template<typename... Args>
using format_wstring = std::wformat_string<Args...>;
#define internal_fmt(fmt) fmt.get()

#else
template<typename... Args>
using format_string = std::_Fmt_string<Args...>;
template<typename... Args>
using format_wstring = std::_Fmt_wstring<Args...>;

#define internal_fmt(fmt) fmt._Str
#endif

FORCEIN void internal_vprint(std::FILE* stream, std::string_view fmt, std::format_args&& args) noexcept;

FORCEIN void internal_vprintln(std::FILE* stream, std::string_view fmt, std::format_args&& args) noexcept;

FORCEIN void internal_wprint(std::FILE* stream, std::wstring_view fmt, std::wformat_args&& args) noexcept;

FORCEIN void internal_wprintln(std::FILE* stream, std::wstring_view fmt, std::wformat_args&& args) noexcept;

template<typename Char, typename ...Args>
static FORCEIN
void
internal_print_format(FILE* stream, const basic_format_string<Char, Args...>& fmt, Args&& ...args)
noexcept;

template<typename Char, typename ...Args>
static FORCEIN
void
internal_println_format(FILE* stream, const basic_format_string<Char, Args...>& fmt, Args&& ...args)
noexcept;

export namespace util
{
	template<typename ...Args>
	FORCEIN
		void Print(std::FILE* stream, format_string<Args...> fmt, Args&& ...args) noexcept
	{
		return internal_print_format(stream, fmt, std::forward<Args>(args)...);
	}

	template<typename ...Args>
	FORCEIN
		void Println(std::FILE* stream, format_string<Args...> fmt, Args&& ...args) noexcept
	{
		return internal_println_format(stream, fmt, std::forward<Args>(args)...);
	}

	template<typename ...Args>
	FORCEIN
		void Print(std::FILE* stream, format_wstring<Args...> fmt, Args&& ...args) noexcept
	{
		return internal_print_format(stream, fmt, std::forward<Args>(args)...);
	}

	template<typename ...Args>
	FORCEIN
		void Println(std::FILE* stream, format_wstring<Args...> fmt, Args&& ...args) noexcept
	{
		return internal_println_format(stream, fmt, std::forward<Args>(args)...);
	}

	template<typename ...Args>
	FORCEIN
		void Print(format_string<Args...> fmt, Args&& ...args) noexcept
	{
		return internal_print_format(stdout, fmt, std::forward<Args>(args)...);
	}

	template<typename ...Args>
	FORCEIN
		void Println(format_string<Args...> fmt, Args&& ...args) noexcept
	{
		return internal_println_format(stdout, fmt, std::forward<Args>(args)...);
	}

	template<typename ...Args>
	FORCEIN
		void Print(format_wstring<Args...> fmt, Args&& ...args) noexcept
	{
		return internal_print_format(stdout, fmt, std::forward<Args>(args)...);
	}

	template<typename ...Args>
	FORCEIN
		void Println(format_wstring<Args...> fmt, Args&& ...args) noexcept
	{
		return internal_println_format(stdout, fmt, std::forward<Args>(args)...);
	}

	namespace debug
	{
		template<typename... Args>
		inline void Print(std::FILE* stream, format_string<Args...> fmt, Args&&... args)
		{
#if _DEBUG
			return util::Print(stream, fmt, std::forward<Args>(args)...);
#endif
		}

		template<typename... Args>
		inline void Print(format_string<Args...> fmt, Args&&... args)
		{
#if _DEBUG
			return util::Print(fmt, std::forward<Args>(args)...);
#endif
		}

		template<typename... Args>
		inline void Println(std::FILE* stream, format_string<Args...> fmt, Args&&... args)
		{
#if _DEBUG
			return util::Println(stream, fmt, std::forward<Args>(args)...);
#endif
		}

		template<typename... Args>
		inline void Println(format_string<Args...> fmt, Args&&... args)
		{
#if _DEBUG
			return util::Println(stdout, fmt, std::forward<Args>(args)...);
#endif
		}

		template<typename... Args>
		inline void Print(std::FILE* stream, format_wstring<Args...> fmt, Args&&... args)
		{
#if _DEBUG
			return util::Print(stream, fmt, std::forward<Args>(args)...);
#endif
		}

		template<typename... Args>
		inline void Print(format_wstring<Args...> fmt, Args&&... args)
		{
#if _DEBUG
			return Print(stdout, fmt, std::forward<Args>(args)...);
#endif
		}

		template<typename... Args>
		inline void Println(std::FILE* stream, format_wstring<Args...> fmt, Args&&... args)
		{
#if _DEBUG
			return util::Println(stream, fmt, std::forward<Args>(args)...);
#endif
		}

		template<typename... Args>
		inline void Println(format_wstring<Args...> fmt, Args&&... args)
		{
#if _DEBUG
			return Println(stdout, fmt, std::forward<Args>(args)...);
#endif
		}
	}
}

#pragma warning(push, 1)
namespace util::test
{
	static void test_print()
	{
		Print("Hello, world!\n", 0);
		Print("Hello, {}!\n", "world");

		Print("Hello, {}!\n", std::string("world"));
		Print("Hello, {}!\n", std::string_view("world"));
		Print(L"Hello, {}!\n", std::wstring(L"world"));
		Print(L"Hello, {}!\n", std::wstring_view(L"world"));

		Println("Hello, world!");
		Println("Hello, {}!", "world");
		Println("Hello, {}!", std::string("world"));
		Println("Hello, {}!", std::string_view("world"));
		Println(L"Hello, {}!", std::wstring(L"world"));
		Println(L"Hello, {}!", std::wstring_view(L"world"));
	}
}
#pragma warning(pop)

template<typename Char, typename ...Args>
static FORCEIN void
internal_print_format(FILE* stream, const basic_format_string<Char, Args...>& fmt, Args&& ...args)
noexcept
{
	if constexpr (0 == sizeof...(Args))
	{
		if constexpr (std::is_same_v<Char, char>)
		{
			std::fputs(internal_fmt(fmt).data(), stream);
		}
		else if constexpr (std::is_same_v<Char, wchar_t>)
		{
			std::fputws(internal_fmt(fmt).data(), stream);
		}
	}
	else if constexpr (std::is_same_v<Char, char>)
	{
		internal_vprint(stream, internal_fmt(fmt), std::make_format_args(std::forward<Args>(args)...));
	}
	else if constexpr (std::is_same_v<Char, wchar_t>)
	{
		internal_wprint(stream, internal_fmt(fmt), std::make_wformat_args(std::forward<Args>(args)...));
	}
}

template<typename Char, typename ...Args>
static FORCEIN
void
internal_println_format(FILE* stream, const basic_format_string<Char, Args...>& fmt, Args&& ...args)
noexcept
{
	if constexpr (0 == sizeof...(Args))
	{
		if constexpr (std::is_same_v<Char, char>)
		{
#if _DEBUG
			std::string_view str = internal_fmt(fmt);
			//std::string buffer = std::string{ str.cbegin(), str.cend() } + '\n';

			std::fputs(str.data(), stream);
			std::fputs("\n", stream);
#else // _DEBUG
			std::fputs(internal_fmt(fmt).data(), stream);
			std::fputs("\n", stream);
#endif // !_DEBUG
		}
		else if constexpr (std::is_same_v<Char, wchar_t>)
		{
#if _DEBUG
			std::wstring_view str = internal_fmt(fmt);
			//std::wstring buffer = std::wstring{ str.cbegin(), str.cend() } + L'\n';

			std::fputws(str.data(), stream);
			std::fputws(L"\n", stream);
#else // _DEBUG
			std::fputws(internal_fmt(fmt).data(), stream);
			std::fputws(L"\n", stream);
#endif // !_DEBUG
		}
	}
	else if constexpr (std::is_same_v<Char, char>)
	{
		return internal_vprintln(stream, internal_fmt(fmt), std::make_format_args(std::forward<Args>(args)...));
	}
	else if constexpr (std::is_same_v<Char, wchar_t>)
	{
		return internal_wprintln(stream, internal_fmt(fmt), std::make_wformat_args(std::forward<Args>(args)...));
	}
}

FORCEIN
void
internal_vprint(std::FILE* stream, std::string_view fmt, std::format_args&& args) noexcept
{
	std::fputs(std::vformat(fmt, std::move(args)).data(), stream);
}

FORCEIN
void
internal_vprintln(std::FILE* stream, std::string_view fmt, std::format_args&& args) noexcept
{
	std::fputs((std::vformat(fmt, std::move(args)) + '\n').data(), stream);
}

FORCEIN
void
internal_wprint(std::FILE* stream, std::wstring_view fmt, std::wformat_args&& args) noexcept
{
	std::fputws(std::vformat(fmt, std::move(args)).data(), stream);
}

FORCEIN
void
internal_wprintln(std::FILE* stream, std::wstring_view fmt, std::wformat_args&& args) noexcept
{
	std::fputws((std::vformat(fmt, std::move(args)) + L'\n').data(), stream);
}
