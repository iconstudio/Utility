export module Utility.Reflection.Unique;
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

		static consteval auto&& GetUniqueName() noexcept { return Name; }
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
		constexpr auto&& dbuffer = dname.data();

		constexpr auto dname_0 = dbuffer[0];
		constexpr auto dname_1 = dbuffer[1];
		constexpr auto dname_2 = dbuffer[2];
		constexpr auto dname_3 = dbuffer[3];
		constexpr auto dname_4 = dbuffer[4];
	}
#endif
}
#pragma warning(pop)
