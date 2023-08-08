export module Utility.Print;
import <cstdlib>;
import <cstdio>;
import <string>;
import <string_view>;
import <format>;

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

template<typename Char, typename ...Args>
static
void
internal_print_format(FILE* stream, const basic_format_string<Char, Args...>& fmt, Args&& ...args)
noexcept;

template<typename Char, typename ...Args>
static
void
internal_println_format(FILE* stream, const basic_format_string<Char, Args...>& fmt, Args&& ...args)
noexcept;

export namespace util
{
	template<typename ...Args>
	void Print(std::FILE* stream, format_string<Args...> fmt, Args&& ...args) noexcept
	{
		return internal_print_format(stream, fmt, std::forward<Args>(args)...);
	}

	template<typename ...Args>
	void Println(std::FILE* stream, format_string<Args...> fmt, Args&& ...args) noexcept
	{
		return internal_println_format(stream, fmt, std::forward<Args>(args)...);
	}

	template<typename ...Args>
	void Print(std::FILE* stream, format_wstring<Args...> fmt, Args&& ...args) noexcept
	{
		return internal_print_format(stream, fmt, std::forward<Args>(args)...);
	}

	template<typename ...Args>
	void Println(std::FILE* stream, format_wstring<Args...> fmt, Args&& ...args) noexcept
	{
		return internal_println_format(stream, fmt, std::forward<Args>(args)...);
	}

	template<typename ...Args>
	void Print(format_string<Args...> fmt, Args&& ...args) noexcept
	{
		return internal_print_format(stdout, fmt, std::forward<Args>(args)...);
	}

	template<typename ...Args>
	void Println(format_string<Args...> fmt, Args&& ...args) noexcept
	{
		return internal_println_format(stdout, fmt, std::forward<Args>(args)...);
	}

	template<typename ...Args>
	void Print(format_wstring<Args...> fmt, Args&& ...args) noexcept
	{
		return internal_print_format(stdout, fmt, std::forward<Args>(args)...);
	}

	template<typename ...Args>
	void Println(format_wstring<Args...> fmt, Args&& ...args) noexcept
	{
		return internal_println_format(stdout, fmt, std::forward<Args>(args)...);
	}

	namespace debug
	{
		template<typename... Args>
		void Print(std::FILE* stream, format_string<Args...> fmt, Args&&... args)
		{
#if _DEBUG
			return util::Print(stream, fmt, std::forward<Args>(args)...);
#endif
		}

		template<typename... Args>
		void Print(format_string<Args...> fmt, Args&&... args)
		{
#if _DEBUG
			return util::Print(fmt, std::forward<Args>(args)...);
#endif
		}

		template<typename... Args>
		void Println(std::FILE* stream, format_string<Args...> fmt, Args&&... args)
		{
#if _DEBUG
			return util::Println(stream, fmt, std::forward<Args>(args)...);
#endif
		}

		template<typename... Args>
		void Println(format_string<Args...> fmt, Args&&... args)
		{
#if _DEBUG
			return util::Println(stdout, fmt, std::forward<Args>(args)...);
#endif
		}

		template<typename... Args>
		void Print(std::FILE* stream, format_wstring<Args...> fmt, Args&&... args)
		{
#if _DEBUG
			return util::Print(stream, fmt, std::forward<Args>(args)...);
#endif
		}

		template<typename... Args>
		void Print(format_wstring<Args...> fmt, Args&&... args)
		{
#if _DEBUG
			return Print(stdout, fmt, std::forward<Args>(args)...);
#endif
		}

		template<typename... Args>
		void Println(std::FILE* stream, format_wstring<Args...> fmt, Args&&... args)
		{
#if _DEBUG
			return util::Println(stream, fmt, std::forward<Args>(args)...);
#endif
		}

		template<typename... Args>
		void Println(format_wstring<Args...> fmt, Args&&... args)
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
static void
internal_print_format(FILE* stream, const basic_format_string<Char, Args...>& fmt, Args&& ...args)
noexcept
{
	if constexpr (0 == sizeof...(Args))
	{
		if constexpr (std::is_same_v<Char, char>)
		{
			std::fprintf(stream, "%s", internal_fmt(fmt).data());
		}
		else if constexpr (std::is_same_v<Char, wchar_t>)
		{
			std::fwprintf(stream, L"%s", internal_fmt(fmt).data());
		}
	}
	else if constexpr (std::is_same_v<Char, char>)
	{
		std::fprintf(stream, "%s", std::vformat(internal_fmt(fmt).data(), std::make_format_args(std::forward<Args>(args)...)).data());
	}
	else if constexpr (std::is_same_v<Char, wchar_t>)
	{
		std::fwprintf(stream, L"%s", std::vformat(internal_fmt(fmt).data(), std::make_wformat_args(std::forward<Args>(args)...)).data());
	}
}

template<typename Char, typename ...Args>
static
void
internal_println_format(FILE* stream, const basic_format_string<Char, Args...>& fmt, Args&& ...args)
noexcept
{
	if constexpr (0 == sizeof...(Args))
	{
		if constexpr (std::is_same_v<Char, char>)
		{
			std::fprintf(stream, "%s\n", internal_fmt(fmt).data());
		}
		else if constexpr (std::is_same_v<Char, wchar_t>)
		{
			std::fwprintf(stream, L"%s\n", internal_fmt(fmt).data());
		}
	}
	else if constexpr (std::is_same_v<Char, char>)
	{
		std::fprintf(stream, "%s\n", std::vformat(internal_fmt(fmt).data(), std::make_format_args(std::forward<Args>(args)...)).data());
	}
	else if constexpr (std::is_same_v<Char, wchar_t>)
	{
		std::fwprintf(stream, L"%s\n", std::vformat(internal_fmt(fmt).data(), std::make_wformat_args(std::forward<Args>(args)...)).data());
	}
}
