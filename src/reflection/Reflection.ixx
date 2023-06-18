export module Utility.Reflection;
import <string>;
import Utility.Meta;
import Utility.FixedString;

namespace reflex
{
	export using ::util::basic_fixed_string;
	export using ::util::fixed_string;

	export template<typename... Ts> using tag_types = meta::MetaList<Ts...>;

	template<typename Tag>
	struct tag_traits
	{
		using type = meta::MetaList<void>;
	};

	template<typename... Ts>
	struct tag_traits<meta::MetaList<Ts...>>
	{
		using type = meta::MetaList<Ts...>;
	};

	template<typename Tag>
	using tag_traits_t = typename tag_traits<Tag>::type;

	export template<typename T>
		struct reflect_traits;

	export template<typename T>
		struct reflect_traits<const T> : reflect_traits<T>
	{};

	export template<typename T>
		struct reflect_traits<volatile T> : reflect_traits<T>
	{};

	export template<typename T>
		struct reflect_traits<const volatile T> : reflect_traits<T>
	{};

	export template<typename T>
		struct reflect_traits<T[]> : reflect_traits<T*>
	{};

	export template<typename T, typename Trait = reflect_traits<T>>
		struct reflection
	{
		consteval reflection() noexcept = default;
		constexpr ~reflection() noexcept = default;

		using type = typename Trait::type;
		using parents = typename tag_traits_t<typename Trait::parents>;
		using children = typename tag_traits_t<typename Trait::children>;

		static constexpr size_t type_id = Trait::type_id;
		static constexpr std::string_view name = Trait::name;
		static constexpr size_t size = Trait::size;
	};

	export constexpr size_t LastTypeIndex = 27;

#define MAKE_PARENTS(...) \
	::reflex::tag_types<__VA_ARGS__>

#define MAKE_CHILDREN(...) \
	::reflex::tag_types<__VA_ARGS__>

#define MAKE_REFLECTION(Type, ID, Name, Parents, Children) \
	template<> \
	struct ::reflex::reflect_traits<Type> \
	{\
		using type = Type;\
		using parents = Parents;\
		using children = Children;\
		\
		static constexpr size_t type_id = ID;\
		static constexpr std::string_view name = Name;\
		static constexpr size_t size = sizeof(Type);\
	}

#define MAKE_VOID_REFLECTION(Type, ID, Name, Parents, Children) \
	template<> \
	struct ::reflex::reflect_traits<Type> \
	{\
		using type = Type;\
		using parents = Parents;\
		using children = Children;\
		\
		static constexpr size_t type_id = ID;\
		static constexpr std::string_view name = Name;\
		static constexpr size_t size = 0;\
	}

#define MAKE_ARRAY_REFLECTION(Type, Name) \
	template<size_t Length> \
	struct ::reflex::reflect_traits<Type[Length]> \
	{\
		using type = Type;\
		using parents = ::reflex::reflect_traits<Type>::parents;\
		using children = ::reflex::reflect_traits<Type>::children;\
		\
		static constexpr size_t type_id = ::reflex::reflect_traits<Type>::type_id;\
		static constexpr std::string_view name = Name;\
		static constexpr size_t size = sizeof(Type) * Length;\
	}

#define MAKE_REFLECTION_NO_CHILDREN(type, id, name, ps) \
	MAKE_REFLECTION(type, id, name, ps, void)

#define MAKE_REFLECTION_NO_PARENTS(type, id, name, chs) \
	MAKE_REFLECTION(type, id, name, void, chs)

#define MAKE_REFLECTION_NO_HIERACHY(type, id, name) \
	MAKE_REFLECTION(type, id, name, void, void)
}

export MAKE_REFLECTION_NO_HIERACHY(nullptr_t, 0, "null");
export MAKE_VOID_REFLECTION(void, 1, "void", void, void);
export MAKE_REFLECTION_NO_HIERACHY(bool, 2, "bool");
export MAKE_ARRAY_REFLECTION(bool, "array of bool");

export MAKE_REFLECTION_NO_HIERACHY(char, 3, "char");
export MAKE_ARRAY_REFLECTION(char, "array of char");
export MAKE_REFLECTION_NO_HIERACHY(signed char, 4, "signed char");
export MAKE_ARRAY_REFLECTION(signed char, "array of signed char");
export MAKE_REFLECTION_NO_HIERACHY(unsigned char, 5, "unsigned char");
export MAKE_ARRAY_REFLECTION(unsigned char, "array of char");
export MAKE_REFLECTION_NO_HIERACHY(wchar_t, 6, "wide char");
export MAKE_ARRAY_REFLECTION(wchar_t, "array of wide char");
export MAKE_REFLECTION_NO_HIERACHY(char8_t, 7, "utf-8");
export MAKE_ARRAY_REFLECTION(char8_t, "array of utf-8");
export MAKE_REFLECTION_NO_HIERACHY(char16_t, 8, "utf-16");
export MAKE_ARRAY_REFLECTION(char16_t, "array of utf-16");
export MAKE_REFLECTION_NO_HIERACHY(char32_t, 9, "utf-32");
export MAKE_ARRAY_REFLECTION(char32_t, "array of utf-32");

