module;
#include <string>
#include <string_view>
#include <format>

export module Utility.Print;
import Utility.Concurrency.Locks;

#if 1935 <= _MSC_VER
using std::basic_format_string;

template<typename... Args>
using fmt_str = std::format_string<Args...>;
template<typename... Args>
using fmt_wtr = std::wformat_string<Args...>;

template<typename Char, typename... Args>
static
constexpr
std::basic_string_view<Char>
internal_fmt(basic_format_string<Char, Args...> fmt) noexcept
{
	return fmt.get();
}
#else
template<typename... Args>
using fmt_str = std::_Fmt_string<Args...>;
template<typename... Args>
using fmt_wtr = std::_Fmt_wstring<Args...>;

#define internal_fmt(fmt) fmt._Str
#endif

#define FORMAT_FN void
#if _DEBUG
#define FORMAT_DEBG_FN void
#define ON_DEBUG(statement) statement
#else // _DEBUG
#define FORMAT_DEBG_FN constexpr void
#define ON_DEBUG(statement)
#endif // !_DEBUG

static util::mutex* syncedIo{};

template<typename Char, typename ...Args>
void
internal_print_format(std::FILE* stream, const basic_format_string<Char, Args...>& fmt, Args&& ...args)
noexcept;

template<typename Char, typename ...Args>
void
internal_println_format(std::FILE* stream, const basic_format_string<Char, Args...>& fmt, Args&& ...args)
noexcept;

template<typename Char, typename ...Args>
void
internal_sync_print_format(std::FILE* stream, const basic_format_string<Char, Args...>& fmt, Args&& ...args)
noexcept;

template<typename Char, typename ...Args>
void
internal_sync_println_format(std::FILE* stream, const basic_format_string<Char, Args...>& fmt, Args&& ...args)
noexcept;

export namespace util
{
#pragma region Declarations
	template<typename ...Args>
	FORMAT_FN Print(std::FILE* stream, fmt_str<Args...> fmt, Args&& ...args) noexcept;

	template<typename ...Args>
	FORMAT_FN Println(std::FILE* stream, fmt_str<Args...> fmt, Args&& ...args) noexcept;

	template<typename ...Args>
	FORMAT_FN Print(std::FILE* stream, fmt_wtr<Args...> fmt, Args&& ...args) noexcept;

	template<typename ...Args>
	FORMAT_FN Println(std::FILE* stream, fmt_wtr<Args...> fmt, Args&& ...args) noexcept;

	template<typename ...Args>
	FORMAT_FN Print(fmt_str<Args...> fmt, Args&& ...args) noexcept;

	template<typename ...Args>
	FORMAT_FN Println(fmt_str<Args...> fmt, Args&& ...args) noexcept;

	template<typename ...Args>
	FORMAT_FN Print(fmt_wtr<Args...> fmt, Args&& ...args) noexcept;

	template<typename ...Args>
	FORMAT_FN Println(fmt_wtr<Args...> fmt, Args&& ...args) noexcept;

	template<typename... Args>
	FORMAT_FN PrintSynced(std::FILE* stream, fmt_str<Args...> fmt, Args&& ...args) noexcept;

	template<typename... Args>
	FORMAT_FN PrintlnSynced(std::FILE* stream, fmt_str<Args...> fmt, Args&& ...args) noexcept;

	template<typename... Args>
	FORMAT_FN PrintSynced(std::FILE* stream, fmt_wtr<Args...> fmt, Args&& ...args) noexcept;

	template<typename... Args>
	FORMAT_FN PrintlnSynced(std::FILE* stream, fmt_wtr<Args...> fmt, Args&& ...args) noexcept;

	template<typename... Args>
	FORMAT_FN PrintSynced(fmt_str<Args...> fmt, Args&& ...args) noexcept;

	template<typename... Args>
	FORMAT_FN PrintlnSynced(fmt_str<Args...> fmt, Args&& ...args) noexcept;

	template<typename... Args>
	FORMAT_FN PrintSynced(fmt_wtr<Args...> fmt, Args&& ...args) noexcept;

	template<typename... Args>
	FORMAT_FN PrintlnSynced(fmt_wtr<Args...> fmt, Args&& ...args) noexcept;
#pragma endregion

	namespace debug
	{
#pragma region Debug Declarations
		template<typename... Args>
		FORMAT_DEBG_FN Print(std::FILE* stream, fmt_str<Args...> fmt, Args&&... args)
		{
			ON_DEBUG(util::Print(stream, fmt, std::forward<Args>(args)...));
		}

