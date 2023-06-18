export module Utility.Traits;
export import <utility>;

export namespace util
{
	// type properties
	using ::std::add_const;
	using ::std::add_const_t;
	using ::std::add_volatile;
	using ::std::add_volatile_t;
	using ::std::add_cv;
	using ::std::add_cv_t;
	using ::std::add_pointer;
	using ::std::add_pointer_t;
	using ::std::add_lvalue_reference;
	using ::std::add_lvalue_reference_t;
	using ::std::add_rvalue_reference;
	using ::std::add_rvalue_reference_t;

	using ::std::remove_const;
	using ::std::remove_const_t;
	using ::std::remove_volatile;
	using ::std::remove_volatile_t;
	using ::std::remove_cv;
	using ::std::remove_cv_t;
	using ::std::remove_pointer;
	using ::std::remove_pointer_t;
	using ::std::remove_extent;
	using ::std::remove_extent_t;
	using ::std::remove_all_extents;
	using ::std::remove_all_extents_t;
	using ::std::remove_reference;
	using ::std::remove_reference_t;
	using ::std::remove_cvref;
	using ::std::remove_cvref_t;

	using ::std::ref;
	using ::std::cref;
	using ::std::as_const;
	using ::std::unwrap_reference;
	using ::std::unwrap_reference_t;
	using ::std::unwrap_reference;
	using ::std::unwrap_reference_t;
	using ::std::decay;
	using ::std::decay_t;
	using ::std::unwrap_ref_decay;
	using ::std::unwrap_ref_decay_t;

	// logical meta functors
	using ::std::negation;
	using ::std::negation_v;
	using ::std::conditional;
	using ::std::conditional_t;
	using ::std::enable_if;
	using ::std::enable_if_t;
	using ::std::void_t;

	using ::std::integral_constant;
	using ::std::true_type;
	using ::std::false_type;

	using ::std::is_same;
	using ::std::is_same_v;

	// ctor flags
	using ::std::is_copy_constructible;
	using ::std::is_move_constructible;
	using ::std::is_copy_constructible_v;
	using ::std::is_move_constructible_v;
	using ::std::is_copy_assignable;
	using ::std::is_move_assignable;
	using ::std::is_copy_assignable_v;
	using ::std::is_move_assignable_v;

	// trivials
	using ::std::is_trivial;
	using ::std::is_trivial_v;
	using ::std::is_trivially_constructible;
	using ::std::is_trivially_constructible_v;
	using ::std::is_trivially_default_constructible;
	using ::std::is_trivially_default_constructible_v;
	using ::std::is_trivially_copy_constructible;
	using ::std::is_trivially_copy_constructible_v;
	using ::std::is_trivially_move_constructible;
	using ::std::is_trivially_move_constructible_v;
	using ::std::is_trivially_copy_assignable;
	using ::std::is_trivially_copy_assignable_v;
	using ::std::is_trivially_move_assignable;
	using ::std::is_trivially_move_assignable_v;
	using ::std::is_trivially_destructible;
	using ::std::is_trivially_destructible_v;

	// models
	//using ::std::same_as;

	using ::std::copyable;
	using ::std::movable;

	using ::std::default_initializable;
	using ::std::copy_constructible;
	using ::std::move_constructible;
	using ::std::constructible_from;
	using ::std::convertible_to;
	using ::std::assignable_from;

	using ::std::equality_comparable;
	using ::std::equality_comparable_with;

	// placeholders
	using ::std::in_place_t;
	using ::std::in_place;
	using ::std::in_place_type_t;
	using ::std::in_place_type;
	using ::std::in_place_index_t;
	using ::std::in_place_index;

	template <size_t... Indices>
	struct in_place_indices_t
	{
		explicit constexpr in_place_indices_t() noexcept = default;
		constexpr ~in_place_indices_t() noexcept = default;

		explicit constexpr in_place_indices_t(std::index_sequence<Indices...>) noexcept
		{}
	};

	template <size_t... Indices>
	inline constexpr in_place_indices_t<Indices...> in_place_indices{};

	// Always
	template<typename T>
	inline constexpr bool always_false = false;

	// cleaner
	template<typename T>
	using clean_t = std::remove_cvref_t<std::remove_all_extents_t<T>>;

	template <typename T>
	inline constexpr bool is_actual_integral_v = std::_Is_any_of_v<std::remove_cvref_t<T>, short, unsigned short, int, unsigned int, long, unsigned long, long long, unsigned long long>;

	template <typename Special, template <typename...> typename Template>
	struct is_specialization : std::false_type
	{};

