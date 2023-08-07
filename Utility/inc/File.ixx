export module Utility.IO.File;
import <cstdint>;
import <cstdio>;
import <span>;
import <string>;
import <string_view>;
import <filesystem>;

export namespace util::io
{
	using FilePath = std::filesystem::path;

	namespace file
	{
		enum [[nodiscard]] ResultCode : std::int32_t
		{
			None = 0,
			Success = 0,
			EndOfFile = -1,
			Error = -2,
			ErrorOnBuffer = -3,
			ErrorOnRead = -4,
			ErrorOnWrite = -5,
			FileNotFound = -1000,
		};

		enum class [[nodiscard]] OpenModes : std::int32_t
		{
			Read = 0b0000,
			Write = 0b0001,
			Append = 0b0010,
			Binary = 0b0100,
		};

		namespace open_mode_literals
		{
			inline constexpr std::string_view Read = "r+";
			inline constexpr std::string_view Write = "w+";
			inline constexpr std::string_view Open = "a+";
			inline constexpr std::string_view BinaryRead = "rb+";
			inline constexpr std::string_view BinaryWrite = "wb+";
			inline constexpr std::string_view BinaryOpen = "ab+";
		}

		inline constexpr std::string_view ModeStrings[] =
		{
			"r+", // 0
			"w+", // 1
			"a+", // 2
			"a+", // 3
			"rb+", // 4
			"wb+", // 5
			"ab+", // 6
			"ab+", // 6
		};
	}

	class [[nodiscard]] File
	{
	public:
		File() noexcept = default;

		File(const FilePath& filepath, file::OpenModes mode) noexcept
			: File(filepath.string(), file::ModeStrings[static_cast<std::size_t>(mode)])
		{}

		File(std::string_view filepath, file::OpenModes mode) noexcept
			: File(filepath, file::ModeStrings[static_cast<std::size_t>(mode)])
		{}

		File(std::string_view filepath, std::string_view mode) noexcept
		{
			Open(filepath, mode);
		}

		~File() noexcept
		{
			Close();
		}

		bool Open(const FilePath& filename, file::OpenModes mode) noexcept
		{
			return Open(filename.string(), mode);
		}

		bool Open(const FilePath& filename) noexcept
		{
			return Open(filename.string(), "rb");
		}

		bool Open(std::string_view filename, std::string_view mode) noexcept
		{
			::errno_t err = ::fopen_s(std::addressof(myHandle), filename.data(), mode.data());
			lastError = err;

			if (0 != err)
			{
				lastResult = file::ResultCode::FileNotFound;
				return false;
			}

			lastResult = file::ResultCode::Success;
			return true;
		}

		void Close() noexcept
		{
			if (myHandle)
			{
				std::fclose(myHandle);
				myHandle = nullptr;
			}
		}

		[[nodiscard]]
		std::string Contents() const noexcept
		{
			if (nullptr == myHandle)
			{
				lastError = 0;
				lastResult = file::ResultCode::FileNotFound;
				return {};
			}

			std::string buffer;
			std::size_t size;
			try
			{
				size = GetSize();
				buffer.resize(size);
			}
			catch (...)
			{
				lastError = errno;
				lastResult = file::ResultCode::ErrorOnBuffer;
				return {};
			}

			try
			{
				std::fread(buffer.data(), 1, size, myHandle);
			}
			catch (...)
			{
				lastError = errno;
				lastResult = file::ResultCode::ErrorOnRead;
				return {};
			}

			return buffer;
		}

		[[nodiscard]]
		std::string ReadLine() const noexcept
		{
			std::string result = Read('\n');

			try
			{
				result.shrink_to_fit();
			}
			catch (...)
			{
				lastError = errno;
				lastResult = file::ResultCode::Error;
				return {};
			}

			return result;
		}

		std::string Read(char delimiter = '\0') const noexcept
		{
			return Read(std::to_string(delimiter));
		}

		std::string Read(std::string_view delimiter_str) const noexcept
		{
			if (nullptr == myHandle)
			{
				lastError = 0;
				lastResult = file::ResultCode::FileNotFound;
				return {};
			}

			std::string buffer;
			try
			{
				buffer.reserve(128);
			}
			catch (...)
			{
				lastError = errno;
				lastResult = file::ResultCode::ErrorOnBuffer;
				return {};
			}

			try
			{
				while (true)
				{
					const int result = std::fgetc(myHandle);
					if (result == file::ResultCode::EndOfFile)
					{
						break;
					}

					buffer.push_back(static_cast<char>(result));
					if (buffer.ends_with(delimiter_str))
					{
						break;
					}
				}
			}
			catch (...)
			{
				lastError = errno;
				lastResult = file::ResultCode::ErrorOnRead;
				return {};
			}

			return buffer;
		}

		std::size_t Read(std::string& target_buffer, const std::size_t& size) const noexcept
		{
			if (nullptr == myHandle)
			{
				lastError = 0;
				lastResult = file::ResultCode::FileNotFound;
				return 0;
			}

			try
			{
				target_buffer.resize(size);
			}
			catch (...)
			{
				lastError = errno;
				lastResult = file::ResultCode::ErrorOnBuffer;
				return 0;
			}

			return Read(target_buffer.data(), size);
		}

