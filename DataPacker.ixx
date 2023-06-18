export module Utility.Datagram.Packer;
import <utility>;
import <tuple>;
import Utility;
import Utility.Meta;
import Utility.Constraints;
import Utility.Memory;
import Utility.Array;
import Utility.Monad;
import Utility.Datagram.Unit;
import Utility.Datagram.Serialization;

export namespace util::datagram
{
	template<typename... Ts>
	class [[nodiscard]] DataPacker;
}

namespace util::detail
{
	template <size_t Index, typename Variant>
	struct get_node_at;

	template <size_t Index, typename T, typename... Ts>
	struct get_node_at<Index, datagram::DataPacker<T, Ts...>>
	{
		using type = typename get_node_at<Index - 1, datagram::DataPacker<Ts...>>::type;
	};

	template <typename T, typename... Ts>
	struct get_node_at<0, datagram::DataPacker<T, Ts...>>
	{
		using seq_type = datagram::DataPacker<T, Ts...>;
		using type = T;
	};
}

export namespace util::datagram
{
	template<notvoids Fty, notvoids... Rty>
	class [[nodiscard]] DataPacker<Fty, Rty...>
	{
	public:
		using Pack = std::tuple<Fty, Rty...>;

		static inline constexpr size_t myLength = 1 + sizeof...(Rty);
		static inline constexpr size_t mySize = meta::byte_size_v<Fty, Rty...>;

		constexpr DataPacker(const DataPacker& other) noexcept = default;
		constexpr DataPacker(DataPacker&& other) noexcept = default;
		constexpr DataPacker& operator=(const DataPacker& other) noexcept = default;
		constexpr DataPacker& operator=(DataPacker&& other) noexcept = default;

		constexpr DataPacker() noexcept
		{}

		template<typename... Args>
		explicit constexpr DataPacker(Args&&... args)
			noexcept
		{
			static_assert(serializables<clean_t<Args>...>);

			InternalWrites(std::forward_as_tuple(std::forward<Args>(args)...), std::index_sequence_for<Args...>{});
		}

		constexpr ~DataPacker() noexcept
		{}

		template<typename... Args, size_t I, size_t Next, size_t... Indices>
		constexpr
			void
			InternalWrites(const std::tuple<Args...>& data, std::index_sequence<I, Next, Indices...>)
			noexcept
		{
			Write(std::get<I>(data), Summarize<I>());

			// continue
			InternalWrites(data, std::index_sequence<Next, Indices...>{});
		}

		template<typename... Args, size_t Last>
		constexpr
			void
			InternalWrites(const std::tuple<Args...>& data, std::index_sequence<Last>)
			noexcept
		{
			Write(std::get<Last>(data), Summarize<Last>());
		}

		template<typename T>
		constexpr void Write(T&& value, const size_t& offset)
		{
			using parser = typename util::template Serializer<clean_t<T>>;

			const auto serialized = parser::Parse(std::forward<T>(value));
			char* const& ptr = internalBuffer + offset;

			serialized.CopyTo(ptr, myLength);
		}

		constexpr void Serialize(const char* output, const size_t& out_length) const noexcept
		{
			for (const char* it = internalBuffer; it != internalBuffer + out_length; ++it, ++output)
			{
				*output = *it;
			}
		}

		constexpr char* Serialize() noexcept
		{
			return internalBuffer;
		}

		char internalBuffer[mySize]{};

		template<size_t Index>
		static consteval size_t Summarize() noexcept
		{
			if constexpr (0 == Index)
			{
				return 0;
			}
			else
			{
				return SummarizeFor(std::make_index_sequence<Index>{});
			}
		}

		template<size_t... Indices>
		static consteval size_t SummarizeFor(std::index_sequence<Indices...>) noexcept
		{
			size_t result = 0;

			((result += sizeof(meta::at<Pack, Indices>)), ...);

			return result;
		}
	};

	template<>
	class DataPacker<>
	{
	public:
		static inline constexpr size_t myLength = 0;
		static inline constexpr size_t mySize = 0;

		constexpr DataPacker() noexcept = default;
		constexpr ~DataPacker() noexcept = default;

		//[[msvc::no_unique_address]] DataUnit<void> myData{};
	};

	template<typename... Ts>
	DataPacker(Ts...) -> DataPacker<Ts...>;
}

#pragma warning(push, 1)
export namespace util::test
{
#if true
	void test_datapacker()
	{
		constexpr datagram::DataPacker test_pk1{};

		constexpr datagram::DataPacker<int> test_pk2{};

		constexpr datagram::DataPacker<int, int, int> test_pk3{ 52534430, 52534430, 52534430 };
		static_assert(test_pk3.internalBuffer[3] != 1);

		constexpr int fg = 52534430;
		const auto handle = reinterpret_cast<const char*>(&fg);

		constexpr datagram::DataPacker<int, long, float, short, unsigned char, unsigned, bool> test_pk4{};
		constexpr size_t pk4_sz_a0 = test_pk4.Summarize<0>();
		constexpr size_t pk4_sz_a1 = test_pk4.Summarize<1>();
		constexpr size_t pk4_sz_a2 = test_pk4.Summarize<2>();
		constexpr size_t pk4_sz_a3 = test_pk4.Summarize<3>();
		constexpr size_t pk4_sz_a4 = test_pk4.Summarize<4>();
		constexpr size_t pk4_sz_a5 = test_pk4.Summarize<5>();
		constexpr size_t pk4_sz_a6 = test_pk4.Summarize<6>();
		constexpr size_t pk4_sz_a7 = test_pk4.Summarize<7>();

		constexpr datagram::DataPacker<int, long, float, short, unsigned char, unsigned, bool> test_pk5{ 256, 40L, 0.0174124983f };

		static_assert(test_pk5.myLength == 7);
		static_assert(test_pk5.mySize == sizeof(int) + sizeof(long) + sizeof(float) + sizeof(short) + sizeof(unsigned char) + sizeof(unsigned) + sizeof(bool));
		//static_assert(test_pk5.internalBuffer[3] == 1);
	}
#endif // true
}
#pragma warning(pop)
