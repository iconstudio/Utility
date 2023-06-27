export module Utility.Reflection.Unique;
import Utility.Reflection;
import Utility.Singleton;
import Utility.FixedString;

export namespace reflex
{
	export using ::util::basic_fixed_string;

	template<typename D, basic_fixed_string Name>
	class Unique : public util::Singleton<Unique<D, Name>>
	{
	public:
		constexpr Unique() noexcept
			: util::Singleton<Unique<D, Name>>(this)
		{}

		static consteval auto&& GetUniqueName() { return Name; }
	};
}

#pragma warning(push, 1)
namespace reflex::test
{
#if 1
	void test()
	{
		const Unique<int, "test"> inst{};
		const auto ptr = Unique<int, "test">::Instance;

		constexpr auto&& dname = inst.GetUniqueName();
		constexpr auto dname_0 = dname.intBuffer[0];
		constexpr auto dname_1 = dname.intBuffer[1];
		constexpr auto dname_2 = dname.intBuffer[2];
		constexpr auto dname_3 = dname.intBuffer[3];
		constexpr auto dname_4 = dname.intBuffer[4];
	}
#endif
}
#pragma warning(pop)