		template<typename... Args>
		FORMAT_DEBG_FN Print(fmt_str<Args...> fmt, Args&&... args)
		{
			ON_DEBUG(util::Print(fmt, std::forward<Args>(args)...));
		}

		template<typename... Args>
		FORMAT_DEBG_FN Println(std::FILE* stream, fmt_str<Args...> fmt, Args&&... args)
		{
			ON_DEBUG(util::Println(stream, fmt, std::forward<Args>(args)...));
		}

		template<typename... Args>
		FORMAT_DEBG_FN Println(fmt_str<Args...> fmt, Args&&... args)
		{
			ON_DEBUG(util::Println(stdout, fmt, std::forward<Args>(args)...));
		}

		template<typename... Args>
		FORMAT_DEBG_FN Print(std::FILE* stream, fmt_wtr<Args...> fmt, Args&&... args)
		{
			ON_DEBUG(util::Print(stream, fmt, std::forward<Args>(args)...));
		}

		template<typename... Args>
		FORMAT_DEBG_FN Print(fmt_wtr<Args...> fmt, Args&&... args)
		{
			ON_DEBUG(util::Print(stdout, fmt, std::forward<Args>(args)...));
		}

		template<typename... Args>
		FORMAT_DEBG_FN Println(std::FILE* stream, fmt_wtr<Args...> fmt, Args&&... args)
		{
			ON_DEBUG(util::Println(stream, fmt, std::forward<Args>(args)...));
		}

		template<typename... Args>
		FORMAT_DEBG_FN Println(fmt_wtr<Args...> fmt, Args&&... args)
		{
			ON_DEBUG(util::Println(stdout, fmt, std::forward<Args>(args)...));
		}

		template<typename... Args>
		FORMAT_DEBG_FN PrintSynced(std::FILE* stream, fmt_str<Args...> fmt, Args&&... args)
		{
			ON_DEBUG(util::PrintSynced(stream, fmt, std::forward<Args>(args)...));
		}

		template<typename... Args>
		FORMAT_DEBG_FN PrintSynced(fmt_str<Args...> fmt, Args&&... args)
		{
			ON_DEBUG(util::PrintSynced(fmt, std::forward<Args>(args)...));
		}

		template<typename... Args>
		FORMAT_DEBG_FN PrintlnSynced(std::FILE* stream, fmt_str<Args...> fmt, Args&&... args)
		{
			ON_DEBUG(util::PrintlnSynced(stream, fmt, std::forward<Args>(args)...));
		}

		template<typename... Args>
		FORMAT_DEBG_FN PrintlnSynced(fmt_str<Args...> fmt, Args&&... args)
		{
			ON_DEBUG(util::PrintlnSynced(stdout, fmt, std::forward<Args>(args)...));
		}

		template<typename... Args>
		FORMAT_DEBG_FN PrintSynced(std::FILE* stream, fmt_wtr<Args...> fmt, Args&&... args)
		{
			ON_DEBUG(util::PrintSynced(stream, fmt, std::forward<Args>(args)...));
		}

		template<typename... Args>
		FORMAT_DEBG_FN PrintSynced(fmt_wtr<Args...> fmt, Args&&... args)
		{
			ON_DEBUG(util::PrintSynced(stdout, fmt, std::forward<Args>(args)...));
		}

		template<typename... Args>
		FORMAT_DEBG_FN PrintlnSynced(std::FILE* stream, fmt_wtr<Args...> fmt, Args&&... args)
		{
			ON_DEBUG(util::PrintlnSynced(stream, fmt, std::forward<Args>(args)...));
		}

		template<typename... Args>
		FORMAT_DEBG_FN PrintlnSynced(fmt_wtr<Args...> fmt, Args&&... args)
		{
			ON_DEBUG(util::PrintlnSynced(stdout, fmt, std::forward<Args>(args)...));
		}
#pragma endregion
	}
}

template<typename ...Args>
FORMAT_FN
util::Print(std::FILE* stream, fmt_str<Args...> fmt, Args && ...args)
noexcept
{
	return internal_print_format(stream, fmt, std::forward<Args>(args)...);
}

template<typename ...Args>
FORMAT_FN
util::Println(std::FILE* stream, fmt_str<Args...> fmt, Args && ...args)
noexcept
{
	return internal_println_format(stream, fmt, std::forward<Args>(args)...);
}

template<typename ...Args>
FORMAT_FN
util::Print(std::FILE* stream, fmt_wtr<Args...> fmt, Args && ...args)
noexcept
{
	return internal_print_format(stream, fmt, std::forward<Args>(args)...);
}

