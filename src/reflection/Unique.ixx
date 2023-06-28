export module Utility.Reflection.Unique;
import Utility.FixedString;

export namespace reflex
{
	export using ::util::basic_fixed_string;

	template<typename D, basic_fixed_string Name>
	class Unique
	{
	public:
		constexpr Unique(D* instance) noexcept
		{
			Instance = instance;
		}

		constexpr ~Unique() noexcept = default;

		[[nodiscard]]
		static constexpr D* GetInstance() noexcept
		{
			return static_cast<D*>(Instance);
		}

		static consteval auto&& GetUniqueName() noexcept { return Name; }

		Unique(const Unique& other) noexcept = delete;
		Unique& operator=(const Unique& other) noexcept = delete;
		Unique(Unique&& other) noexcept = default;
		Unique& operator=(Unique&& other) noexcept = default;

		static inline D* Instance = nullptr;
	};
}

#pragma warning(push, 1)
namespace reflex::test
{
#if 1
	void test()
	{
		int test = 0;

		const Unique<int, "test"> inst{ &test };
		const auto ptr = Unique<int, "test">::GetInstance();

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
