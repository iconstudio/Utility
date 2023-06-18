module;
#include "Deserialization.inl"

export module Utility.Datagram.Deserialization;
import Utility;
import Utility.String;
import Utility.Array;
import Utility.Constraints;

export namespace util
{
	template<typename T>
	struct [[nodiscard]] Deserializer
	{
		Deserializer() = delete;
		~Deserializer() = delete;
	};
}

export template<>
struct util::Deserializer<int>
{
	template<size_t Offset, size_t Length>
	[[nodiscard]]
	static constexpr int Parse(const util::Array<char, Length>& buffer)
		noexcept
	{
		if constexpr (Length < Offset + sizeof(int))
		{
			static_assert(util::always_false<util::Array<char, Length>>, "The buffer is too small to contain an int.");
		}

		const unsigned int result = (static_cast<unsigned>(buffer[Offset]) << 24)
			| (static_cast<unsigned>(buffer[Offset + 1]) << 16)
			| (static_cast<unsigned>(buffer[Offset + 2]) << 8)
			| static_cast<unsigned>(buffer[Offset + 3]);

		return static_cast<int>(result);
	}

	template<size_t Length>
	[[nodiscard]]
	static constexpr int Parse(const char(&buffer)[Length])
		noexcept(sizeof(int) <= Length)
	{
		if constexpr (Length < sizeof(int))
		{
			static_assert(util::always_false<util::Array<char, Length>>, "The buffer is too small to contain an int.");
		}

		const unsigned int result = (static_cast<unsigned>(buffer[0]) << 24)
			| (static_cast<unsigned>(buffer[1]) << 16)
			| (static_cast<unsigned>(buffer[2]) << 8)
			| static_cast<unsigned>(buffer[3]);

		return static_cast<int>(result);
	}

	[[nodiscard]]
	static constexpr int Parse(const char* buffer, const size_t& length)
	{
		if (length < sizeof(int))
		{
			throw "The buffer is too small to contain an int.";
		}

		const unsigned int result = (static_cast<unsigned>(buffer[0]) << 24)
			| (static_cast<unsigned>(buffer[1]) << 16)
			| (static_cast<unsigned>(buffer[2]) << 8)
			| static_cast<unsigned>(buffer[3]);

		return static_cast<int>(result);
	}
};