template<typename ...Args>
FORMAT_FN
util::Println(std::FILE* stream, fmt_wtr<Args...> fmt, Args && ...args)
noexcept
{
	return internal_println_format(stream, fmt, std::forward<Args>(args)...);
}

template<typename ...Args>
FORMAT_FN
util::Print(fmt_str<Args...> fmt, Args && ...args)
noexcept
{
	return internal_print_format(stdout, fmt, std::forward<Args>(args)...);
}

template<typename ...Args>
FORMAT_FN
util::Println(fmt_str<Args...> fmt, Args && ...args)
noexcept
{
	return internal_println_format(stdout, fmt, std::forward<Args>(args)...);
}

template<typename ...Args>
FORMAT_FN
util::Print(fmt_wtr<Args...> fmt, Args && ...args)
noexcept
{
	return internal_print_format(stdout, fmt, std::forward<Args>(args)...);
}

template<typename ...Args>
FORMAT_FN
util::Println(fmt_wtr<Args...> fmt, Args && ...args)
noexcept
{
	return internal_println_format(stdout, fmt, std::forward<Args>(args)...);
}

template<typename ...Args>
FORMAT_FN
util::PrintSynced(std::FILE* stream, fmt_str<Args...> fmt, Args && ...args)
noexcept
{
	return internal_sync_print_format(stream, fmt, std::forward<Args>(args)...);
}

template<typename ...Args>
FORMAT_FN
util::PrintlnSynced(std::FILE* stream, fmt_str<Args...> fmt, Args && ...args)
noexcept
{
	return internal_sync_println_format(stream, fmt, std::forward<Args>(args)...);
}

template<typename ...Args>
FORMAT_FN
util::PrintSynced(std::FILE* stream, fmt_wtr<Args...> fmt, Args && ...args)
noexcept
{
	return internal_sync_print_format(stream, fmt, std::forward<Args>(args)...);
}

template<typename ...Args>
FORMAT_FN
util::PrintlnSynced(std::FILE* stream, fmt_wtr<Args...> fmt, Args && ...args)
noexcept
{
	return internal_sync_println_format(stream, fmt, std::forward<Args>(args)...);
}

template<typename ...Args>
FORMAT_FN
util::PrintSynced(fmt_str<Args...> fmt, Args && ...args)
noexcept
{
	return internal_sync_print_format(stdout, fmt, std::forward<Args>(args)...);
}

template<typename ...Args>
FORMAT_FN
util::PrintlnSynced(fmt_str<Args...> fmt, Args && ...args)
noexcept
{
	return internal_sync_println_format(stdout, fmt, std::forward<Args>(args)...);
}

template<typename ...Args>
FORMAT_FN
util::PrintSynced(fmt_wtr<Args...> fmt, Args && ...args)
noexcept
{
	return internal_sync_print_format(stdout, fmt, std::forward<Args>(args)...);
}

template<typename ...Args>
FORMAT_FN
util::PrintlnSynced(fmt_wtr<Args...> fmt, Args && ...args)
noexcept
{
	return internal_sync_println_format(stdout, fmt, std::forward<Args>(args)...);
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

		PrintSynced("Hello, world!\n");
		PrintSynced("Hello, {}!\n", "world");
		PrintSynced("Hello, {}!\n", std::string("world"));
		PrintSynced("Hello, {}!\n", std::string_view("world"));
		PrintSynced(L"Hello, {}!\n", std::wstring(L"world"));
		PrintSynced(L"Hello, World!\n");
	}
}
#pragma warning(pop)

void internal_vprint(std::FILE* stream, std::string_view fmt, std::format_args&& args) noexcept;

void internal_vprintln(std::FILE* stream, std::string_view fmt, std::format_args&& args) noexcept;

void internal_wprint(std::FILE* stream, std::wstring_view fmt, std::wformat_args&& args) noexcept;

void internal_wprintln(std::FILE* stream, std::wstring_view fmt, std::wformat_args&& args) noexcept;

template<typename Char, typename ...Args>
void
internal_print_format(std::FILE* stream, const basic_format_string<Char, Args...>& fmt, Args&& ...args)
noexcept
{
	if constexpr (0 == sizeof...(Args))
	{
		if constexpr (std::is_same_v<Char, char>)
		{
			std::fputs(fmt.get().data(), stream);
		}
		else if constexpr (std::is_same_v<Char, wchar_t>)
		{
			std::fputws(fmt.get().data(), stream);
		}
	}
	else if constexpr (std::is_same_v<Char, char>)
	{
		internal_vprint(stream, fmt.get(), std::make_format_args(std::forward<Args>(args)...));
	}
	else if constexpr (std::is_same_v<Char, wchar_t>)
	{
		internal_wprint(stream, fmt.get(), std::make_wformat_args(std::forward<Args>(args)...));
	}
}