	template<template<typename...> typename Template, typename... Args>
	struct is_specialization<Template<Args...>, Template> : std::true_type
	{};

	template<typename Special, template<typename...> typename Template>
	inline constexpr bool is_specialization_v = is_specialization<Special, Template>::value;

	template <typename Special, template <size_t...> typename Template>
	struct is_indexed : std::false_type
	{};

	template<template<size_t...> typename Template, size_t... Indices>
	struct is_indexed<Template<Indices...>, Template> : std::true_type
	{};

	template<typename Special, template<size_t...> typename Template>
	inline constexpr bool is_indexed_v = is_indexed<Special, Template>::value;

	template<typename T>
	struct avoid_void
	{
		template<typename Fn, typename... Args>
		static consteval auto Eval() noexcept(std::is_nothrow_invocables<Fn, Args...>)->std::invoke_result_t<Fn, Args...>;
	};

	template<typename Void>
		requires (is_same_v<Void, void>)
	struct avoid_void<Void>
	{
		template<typename Fn, typename... Args>
		static consteval auto Eval() noexcept(std::is_nothrow_invocable_v<Fn, Args...>)->std::invoke_result_t<Fn, Args...>;

		template<typename Fn>
		static consteval auto Eval() noexcept(std::is_nothrow_invocable_v<Fn>)->std::invoke_result_t<Fn>;
	};

	template<template<typename> typename MetaFn, template<typename> typename Wrapper, typename... Ts>
	struct logical_product
	{
		using result = Wrapper<MetaFn<Ts>...>;
		static inline constexpr bool value = result::value;
	};

	template<template<typename> typename MetaFn, template<typename> typename Wrapper, typename... Rests>
	struct logical_product<MetaFn, Wrapper, void, Rests...> : logical_product<MetaFn, Wrapper, Rests...>
	{};

	template<template<typename> typename MetaFn, template<typename> typename Wrapper>
	struct logical_product<MetaFn, Wrapper> : false_type
	{};

	template<template<typename> typename MetaFn, template<typename> typename Wrapper, typename... Ts>
	inline constexpr bool logical_product_v = logical_product<MetaFn, Wrapper, Ts...>::value;

	template<template<typename> typename MetaFn, typename... Ts>
	inline constexpr bool make_conjunction = logical_product_v<MetaFn, std::conjunction, Ts...>;

	template<template<typename> typename MetaFn, typename... Ts>
	inline constexpr bool make_disjunction = logical_product_v<MetaFn, std::disjunction, Ts...>;

	template<typename T>
	struct is_explicit_constructible : conditional_t<is_trivial_v<T>, false_type, true_type> {};

	template<typename T>
	inline constexpr bool is_explicit_constructible_v = is_explicit_constructible<T>::value;

	template<typename Fn, typename... Args>
	using monad_result_t = clean_t<std::invoke_result_t<Fn, Args...>>;

	template<typename T>
	using make_lvalue_t = conditional_t<std::same_as<clean_t<T>, void>, void, add_lvalue_reference_t<remove_const_t<T>>>;

	template<typename T>
	using make_clvalue_t = conditional_t<std::same_as<clean_t<T>, void>, void, add_lvalue_reference_t<add_const_t<T>>>;

	template<typename T>
	using make_rvalue_t = conditional_t<std::same_as<clean_t<T>, void>, void, add_rvalue_reference_t<remove_const_t<T>>>;

	template<typename T>
	using make_crvalue_t = conditional_t<std::same_as<clean_t<T>, void>, void, add_rvalue_reference_t<add_const_t<T>>>;

	template<typename Fn, typename T, template<typename> typename Wrapper>
	using fn_result_t = util::monad_result_t<Fn, Wrapper<T>>;
}

#pragma warning(push, 1)
namespace util::test
{
	struct test_noncopy_ctor
	{
		test_noncopy_ctor(const test_noncopy_ctor& other) = delete;
	};

	struct test_noncopy_asin
	{
		test_noncopy_asin& operator=(const test_noncopy_asin& other) = delete;
	};

	constexpr bool av0 = logical_product_v<is_copy_assignable, std::conjunction, int, long, float, short>;
	constexpr bool av1 = logical_product_v<is_copy_assignable, std::conjunction, int, test_noncopy_ctor>;
	constexpr bool av2 = logical_product_v<is_copy_assignable, std::conjunction, int, test_noncopy_asin>;
	constexpr bool av3 = logical_product_v<is_copy_assignable, std::conjunction, void, bool>;
}
#pragma warning(pop)
