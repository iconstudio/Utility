export module Utility.Constraints;
import <concepts>;
export import Utility.Traits;

export namespace util
{
	template<typename... Ts>
	concept classes = make_conjunction<std::is_class, clean_t<Ts>...>;

	template<typename T>
	concept finalized = classes<T> && std::is_final_v<clean_t<T>>;

	template<typename Derived, typename Parent>
	concept hierachy = classes<Derived, Parent> && std::derived_from<Derived, remove_cv_t<Parent>>;

	template<typename D>
	concept crtp = classes<D> && std::same_as<D, remove_cv_t<D>>;

	template<typename T>
	using not_void = std::negation<std::is_same<clean_t<T>, void>>;

	template<typename... Ts>
	concept notvoids = make_conjunction<not_void, clean_t<Ts>...>;

	template<typename... Ts>
	concept integrals = make_conjunction<std::is_integral, clean_t<Ts>...>;

	template<typename... Ts>
	concept floats = make_conjunction<std::is_floating_point, clean_t<Ts>...>;

	template<typename... Ts>
	concept enumerations = make_conjunction<std::is_enum, clean_t<Ts>...>;

	template<typename... Ts>
	concept soft_enumerations = enumerations<Ts...> && !make_disjunction<std::is_scoped_enum, clean_t<Ts>...>;

	template<typename... Ts>
	concept hard_enumerations = enumerations<Ts...> && make_conjunction<std::is_scoped_enum, clean_t<Ts>...>;

	template<typename T, typename... Args>
	concept invocables = !std::is_abstract_v<clean_t<T>> && std::invocable<T, Args...>;

	template<typename T, typename Result, typename... Args>
	concept r_invocables = !std::is_abstract_v<clean_t<T>> && std::invocable<T, Args...>&& std::is_invocable_r_v<Result, T, Args...>;

	template<typename Fn, typename T>
	concept lv_invocable = invocables<Fn, make_lvalue_t<T>>;

	template<typename Fn, typename T>
	concept rv_invocable = invocables<Fn, make_rvalue_t<T>>;

	template<typename Fn, typename T>
	concept cl_invocable = invocables<Fn, make_clvalue_t<T>>;

	template<typename Fn, typename T>
	concept cr_invocable = invocables<Fn, make_crvalue_t<T>>;

	template<typename T>
	concept aggregate_classes = classes<T> && std::is_aggregate_v<clean_t<T>>;

	template<typename T>
	concept nonaggregate_classes = classes<T> && std::is_aggregate_v<clean_t<T>>;

	template<typename... Ts>
	concept default_initializables = make_conjunction<std::is_default_constructible, Ts...>;

	template<typename... Ts>
	concept copy_constructibles = make_conjunction<std::is_copy_constructible, Ts...>;

	template<typename... Ts>
	concept move_constructibles = make_conjunction<std::is_move_constructible, Ts...>;

	template<typename... Ts>
	concept copy_assignables = make_conjunction<std::is_copy_assignable, Ts...>;

	template<typename... Ts>
	concept move_assignables = make_conjunction<std::is_move_assignable, Ts...>;

	template<typename... Ts>
	concept trivials = make_conjunction<std::is_trivial, Ts...>;

	template<typename... Ts>
	concept trivially_destructibles = make_conjunction<std::is_trivially_destructible, Ts...>;

	template<typename... Ts>
	concept trivially_initializables = make_conjunction<std::is_trivially_default_constructible, Ts...>;

	template<typename... Ts>
	concept trivially_copy_constructibles = make_conjunction<std::is_trivially_copy_constructible, Ts...>;

	template<typename... Ts>
	concept trivially_move_constructibles = make_conjunction<std::is_trivially_move_constructible, Ts...>;

	template<typename... Ts>
	concept trivially_copy_assignables = make_conjunction<std::is_trivially_copy_assignable, Ts...>;

	template<typename... Ts>
	concept trivially_move_assignables = make_conjunction<std::is_trivially_move_assignable, Ts...>;

	template<typename T>
	concept nothrow_default_constructibles = std::is_nothrow_default_constructible_v<T>;

	template<typename T, typename... Args>
	concept nothrow_constructibles = std::is_nothrow_constructible_v<T, Args...>;

	template<typename From, typename To>
	concept nothrow_convertibles = std::is_nothrow_convertible_v<From, To>;

	template<typename From, typename To>
	concept nothrow_assignables = std::is_nothrow_assignable_v<To, From>;

	template<typename... Ts>
	concept nothrow_copy_constructibles = make_conjunction<std::is_nothrow_copy_constructible, Ts...>;

	template<typename... Ts>
	concept nothrow_move_constructibles = make_conjunction<std::is_nothrow_move_constructible, Ts...>;

	template<typename... Ts>
	concept nothrow_copy_assignables = make_conjunction<std::is_nothrow_copy_assignable, Ts...>;

	template<typename... Ts>
	concept nothrow_move_assignables = make_conjunction<std::is_nothrow_move_assignable, Ts...>;

	template<typename... Ts>
	concept nothrow_swappables = make_conjunction<std::is_nothrow_swappable, Ts...>;

	template<typename... Ts>
	concept nothrow_destructibles = make_conjunction<std::is_nothrow_destructible, Ts...>;

	template<typename T, typename... Args>
	concept nothrow_invocables = std::is_nothrow_invocable_v<T, Args...>;

	template<typename T, typename Result, typename... Args>
	concept nothrow_r_invocables = std::is_nothrow_invocable_r_v<Result, T, Args...>;

	template<typename T>
	concept basic_arithmeticals = std::is_arithmetic_v<clean_t<T>>;

	template<typename... Args>
	concept argument_available = 0 < sizeof...(Args);

	template<size_t... Indices>
	concept index_available = 0 < sizeof...(Indices);

	template<typename Ty1, typename Ty2>
	concept similar = !std::same_as<clean_t<Ty1>, clean_t<Ty2>>&& std::common_with<clean_t<Ty1>, clean_t<Ty2>>;

	template <typename T>
	concept actual_integral = is_actual_integral_v<T>;

	namespace detail
	{
		template<typename T, typename Oty>
		struct same_as_t : std::bool_constant<std::same_as<T, Oty>>
		{};

		template<typename T>
		struct same_as_bind
		{
			template<typename Oty>
			using result = same_as_t<T, Oty>;
		};
	}

	template<typename T, typename... Ots>
	concept same_as = make_conjunction<typename detail::same_as_bind<T>::template result, Ots...>;

	template<typename... Ts>
	concept specializations = is_specialization_v<clean_t<Ts>...>;

	template<typename Rx, typename Fn, typename... Args>
	concept invocable_results = invocables<Fn, Args...>&& convertible_to<std::invoke_result_t<Fn, Args...>, Rx>;
}