template<typename Char, typename ...Args>
void
internal_println_format(std::FILE* stream, const basic_format_string<Char, Args...>& fmt, Args&& ...args)
noexcept
{
	if constexpr (0 == sizeof...(Args))
	{
		if constexpr (std::is_same_v<Char, char>)
		{
#if _DEBUG
			std::string_view str = fmt.get();
			//std::string buffer = std::string{ str.cbegin(), str.cend() } + '\n';

			std::fputs(str.data(), stream);
			std::fputs("\n", stream);
#else // _DEBUG
			std::fputs(fmt.get().data(), stream);
			std::fputs("\n", stream);
#endif // !_DEBUG
		}
		else if constexpr (std::is_same_v<Char, wchar_t>)
		{
#if _DEBUG
			std::wstring_view str = fmt.get();
			//std::wstring buffer = std::wstring{ str.cbegin(), str.cend() } + L'\n';

			std::fputws(str.data(), stream);
			std::fputws(L"\n", stream);
#else // _DEBUG
			std::fputws(fmt.get().data(), stream);
			std::fputws(L"\n", stream);
#endif // !_DEBUG
		}
	}
	else if constexpr (std::is_same_v<Char, char>)
	{
		return internal_vprintln(stream, fmt.get(), std::make_format_args(std::forward<Args>(args)...));
	}
	else if constexpr (std::is_same_v<Char, wchar_t>)
	{
		return internal_wprintln(stream, fmt.get(), std::make_wformat_args(std::forward<Args>(args)...));
	}
}

template<typename Char, typename ...Args>
void
internal_sync_print_format(std::FILE* stream, const basic_format_string<Char, Args...>& fmt, Args&& ...args)
noexcept
{
	if constexpr (0 == sizeof...(Args))
	{
		if constexpr (std::is_same_v<Char, char>)
		{
			std::string_view buffer = fmt.get();

			util::lock_guard guard{ *syncedIo };
			std::fputs(buffer.data(), stream);
		}
		else if constexpr (std::is_same_v<Char, wchar_t>)
		{
			std::wstring_view buffer = fmt.get();

			util::lock_guard guard{ *syncedIo };
			std::fputws(buffer.data(), stream);
		}
	}
	else if constexpr (std::is_same_v<Char, char>)
	{
		std::string_view buff = fmt.get();
		std::format_args pack = std::make_format_args(std::forward<Args>(args)...);

		util::lock_guard guard{ *syncedIo };
		return internal_vprint(stream, buff, std::move(pack));
	}
	else if constexpr (std::is_same_v<Char, wchar_t>)
	{
		std::wstring_view buff = fmt.get();
		std::wformat_args pack = std::make_wformat_args(std::forward<Args>(args)...);

		util::lock_guard guard{ *syncedIo };
		return internal_wprint(stream, buff, std::move(pack));
	}
}

template<typename Char, typename ...Args>
void
internal_sync_println_format(std::FILE* stream, const basic_format_string<Char, Args...>& fmt, Args && ...args) noexcept
{
	if constexpr (0 == sizeof...(Args))
	{
		if constexpr (std::is_same_v<Char, char>)
		{
			std::string_view fwd = fmt.get();
			const std::string& buffer = std::string{ fwd.cbegin(), fwd.cend() } + '\n';

			util::lock_guard guard{ *syncedIo };
			std::fputs(buffer.data(), stream);
		}
		else if constexpr (std::is_same_v<Char, wchar_t>)
		{
			std::wstring_view fwd = fmt.get();
			const std::wstring& buffer = std::wstring{ fwd.cbegin(), fwd.cend() } + L'\n';

			util::lock_guard guard{ *syncedIo };
			std::fputws(buffer.data(), stream);
		}
	}
	else if constexpr (std::is_same_v<Char, char>)
	{
		std::string_view buff = fmt.get();
		std::format_args pack = std::make_format_args(std::forward<Args>(args)...);

		util::lock_guard guard{ *syncedIo };
		return internal_vprintln(stream, buff, std::move(pack));
	}
	else if constexpr (std::is_same_v<Char, wchar_t>)
	{
		std::wstring_view buff = fmt.get();
		std::wformat_args pack = std::make_wformat_args(std::forward<Args>(args)...);

		util::lock_guard guard{ *syncedIo };
		return internal_wprintln(stream, buff, std::move(pack));
	}
}
