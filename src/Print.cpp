module Utility.Print;

void
internal_vprint(std::FILE* stream, std::string_view fmt, std::format_args&& args) noexcept
{
	std::fputs(std::vformat(fmt, std::move(args)).data(), stream);
}

void
internal_vprintln(std::FILE* stream, std::string_view fmt, std::format_args&& args) noexcept
{
	std::fputs((std::vformat(fmt, std::move(args)) + '\n').data(), stream);
}

void
internal_wprint(std::FILE* stream, std::wstring_view fmt, std::wformat_args&& args) noexcept
{
	std::fputws(std::vformat(fmt, std::move(args)).data(), stream);
}

void
internal_wprintln(std::FILE* stream, std::wstring_view fmt, std::wformat_args&& args) noexcept
{
	std::fputws((std::vformat(fmt, std::move(args)) + L'\n').data(), stream);
}
