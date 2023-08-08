module;
#include <exception>
#include <system_error>
export module Utility.Error;

export namespace util::err
{
	[[noreturn]]
	inline void RaiseSystemError() noexcept(false)
	{
		throw std::system_error(std::error_code(errno, std::system_category()));
	}

	[[noreturn]]
	inline void RaiseSystemError(const int& error_code) noexcept(false)
	{
		throw std::system_error(std::error_code(error_code, std::system_category()));
	}

	[[noreturn]]
	inline void RaiseSystemError(const std::error_code& error_code) noexcept(false)
	{
		throw std::system_error(error_code);
	}

	[[noreturn]]
	inline void RaiseSystemError(std::error_code&& error_code) noexcept(false)
	{
		throw std::system_error(std::move(error_code));
	}

	[[noreturn]]
	inline void RaiseSystemError(const std::errc& error_code) noexcept(false)
	{
		throw std::system_error(make_error_code(error_code));
	}

	[[noreturn]]
	inline void RaiseSystemError(std::errc&& error_code) noexcept(false)
	{
		throw std::system_error(make_error_code(std::move(error_code)));
	}

	[[noreturn]]
	inline void RaiseSystemError(const std::errc& error_code, std::string_view message) noexcept(false)
	{
		throw std::system_error{ make_error_code(error_code), message.data() };
	}

	[[noreturn]]
	inline void RaiseSystemError(std::errc&& error_code, std::string_view message) noexcept(false)
	{
		throw std::system_error{ make_error_code(std::move(error_code)), message.data() };
	}
}
