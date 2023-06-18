module;
#include <memory>

export module Utility.Datagram.Unit;
import Utility;
import Utility.Meta;
import Utility.Constraints;
import Utility.Memory;
import Utility.Array;
import Utility.Datagram.Serialization;

export namespace util::datagram
{
	template<typename T>
	struct DataUnit
	{
		static inline constexpr size_t size = sizeof(T);

		constexpr DataUnit(char* output, const size_t& offset) noexcept
			: myBuffer(output + offset)
		{}

		constexpr void Write(const T& value) const
		{
			static_assert(serializables<T>, "T must be serializable");

			const Array<char, size> result = util::Serialize(value);
			result.CopyTo(myBuffer, size);
		}

		constexpr DataUnit(const DataUnit& other) noexcept = default;
		constexpr DataUnit(DataUnit&& other) noexcept = default;
		constexpr DataUnit& operator=(const DataUnit& other) noexcept = default;
		constexpr DataUnit& operator=(DataUnit&& other) noexcept = default;
		constexpr ~DataUnit() noexcept = default;

		char* myBuffer;
	};

	template<typename T>
	DataUnit(T) -> DataUnit<T>;
}

#pragma warning(push, 1)
namespace util::test
{
#if true
	constexpr void test_dataunit_stack() noexcept
	{
		char output[1000]{};

		datagram::DataUnit<int> data0{ output, 0 };
		datagram::DataUnit<int> data1{ output, 4 };
		datagram::DataUnit<int> data2{ output, 8 };

		data0.Write(1);
		data1.Write(2);
		data2.Write(3);

		//static_assert(data0.myBuffer[3] == 5);
		//static_assert((data3.myBuffer)[3] == 5);
	}
#endif
}
#pragma warning(pop)