		std::size_t Read(std::string& target_buffer, char delimiter = '\0') const noexcept
		{
			target_buffer = Read(delimiter);

			return target_buffer.size();
		}

		std::size_t Read(void* const& buffer, const std::size_t& size) const noexcept
		{
			if (nullptr == myHandle)
			{
				lastError = 0;
				lastResult = file::ResultCode::FileNotFound;
				return 0;
			}

			size_t read_bytes = 0;
			try
			{
				read_bytes = std::fread(buffer, 1, size, myHandle);
			}
			catch (...)
			{
				lastError = errno;
				lastResult = file::ResultCode::ErrorOnRead;
				return 0;
			}

			return read_bytes;
		}

		std::size_t Write(std::string_view content) const noexcept
		{
			if (nullptr == myHandle)
			{
				lastError = 0;
				lastResult = file::ResultCode::FileNotFound;
				return 0;
			}

			int length = std::fputs(content.data(), myHandle);
			if (length == file::ResultCode::EndOfFile)
			{
				lastError = errno;
				lastResult = file::ResultCode::ErrorOnWrite;
				return 0;
			}

			return length;
		}

		template<typename T, std::size_t Extent>
		std::size_t Write(std::span<T, Extent> content) const noexcept
		{
			if (nullptr == myHandle)
			{
				lastError = 0;
				lastResult = file::ResultCode::FileNotFound;
				return 0;
			}

			const std::size_t target_count = content.size();
			std::size_t count = std::fwrite(content.data(), sizeof(T), target_count, myHandle);
			if (count != target_count)
			{
				lastError = errno;
				lastResult = file::ResultCode::ErrorOnWrite;
			}

			return count;
		}

		void Rewind() const noexcept
		{
			std::rewind(myHandle);
		}

		[[nodiscard]]
		std::size_t GetSize() const noexcept
		{
			if (nullptr == myHandle)
			{
				return std::size_t{};
			}

			const std::int32_t current = std::ftell(myHandle);
			std::fseek(myHandle, 0, SEEK_END);
			const std::int32_t size = std::ftell(myHandle);
			std::fseek(myHandle, current, SEEK_SET);

			return static_cast<size_t>(size);
		}

		[[nodiscard]]
		file::ResultCode GetLastResult() const noexcept
		{
			return lastResult;
		}

		[[nodiscard]]
		bool IsEmpty() const noexcept
		{
			return myHandle == nullptr;
		}

		[[nodiscard]]
		bool IsOpen() const noexcept
		{
			return myHandle != nullptr;
		}

		[[nodiscard]]
		bool IsEndOfFile() const noexcept
		{
			return std::feof(myHandle) != 0;
		}

	private:
		std::FILE* myHandle = nullptr;

		mutable file::ResultCode lastResult = file::ResultCode::None;
		mutable ::errno_t lastError = 0;
	};

	[[nodiscard]]
	constexpr file::OpenModes operator|(file::OpenModes lhs, file::OpenModes rhs) noexcept
	{
		return static_cast<file::OpenModes>(static_cast<std::int32_t>(lhs) | static_cast<std::int32_t>(rhs));
	}

	[[nodiscard]]
	constexpr file::OpenModes operator&(file::OpenModes lhs, file::OpenModes rhs) noexcept
	{
		return static_cast<file::OpenModes>(static_cast<std::int32_t>(lhs) & static_cast<std::int32_t>(rhs));
	}

	[[nodiscard]]
	constexpr file::OpenModes operator^(file::OpenModes lhs, file::OpenModes rhs) noexcept
	{
		return static_cast<file::OpenModes>(static_cast<std::int32_t>(lhs) ^ static_cast<std::int32_t>(rhs));
	}

	[[nodiscard]]
	constexpr file::OpenModes operator~(file::OpenModes rhs) noexcept
	{
		return static_cast<file::OpenModes>(~static_cast<std::int32_t>(rhs));
	}

	constexpr file::OpenModes& operator|=(file::OpenModes& lhs, const file::OpenModes& rhs) noexcept
	{
		return lhs = static_cast<file::OpenModes>(static_cast<std::int32_t>(lhs) | static_cast<std::int32_t>(rhs));
	}

	constexpr file::OpenModes& operator&=(file::OpenModes& lhs, const file::OpenModes& rhs) noexcept
	{
		return lhs = static_cast<file::OpenModes>(static_cast<std::int32_t>(lhs) & static_cast<std::int32_t>(rhs));
	}

	constexpr file::OpenModes& operator^=(file::OpenModes& lhs, const file::OpenModes& rhs) noexcept
	{
		return lhs = static_cast<file::OpenModes>(static_cast<std::int32_t>(lhs) ^ static_cast<std::int32_t>(rhs));
	}

	[[nodiscard]]
	constexpr bool HasMode(const file::OpenModes& lhs, const file::OpenModes& rhs) noexcept
	{
		return (static_cast<std::int32_t>(lhs) & static_cast<std::int32_t>(rhs)) != 0;
	}
}