export MAKE_REFLECTION_NO_HIERACHY(short, 10, "short");
export MAKE_ARRAY_REFLECTION(short, "array of short");
export MAKE_REFLECTION_NO_HIERACHY(unsigned short, 11, "unsigned short");
export MAKE_ARRAY_REFLECTION(unsigned short, "array of unsigned short");
export MAKE_REFLECTION_NO_HIERACHY(int, 12, "int");
export MAKE_ARRAY_REFLECTION(int, "array of int");
export MAKE_REFLECTION_NO_HIERACHY(unsigned int, 13, "unsigned int");
export MAKE_ARRAY_REFLECTION(unsigned int, "array of unsigned int");
export MAKE_REFLECTION_NO_HIERACHY(long, 14, "long");
export MAKE_ARRAY_REFLECTION(long, "array of long");
export MAKE_REFLECTION_NO_HIERACHY(unsigned long, 15, "unsigned long");
export MAKE_ARRAY_REFLECTION(unsigned long, "array of unsigned long");
export MAKE_REFLECTION_NO_HIERACHY(long long, 16, "long long");
export MAKE_ARRAY_REFLECTION(long long, "array of long long");
export MAKE_REFLECTION_NO_HIERACHY(unsigned long long, 17, "unsigned long long");
export MAKE_ARRAY_REFLECTION(unsigned long long, "array of unsigned long long");
export MAKE_REFLECTION_NO_HIERACHY(float, 18, "float");
export MAKE_ARRAY_REFLECTION(float, "array of float");
export MAKE_REFLECTION_NO_HIERACHY(double, 19, "double");
export MAKE_ARRAY_REFLECTION(double, "array of double");
export MAKE_REFLECTION_NO_HIERACHY(long double, 20, "long double");
export MAKE_ARRAY_REFLECTION(long double, "array of long double");
export MAKE_REFLECTION_NO_HIERACHY(void*, 21, "void*");
export MAKE_ARRAY_REFLECTION(void*, "array of void*");

export MAKE_REFLECTION_NO_HIERACHY(std::string, 22, "string");
export MAKE_REFLECTION_NO_HIERACHY(std::wstring, 23, "wide string");
export MAKE_REFLECTION_NO_HIERACHY(std::u8string, 24, "utf-8 string");
export MAKE_REFLECTION_NO_HIERACHY(std::u16string, 25, "utf-16 string");
export MAKE_REFLECTION_NO_HIERACHY(std::u32string, 26, "utf-32 string");
export MAKE_REFLECTION_NO_HIERACHY(std::string_view, ::reflex::LastTypeIndex, "string_view");

export namespace reflex
{
	template<typename T>
	concept reflected = requires
	{
		reflect_traits<T>::type;
		reflect_traits<T>::parents;
		reflect_traits<T>::children;
		reflect_traits<T>::type_id;
		reflect_traits<T>::name;
	};

	template<typename T>
	using reflect_t = typename reflect_traits<T>::template type;

	template<typename T>
	using parents = typename reflect_traits<T>::template parents;

	template<typename T>
	using children = typename reflect_traits<T>::template children;

	template<typename T, size_t I>
	using parent = meta::at<parents<T>, I>;

	template<typename T, size_t I>
	using child = meta::at<children<T>, I>;

	template<typename T>
	inline constexpr size_t type_id = reflect_traits<T>::template type_id;

	template<typename T>
	inline constexpr std::string_view name = reflect_traits<T>::template name;

	template<typename T, typename U>
	consteval bool operator==(reflect_traits<T>, reflect_traits<U>) noexcept
	{
		return type_id<T> == type_id<U>;
	}
}

#pragma warning(push, 1)
namespace reflex::test
{
#if false
	void test()
	{
		using namespace ::std::literals;

		constexpr fixed_string aaa{ "342" };
		const fixed_string bbb{ "342" };
		fixed_string ccc{ "342" };

		constexpr reflect_t<const volatile int> aaa_2 = 0;
		//constexpr parents<int> bbb_2{};
		//constexpr children<int> ccc_2{};
		constexpr auto nameii = name<int>;
		constexpr char name_2 = nameii[0];
		constexpr auto id_2 = type_id<int>;
	}
#endif
}
#pragma warning(pop)
