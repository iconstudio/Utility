module;
#include <utility>

export module Utility.Meta;
import Utility.Traits;

#define EXPORT export

namespace meta::detail
{
	template <typename Ty, size_t>
	using item = Ty;

	template <typename, typename, template <typename...> typename>
	struct invoke_args;

	template <typename T, size_t... Indices, template <typename...> class Predicate>
	struct invoke_args<T, std::index_sequence<Indices...>, Predicate>
	{
		using type = Predicate<item<T, Indices>...>;
	};

	// construct a sequence consisting of repetitions of T
	template <size_t Count, typename T, template <typename...> typename Predicate>
	using create = typename invoke_args<T, std::make_index_sequence<Count>, Predicate>::type;

	template <size_t Index, typename Void, template <typename...> typename Seq, typename...>
	struct __at_impl;

	// extract the Index-th type from Sequence
	template <typename Seq, size_t Index, typename = void>
	struct __at;

#ifdef __clang__
	template <template <typename...> class Seq, size_t Index, typename... _Types>
	struct __at<Seq<_Types...>, Index, std::void_t<__type_pack_element<Index, _Types...>>>
	{
		using type = __type_pack_element<Index, _Types...>;
	};
#else // ^^^ __clang__ / !__clang__ vvv
	template <typename... _VoidPtrs>
	struct enumerate_ptrs
	{
		template <typename T, typename... Rests>
		static consteval T _Eval(_VoidPtrs..., T*, Rests*...) noexcept
		{
			static_assert(util::always_false<T>, "Cannot instantiate _Eval()");
		}
	};

	template <typename T>
	consteval std::type_identity_t<T>* AsPointer() noexcept
	{
		return nullptr;
	}

	template<template<typename...> typename Seq, size_t Index>
	struct __at<Seq<>, Index>
	{
		static_assert(util::always_false<std::integral_constant<size_t, Index>>, "sequence's index out of bounds");
	};

	template <template<typename...> typename Seq, typename Fty, typename... Rty>
	struct __at<Seq<Fty, Rty...>, 0>
	{
		using type = Fty;
	};

	template <template<typename...> typename Seq, size_t Index, class Fty, class... Rty>
	struct __at<Seq<Fty, Rty...>, Index>
		: __at<Seq<Rty...>, Index - 1> {};

	/*
	template <template <typename...> typename Seq, size_t Index, typename... Ts>
		requires (Index < sizeof...(Ts))
	struct __at<Seq<Ts...>, Index>
	{
		using type = typename decltype(create<Index, void*, enumerate_ptrs>::_Eval(AsPointer<Ts>()...));
	};*/
#endif // !__clang__
}

export namespace meta
{
	/// <summary>
	/// a sequence of types
	/// </summary>
	template <typename... Ts>
	struct MetaList { consteval MetaList() noexcept = default; };
	struct MetaNil {};

	template <typename Void, template <typename...> typename Fn, typename... Args>
	struct wrapper;

	template <template <typename...> typename Fn, typename... Args>
	struct wrapper<std::void_t<Fn<Args...>>, Fn, Args...>
	{
		using type = Fn<Args...>;
	};

	// needs result type
	template <typename Fn, typename... Args>
	concept meta_invocables = requires
	{
		typename Fn::template result<Args...>;
	};

	// invoke meta-callable Functor with Arguments
	template <typename Fn, typename... Args>
		requires meta_invocables<Fn, Args...>
	using invoke = typename Fn::template result<Args...>;

	template <typename Fn, typename... Args>
		requires meta_invocables<Fn, Args...>
	using invoke_r = typename invoke<Fn, Args...>::type;

	// encapsulate a template into a meta-callable type
	template <template <typename...> typename Fn>
	struct wrap
	{
		template <typename... Args>
		using result = typename wrapper<void, Fn, Args...>::type;
	};

	// construct a meta-callable that passes its arguments and Args to Functor
	template <typename Fn, typename... Args>
	struct bind
	{
		template <typename... Ty>
			requires meta_invocables<Fn, Args..., Ty...>
		using result = typename Fn::template result<Args..., Ty...>;
	};

	// pick a type from a sequence of types by an index
	template <typename Seq>
	struct picker
	{
		// this is callable
		template<size_t I>
		using result = typename detail::__at<Seq, I>;
	};

	// unpack List into the parameters of meta-callable Functor
	template <typename Fn, typename Seq>
	struct apply;

	template <class Fn, class Seq>
		requires meta_invocables<Fn, Seq>
	using apply_t = typename apply<Fn, Seq>::type;

	// invoke meta-callable Functor with the parameters of a template specialization
	template <typename Fn, template <typename...> typename Seq, typename... Ts>
		requires meta_invocables<Fn, Seq<Ts...>>
	struct apply<Fn, Seq<Ts...>>
	{
		using type = typename Fn::template result<Seq<Ts...>>::type;
	};

	// invoke meta-callable Functor with the elements of an integer_sequence
	template <typename Fn, typename T, T... Values>
		requires meta_invocables<Fn, std::integer_sequence<T, Values...>>
	struct apply<Fn, std::integer_sequence<T, Values...>>
	{
		using type = typename Fn::template result<std::integral_constant<T, Values>...>;
	};

	// transform sequence of Ty... into sequence of Functor<Ty...>
	template <typename Fn, class Seq>
	struct transform;

	template <template <typename...> typename Seq, typename Fn, typename... Ts>
	struct transform<Fn, Seq<Ts...>>
	{
		using type = Seq<invoke_r<Fn, Ts>...>;
	};

	template <typename Fn, class Seq>
	using transform_t = typename transform<Fn, Seq>::type;

	template <typename Fn, typename T, typename... Ts>
	struct foldl;

	template <typename Fn, typename T, typename... Ts>
	using foldl_t = typename foldl<Fn, T, Ts...>::type;

	template <typename Fn, typename T, typename U, typename... Rests>
	struct foldl<Fn, T, U, Rests...>
	{
		using type = typename foldl<Fn, invoke_r<Fn, T, U>, Rests...>::type;
	};

	template <typename Fn, typename T>
	struct foldl<Fn, T>
	{
		using type = T;
	};

	template <typename Seq>
	struct front;

	template <typename Seq>
	using front_t = typename front<Seq>::type;

	template <template <typename...> typename Seq, typename First, typename... Rests>
	struct front<Seq<First, Rests...>>
	{
		using type = First;
	};

	template <template <typename...> typename Seq>
	struct front<Seq<>>
	{
		static_assert(util::always_false<Seq<>>, "An empty sequence has no element.");
	};

	template <typename Seq>
	struct back;

	template <typename Seq>
	using back_t = typename back<Seq>::type;

	template <template <typename...> typename Seq, typename Fty, typename Nty, typename... Rests>
	struct back<Seq<Fty, Nty, Rests...>> : public back<Seq<Nty, Rests...>>
	{};

	template <template <typename...> typename Seq, typename Last>
	struct back<Seq<Last>>
	{
		using type = Last;
	};

	template <template <typename...> typename Seq>
	struct back<Seq<>>
	{
		static_assert(util::always_false<Seq<>>, "An empty sequence has no element.");
	};

	template <typename LTy, typename RTy>
	struct push;

	template <typename LTy, typename RTy>
	using push_t = typename push<LTy, RTy>::type;

	template <template <typename...> typename Seq, typename T, typename... Fronts>
	struct push<Seq<Fronts...>, T>
	{
		using type = Seq<T, Fronts...>;
	};

	template <template <typename...> typename Seq, typename T, typename... Fronts>
	struct push<T, Seq<Fronts...>>
	{
		using type = Seq<T, Fronts...>;
	};

	template <typename LTy, typename RTy>
	struct push_back;

	template <typename LTy, typename RTy>
	using push_back_t = typename push_back<LTy, RTy>::type;

	template <template <typename...> typename Seq, typename... Lhs, typename... Rhs>
	struct push_back<Seq<Lhs...>, Seq<Rhs...>>
	{
		using type = Seq<Lhs..., Rhs...>;
	};

	template <template <typename...> typename Seq, typename T, typename... Backs>
	struct push_back<Seq<Backs...>, T>
	{
		using type = Seq<Backs..., T>;
	};

	template <template <typename...> typename Seq, typename T, typename... Backs>
	struct push_back<T, Seq<Backs...>>
	{
		using type = Seq<T, Backs...>;
	};

	template <typename Seq>
	struct pop;

	template <typename Seq>
	using pop_t = typename pop<Seq>::type;

	template <template <typename...> typename Seq, typename First, typename... Rests>
	struct pop<Seq<First, Rests...>>
	{
		using type = Seq<Rests...>;
	};

	template <template <typename...> typename Seq>
	struct pop<Seq<>>
	{
		static_assert(util::always_false<Seq<>>, "An empty sequence has no element.");
	};

	template <typename Seq>
	struct pop_back;

	template <typename Seq>
	using pop_back_t = typename pop_back<Seq>::type;

	template <template <typename...> typename Seq, typename First, typename Next, typename... Rests>
	struct pop_back<Seq<First, Next, Rests...>> : public push_back<First, pop_back_t<Seq<Next, Rests...>>>
	{};

	template <template <typename...> typename Seq, typename First, typename Last>
	struct pop_back<Seq<First, Last>>
	{
		using type = Seq<First>;
	};

	template <template <typename...> typename Seq, typename Last>
	struct pop_back<Seq<Last>>
	{
		using type = Seq<>;
	};

	template <template <typename...> typename Seq>
	struct pop_back<Seq<>>
	{};

	// find a type in sequence
	template <typename T, typename...>
	struct find;

	template <typename T, typename... Ts>
	using find_t = typename find<T, Ts...>::type;

	template <typename T, typename... Rests>
	struct find<T, T, Rests...>
	{
		using type = T;
	};

	template <typename T, typename First, typename... Rests>
	struct find<T, First, Rests...> : public find<T, Rests...>
	{};

	template <typename T>
	struct find<T>
	{
		//static_assert(util::always_false<T>, "Cannot find the type in the sequence.");
	};

	namespace detail
	{
		// find the index of type T in loosen sequence
		template <typename Indexer, typename T, typename...>
		struct seek_impl;

		template <typename T, typename... Ts>
		inline constexpr size_t seek_impl_v = seek_impl<std::integral_constant<size_t, 0>, T, Ts...>::value;

		template <size_t I, typename T, typename... Rests>
		struct seek_impl<std::integral_constant<size_t, I>, T, T, Rests...>
		{
			static inline constexpr size_t value = I;
		};

		template <size_t I, typename T, typename First, typename... Rests>
		struct seek_impl<std::integral_constant<size_t, I>, T, First, Rests...> : public seek_impl<std::integral_constant<size_t, I + 1>, T, Rests...>
		{};

		// find the index of type T in sequence
		template <typename Indexer, typename T, typename Seq>
		struct seek_range_impl;

		template <typename Indexer, typename T, typename Seq>
		inline constexpr size_t seek_range_impl_v = seek_range_impl<Indexer, T, Seq>::value;

		template <size_t I, template <typename...> typename Seq, typename T, typename... Rests>
		struct seek_range_impl<std::integral_constant<size_t, I>, T, Seq<T, Rests...>>
		{
			static inline constexpr size_t value = I;
		};

		template <size_t I, template <typename...> typename Seq, typename T, typename First, typename... Rests>
		struct seek_range_impl<std::integral_constant<size_t, I>, T, Seq<First, Rests...>> : public seek_range_impl<std::integral_constant<size_t, I + 1>, T, Seq<Rests...>>
		{};

		template <typename Indexer, typename T, template <typename...> typename Seq>
		struct seek_range_impl<Indexer, T, Seq<>>
		{};
	}

	template <typename T, typename... Ts>
	inline constexpr size_t seek = detail::seek_impl_v<std::integral_constant<size_t, 0>, T, Ts...>;

	template <typename T, typename Seq>
	inline constexpr size_t seek_range = detail::seek_range_impl_v<std::integral_constant<size_t, 0>, T, Seq>;

	// determine if a type is in sequence
	template <typename T, typename...>
	struct included;

	template <typename T, typename... Ts>
	constexpr bool included_v = included<T, Ts...>::value;

	template <typename T, typename... Rests>
	struct included<T, T, Rests...> : public std::true_type
	{};

	template <typename T, typename First, typename... Rests>
	struct included<T, First, Rests...> : public included<T, Rests...>
	{};

	template <typename T>
	struct included<T> : public std::false_type
	{};

	// determine if a type is in specialized template
	template <typename T, typename R>
	struct included_range : public std::false_type
	{};

	template <typename T, typename R>
	constexpr bool included_range_v = included_range<T, R>::value;

	template <typename T, template<typename...> typename R, typename... Ts>
	struct included_range<T, R<T, Ts...>>
		: public std::true_type
	{};

	template <typename T, template<typename...> typename R, typename First, typename... Rests>
	struct included_range<T, R<First, Rests...>>
		: public included_range<T, R<Rests...>>
	{};

	template <typename T, typename V, template<V, typename...> typename R, V I, typename... Ts>
	struct included_range<T, R<I, T, Ts...>> // ignore the value
		: public std::true_type
	{};

	template <typename T, typename V, template<V, typename...> typename R, V I, typename First, typename... Rests>
	struct included_range<T, R<I, First, Rests...>> // ignore the value
		: public included_range<T, R<I, Rests...>>
	{};

	// get typeparams' count of sequence
	template <typename T>
	struct tsize : public std::integral_constant<size_t, 0>
	{};

	template <template <typename...> typename Seq, typename... Ts>
	struct tsize<Seq<Ts...>> : public std::integral_constant<size_t, sizeof...(Ts)>
	{};

	template <template <typename...> typename Seq>
	struct tsize<Seq<>> : public std::integral_constant<size_t, 0>
	{};

	template <typename T>
	inline constexpr size_t tsize_v = tsize<T>::value;

	// summarize byte size of a packed sequence
	template <typename T>
	struct rng_size : public std::integral_constant<size_t, 0>
	{};

	template <template <typename...> typename Seq, typename T, typename... Ts>
	struct rng_size<Seq<T, Ts...>> : public std::integral_constant<size_t, sizeof(T) + rng_size<Seq<Ts...>>::value>
	{};

	template <template <typename...> typename Seq, typename... Ts>
	inline constexpr size_t rng_size_v = rng_size<Seq<Ts...>>::value;

	template <typename T>
	struct empty;

	template <template <typename...> typename Seq, typename... Ts>
	struct empty<Seq<Ts...>> : public std::bool_constant<sizeof...(Ts) == 0>
	{};

	template <template <typename...> typename Seq>
	struct empty<Seq<>> : public std::true_type
	{};

	template <typename T>
	inline constexpr bool empty_v = empty<T>::value;

	// summarize byte size of each type in a sequence
	template <typename...>
	struct byte_size;

	template <typename... Ts>
	inline constexpr size_t byte_size_v = byte_size<Ts...>::value;

	template <typename T>
	struct byte_size<T> : public std::integral_constant<size_t, sizeof(T)>
	{};

	template <typename T, typename... Rests>
	struct byte_size<T, Rests...> : public std::integral_constant<size_t, sizeof(T) + byte_size_v<Rests...>>
	{};

	// merge several lists into one
	template <typename...>
	struct concat;

	template <typename... Lists>
	using concat_t = typename concat<Lists...>::type;

	template <template <typename...> typename List>
	struct concat<List<>>
	{
		using type = List<>;
	};

	template <template <typename...> typename List, typename... Ts>
	struct concat<List<Ts...>>
	{
		using type = List<Ts...>;
	};

	template <>
	struct concat<void>
	{};

	template <template <typename...> typename List, typename... Ts0, typename... Ts1>
	struct concat<List<Ts0...>, List<Ts1...>>
	{
		using type = List<Ts0..., Ts1...>;
	};

	template <template <typename...> typename List, typename... Ts>
	struct concat<List<Ts...>, void>
	{
		using type = List<Ts...>;
	};

	template <template <typename...> typename List, typename... Ts>
	struct concat<void, List<Ts...>>
	{
//#pragma message("void is not a valid type in template parameter list, so it will be ignored.")
		using type = List<Ts...>;
	};

	template <template <typename...> typename List, typename... Ts0, typename... Ts1, typename... Ts2>
	struct concat<List<Ts0...>, List<Ts1...>, List<Ts2...>>
	{
		using type = List<Ts0..., Ts1..., Ts2...>;
	};

	template <template <typename...> typename List, typename... Ts0, typename... Ts1, typename... Ts2, typename... Rests>
	struct concat<List<Ts0...>, List<Ts1...>, List<Ts2...>, Rests...>
	{
		using type = concat<List<Ts0..., Ts1..., Ts2...>, Rests...>::type;
	};

	// construct a sequence consisting of repetitions of T
	template <size_t Count, typename T, template <typename...> typename Predicate>
	using create = typename detail::invoke_args<T, std::make_index_sequence<Count>, Predicate>::type;

	// get the type at Index in Sequence
	template <typename Seq, size_t Index>
	using at = typename detail::__at<Seq, Index>::type;

	// get an index from index sequence
	template <typename Indexer>
	struct peek_index;

	template <typename Indexer>
	inline constexpr size_t peek_index_v = peek_index<Indexer>::value;

	template <size_t C, size_t... Indices>
	struct peek_index<std::index_sequence<C, Indices...>>
	{
		static inline constexpr size_t value = C;
	};

	template <>
	struct peek_index<std::index_sequence<>>
	{};

	template <typename T, size_t>
	using repeater = T;

	template <typename Fn, typename Seq>
	concept meta_enumerables = meta_invocables<Fn, Seq>&& requires(Fn fn, Seq seq)
	{
		{ fn(seq) } -> std::same_as<Seq>;
	};

	namespace detail
	{
		template <typename Void, typename Fn
			, typename Seq
			, typename Indexer>
			requires meta_invocables<Fn, Seq>
		struct repeat_impl;

		template <typename Fn
			, template<typename...> typename Seq, typename... Ts
			, size_t I, size_t... Indices>
			requires meta_invocables<Fn, Seq<Ts...>>
		struct repeat_impl<std::void_t<Seq<Ts...>>, Fn
			, Seq<Ts...>
			, std::index_sequence<I, Indices...>>
			: repeat_impl<void, Fn
			, typename invoke<bind<Fn, Seq<Ts...>>>::type
			, std::index_sequence<Indices...>>
		{};

		template <typename Fn, typename Seq>
		struct repeat_impl<void, Fn, Seq, std::index_sequence<>>
		{
			using type = Seq;
		};

		template <typename Void, typename Fn
			, typename Result
			, typename Seq
			, typename Indexer>
			requires meta_invocables<Fn, Seq>
		struct take_while_impl;

		/// <summary>
		/// Pick the first element of the sequence and invoke function with it, then append to the result sequence.
		/// </summary>
		/// <typeparam name="Fn">unary functor(T)</typeparam>
		/// <typeparam name="Result">the result sequence</typeparam>
		/// <typeparam name="Seq">type sequence(T)</typeparam>
		/// <typeparam name="T">the current proceed type</typeparam>
		/// <typeparam name="...Ts"> rest types</typeparam>
		/// <typeparam name="I">the current index</typeparam>
		/// <typeparam name="...Indices">rest indices</typeparam>
		template <typename Fn
			, typename Result
			, template<typename...> typename Seq, typename T, typename... Ts
			, size_t I, size_t... Indices>
			requires meta_invocables<Fn, T>
		struct take_while_impl<void, Fn
			, Result
			, Seq<T, Ts...>
			, std::index_sequence<I, Indices...>>
			: take_while_impl<std::void_t<Seq<Ts...>>, Fn
			, concat_t<Result, invoke_r<bind<Fn, T>>>
			, Seq<Ts...>
			, std::index_sequence<Indices...>>
		{};

		// end with empty index sequence
		template <typename Fn, typename Result, typename Seq>
		struct take_while_impl<void, Fn, Result, Seq, std::index_sequence<>>
		{
			using type = Result;
		};

		// end with empty type sequence
		template <typename Fn, typename Result, template<typename...> typename Seq, size_t... Indices>
		struct take_while_impl<void, Fn, Result, Seq<>, std::index_sequence<Indices...>>
		{
			using type = Result;
		};

		template <typename Fn, typename Seq, typename Indexer>
		struct take_while_for_impl;

		template <typename Fn, template<typename...> typename Seq, typename... Ts, size_t... Indices>
		struct take_while_for_impl<Fn, Seq<Ts...>, std::index_sequence<Indices...>>
			: take_while_impl<void, Fn, Seq<>, Seq<Ts...>, std::index_sequence<Indices...>>
		{};

		// immediately end with empty index sequence
		template <typename Fn, template<typename...> typename Seq, typename... Ts>
		struct take_while_for_impl<Fn, Seq<Ts...>, std::index_sequence<>>
			: take_while_impl<void, Fn, Seq<>, Seq<Ts...>, std::index_sequence<>>
		{};

		// immediately end with empty type sequence
		template <typename Fn, template<typename...> typename Seq, size_t... Indices>
		struct take_while_for_impl<Fn, Seq<>, std::index_sequence<Indices...>>
		{
			using type = Seq<>;
		};
	}

	// take items from sequence by unary function
	template <typename Fn, typename Seq, size_t Count>
	using take_while = detail::take_while_for_impl<Fn, Seq, std::make_index_sequence<Count>>;

	template <typename Fn, typename Seq, size_t Count>
	using take_while_t = typename take_while<Fn, Seq, Count>::type;

	// enumerate sequence with a unary function
	template <typename Fn, typename Seq, size_t Count>
		requires meta_invocables<Fn, Seq>
	using repeat_n = detail::repeat_impl<void, Fn, Seq, std::make_index_sequence<Count>>;

	template <typename Fn, typename Seq, size_t Count>
		requires meta_invocables<Fn, Seq>
	using repeat_n_t = typename repeat_n<Fn, Seq, Count>::type;

	// transform a list of lists of elements into a single list containing those elements
	template <typename ListOfLists>
	using unzip = apply<wrap<concat>, ListOfLists>;

	template <typename ListOfLists>
	using unzip_t = typename unzip<ListOfLists>::type;

	template <typename>
	struct _Meta_cartesian_product_;

	// find the n-ary Cartesian Product of the lists in the input list
	template <typename ListOfLists>
	using cartesian_product = typename _Meta_cartesian_product_<ListOfLists>::type;

	template <template <typename...> typename Seq>
	struct _Meta_cartesian_product_<Seq<>>
	{
		using type = Seq<>;
	};

	template <template <typename...> typename Seq0, template <typename...> typename Seq1
		, typename... Ts>
	struct _Meta_cartesian_product_<Seq0<Seq1<Ts...>>>
	{
		using type = Seq0<Seq1<Ts>...>;
	};

	template <template <typename...> class Seq0, template <typename...> class Seq1
		, typename... Ts
		, typename... Rests>
	struct _Meta_cartesian_product_<Seq0<Seq1<Ts...>, Rests...>>
	{
		using Adder = wrap<push_back>;
		using Predicate = bind<Adder, Ts...>;
		using Next = cartesian_product<Seq0<Rests...>>;
		using Merged = Seq0<transform_t<bind<Adder, Ts>, Next>...>;

		using type = typename unzip<Merged>::type;
	};
}

export namespace std
{
	template<size_t Index, typename Fty, typename... Ts>
	struct tuple_element<Index, meta::MetaList<Fty, Ts...>>
	{
		using type = Fty;
	};

	template<size_t Index, typename Fty, typename... Ts>
	struct tuple_element<Index, const meta::MetaList<Fty, Ts...>>
	{
		using type = Fty;
	};

	template<size_t Index>
	struct tuple_element<Index, meta::MetaList<>>
	{
		static_assert(util::always_false<integral_constant<size_t, Index>>, "tuple_element index out of range");
	};

	template<typename... Ts>
	struct tuple_size<meta::MetaList<Ts...>>
		: public integral_constant<size_t, meta::tsize_v<Ts...>>
	{};

	template<typename... Ts>
	struct tuple_size<const meta::MetaList<Ts...>>
		: public integral_constant<size_t, meta::tsize_v<Ts...>>
	{};
}

#pragma warning(push, 1)
namespace meta::test
{
	void test_metafunctions() noexcept
	{
		using test_seq = MetaList<int, float, double>;
		constexpr front_t<test_seq> a000{};
		constexpr back_t<test_seq> a001{};

		constexpr pop_back<test_seq> a002{};
		constexpr pop_back_t<test_seq> t_a002{};

		constexpr push_t<test_seq, long> a010{};
		constexpr front_t<push_t<test_seq, long>> a011{};

		constexpr push_t<long, test_seq> a012{};
		constexpr pop_t<push_t<long, test_seq>> a013{};

		constexpr wrap<front> a014{};
		constexpr wrap<pop> a015{};
		constexpr wrap<pop_back> a016{};
		constexpr wrap<push>::result<test_seq, long>::type a017{};

		constexpr wrap<front>::result<test_seq> _a014{};
		constexpr wrap<pop>::result<test_seq> _a015{};

		constexpr wrap<pop_back>::result<test_seq> _a016{};
		constexpr wrap<pop_back>::result<test_seq>::type t_a016{};

		constexpr wrap<push>::result<test_seq, long> _a017{};
		constexpr wrap<push>::result<test_seq, long>::type t_a017{};
		constexpr wrap<pop>::result<test_seq> a018{};
		constexpr wrap<pop>::result<test_seq>::type t_a018{};
		constexpr wrap<pop>::result<push_t<long, test_seq>> a019{};
		constexpr wrap<pop>::template result<push_t<long, test_seq>>::type t_a019{};

		constexpr apply<wrap<front>, test_seq> a020{};
		constexpr apply_t<wrap<front>, test_seq> a021{};

		constexpr bind<wrap<back>> _a030{};
		constexpr bind<wrap<back>>::result<test_seq> t_a030{};
		constexpr bind<wrap<back>>::result<test_seq>::type tt_a030{};

		constexpr bind<wrap<push>, test_seq> a031{};
		constexpr bind<wrap<push>, test_seq>::result<long> t_a031{};
		constexpr bind<wrap<push>, test_seq>::result<long>::type tt_a031{};

		constexpr bind<wrap<pop_back>, test_seq> a032{};
		constexpr bind<wrap<pop_back>, test_seq>::result<> t_a032{};
		constexpr bind<wrap<pop_back>, test_seq>::result<>::type tt_a032{};

		constexpr apply_t<wrap<front>, test_seq> a041{};
		constexpr apply_t<wrap<back>, test_seq> a042{};
		constexpr apply_t<wrap<pop>, test_seq> a043{};
		constexpr apply_t<wrap<pop_back>, test_seq> a044{};

		using SeqOfSeq = MetaList<test_seq, test_seq, test_seq>;
		constexpr apply_t<wrap<back>, SeqOfSeq> a045{};
		constexpr front_t<SeqOfSeq> a046{};
		constexpr back_t<SeqOfSeq> a047{};

		constexpr pop_back<SeqOfSeq> a048{};
		constexpr pop_back_t<SeqOfSeq> t_a048_0{};
		constexpr pop_t<pop_back_t<SeqOfSeq>> t_a048_1{};

		constexpr back_t<push_t<SeqOfSeq, long>> a049{};

		using NewPusher = wrap<push>;
		constexpr NewPusher::result<test_seq, long>::type pusher_test{};

		using NewBinder = bind<NewPusher, test_seq>;
		constexpr NewBinder::result<long>::type binder_test{};
		constexpr invoke_r<NewBinder, long> invoker_r_test{};

		using NewTransform = transform<NewBinder, test_seq>;
		constexpr NewTransform transform_test{};

		using NewTransformT = typename transform<NewBinder, test_seq>::type;
		constexpr NewTransformT transform_t_test{};

		using NewTransformT_0 = at<NewTransformT, 0>;
		using NewTransformT_0_Int = at<NewTransformT, 0>;
		constexpr NewTransformT_0_Int transform_t0_test{};

		using NewTransformT_1 = at<NewTransformT, 1>;
		using NewTransformT_1_Int = at<NewTransformT, 1>;
		constexpr NewTransformT_1_Int transform_t1_test{};

		using NewTransformT_2 = at<NewTransformT, 2>;
		using NewTransformT_2_Int = at<NewTransformT, 2>;
		constexpr NewTransformT_2_Int transform_t2_test{};

		//using NewTransformT_3 = typename at<NewTransformT, 3>;
		//using NewTransformT_3_Int = typename at<NewTransformT, 3>::MetaList;
		//constexpr NewTransformT_3_Int transform_t3_test{};

		using NewSeq = typename transform<NewBinder, test_seq>::type;
		constexpr NewSeq a050{};

		//constexpr pop_back<NewSeq> a051{};
		//using t_a051_t = typename pop_back<NewSeq>::type;
		//constexpr t_a051_t t_a051{};

		//constexpr front<NewSeq> a052{};
		//using t_a052_t_0 = typename front<NewSeq>::type;
		//constexpr t_a052_t_0 t_a052_0{};
		//using t_a052_t_0_0 = typename front<front<NewSeq>>::type;
		//constexpr t_a052_t_0_0 t_a052_0_0{};
		//using t_a052_t_0_0_0 = typename front<front<front<NewSeq>>>::type;
		//constexpr t_a052_t_0_0_0 t_a052_0_0_0{};
		//using t_a052_t_0_0_0_0 = typename front<front<front<front<NewSeq>>>>::type;
		//constexpr t_a052_t_0_0_0_0 t_a052_0_0_0_0{};

		//static_assert(std::is_same_v<int, t_a052_t>, "t_a052 is not int");
		//static_assert(std::is_same_v<int, t_a052_t_0>, "t_a052_0 is not int");

		constexpr back_t<front_t<NewSeq>> a053_0{};
		constexpr front_t<back_t<NewSeq>> a054_1{};

		constexpr pop_back<transform_t<bind<wrap<push>, test_seq>, test_seq>> a055{};
		constexpr pop_back_t<transform_t<bind<wrap<push>, test_seq>, test_seq>> t_a055{};
		constexpr back_t<pop_back_t<transform_t<bind<wrap<push>, test_seq>, test_seq>>> a056{};
		constexpr front_t<pop_back_t<transform_t<bind<wrap<push>, test_seq>, test_seq>>> a057{};
		constexpr back_t<pop_back_t<transform_t<bind<wrap<push>, test_seq>, test_seq>>> a058{};
		constexpr back_t<back_t<pop_back_t<transform_t<bind<wrap<push>, test_seq>, test_seq>>>> a059{};

		constexpr create<5, int, MetaList> a060{};

		constexpr detail::__at<test_seq, 0> a070{};
		constexpr at<test_seq, 0> r_a070{};
		constexpr detail::__at<test_seq, 1> a071{};
		constexpr at<test_seq, 1> r_a071{};
		constexpr detail::__at<test_seq, 2>::type a072{};
		constexpr at<test_seq, 2> r_a072{};
		//constexpr __at<test_seq, 3> a073{};
		//constexpr at<test_seq, 3> r_a073{};

		constexpr unzip<test_seq> a074{};
		constexpr unzip_t<test_seq> t_a074{};
		constexpr unzip<SeqOfSeq> a075{};
		constexpr unzip_t<SeqOfSeq> t_a075{};
		constexpr unzip<transform_t<bind<wrap<push>, test_seq>, test_seq>> a076{};
		constexpr unzip_t<transform_t<bind<wrap<push>, test_seq>, test_seq>> t_a076{};

		constexpr cartesian_product<NewSeq> a080{};
		using Product = cartesian_product<NewSeq>;

		constexpr at<Product, 0> r_a080_at0{};
		constexpr at<at<Product, 0>, 0> r_a080_at0_at0{};

		constexpr at<at<at<Product, 0>, 0>, 0>        r_a080_at0_at0_at0{};
		//constexpr at<at<at<at<Product, 0>, 0>, 0>, 0> r_a080_at0_at0_at0_at0{};
		//constexpr at<at<at<at<Product, 0>, 0>, 0>, 1> r_a080_at0_at0_at0_at1{};

		constexpr at<at<at<Product, 0>, 0>, 1>        r_a080_at0_at0_at1{};
		constexpr at<at<at<at<Product, 0>, 0>, 1>, 0> r_a080_at0_at0_at1_at0{};
		constexpr at<at<at<at<Product, 0>, 0>, 1>, 1> r_a080_at0_at0_at1_at1{};

		constexpr at<at<Product, 0>, 1> r_a080_at0_at1{};
		constexpr at<at<Product, 0>, 2> r_a080_at0_at2{};
		constexpr at<at<Product, 0>, 3> r_a080_at0_at3{};

		constexpr at<Product, 1> r_a080_at1{};
		constexpr at<at<Product, 1>, 0> r_a080_at1_at0{};
		constexpr at<at<Product, 1>, 1> r_a080_at1_at1{};
		constexpr at<at<Product, 1>, 2> r_a080_at1_at2{};
		constexpr at<at<Product, 1>, 3> r_a080_at1_at3{};

		constexpr at<Product, 2> r_a080_at2{};
		constexpr at<at<Product, 2>, 0> r_a080_at2_at0{};
		constexpr at<at<Product, 2>, 1> r_a080_at2_at1{};
		constexpr at<at<Product, 2>, 2> r_a080_at2_at2{};
		constexpr at<at<Product, 2>, 3> r_a080_at2_at3{};

		constexpr at<Product, 3> r_a080_at3{};
		constexpr at<at<Product, 3>, 0> r_a080_at3_at0{};
		constexpr at<at<Product, 3>, 1> r_a080_at3_at1{};
		constexpr at<at<Product, 3>, 2> r_a080_at3_at2{};
		constexpr at<at<Product, 3>, 3> r_a080_at3_at3{};

		//constexpr at<Product, 4> r_a080_at4{};
		//constexpr at<Product, 5> r_a080_at5{};
		//constexpr at<Product, 6> r_a080_at6{};
		//constexpr at<Product, 7> r_a080_at7{};
		//constexpr at<Product, 8> r_a080_at8{};
		//constexpr at<Product, 9> r_a080_at9{};

		constexpr front_t<at<Product, 0>> r_a080_at0_f{};
		constexpr front_t<front_t<at<Product, 0>>> r_a080_at0_ff{};
		//constexpr at<front_t<front_t<at<Product, 0>>>, 0> r_a080_at0_ff_at0{};
		//constexpr at<front_t<front_t<at<Product, 0>>>, 1> r_a080_at0_ff_at1{};

		constexpr back_t<front_t<at<Product, 0>>> r_a080_at0_bf{};
		//constexpr at<back_t<front_t<at<Product, 0>>>, 0> r_a080_at0_bf_at0{};
		//constexpr at<back_t<front_t<at<Product, 0>>>, 1> r_a080_at0_bf_at1{};

		constexpr front_t<front_t<at<Product, 1>>> r_a080_at1_ff{};
		//constexpr at<front_t<front_t<at<Product, 1>>>, 0> r_a080_at1_ff_at0{};
		//constexpr at<front_t<front_t<at<Product, 1>>>, 1> r_a080_at1_ff_at1{};

		constexpr front_t<front_t<at<Product, 2>>> r_a080_at2_ff{};
		//constexpr at<front_t<front_t<at<Product, 2>>>, 0> r_a080_at2_ff_at0{};
		//constexpr at<front_t<front_t<at<Product, 2>>>, 1> r_a080_at2_ff_at1{};

		//constexpr front_t<front_t<front_t<at<Product, 0>>>> r_a080_at0_fff{};

		// list of list
		constexpr front_t<cartesian_product<NewSeq>> a080_f{};
		// int, float
		constexpr front_t<front_t<cartesian_product<NewSeq>>> a080_ff{};
		// int
		constexpr front_t<front_t<front_t<cartesian_product<NewSeq>>>> a080_fff{};
	}

	void test_repeats() noexcept
	{
		using test_rp_list = MetaList<int, float, double>;
		using test_fullrange = std::make_index_sequence<tsize_v<test_rp_list>>;
		using test_range_0 = std::make_index_sequence<0>;
		using test_range_1 = std::make_index_sequence<1>;
		using test_range_2 = std::make_index_sequence<2>;
		using test_range_3 = std::make_index_sequence<3>;

		constexpr size_t sz_klist = tsize_v<test_rp_list>;

		// �̰� �Ƕ��� ���� ����
		//invoke_r<wrap<pop>, test_rp_list>;

		using en0 = detail::repeat_impl<void, wrap<pop>, test_rp_list, test_range_0>;

		using en0_t = en0::type;
		using en0_top = front_t<en0_t>;
		using en0_bot = back_t<en0_t>;

		using en0_alt = repeat_n_t<wrap<pop>, test_rp_list, 0>;
		using en0_alt_top = front_t<en0_alt>;
		using en0_alt_bot = back_t<en0_alt>;

		static_assert(std::is_same_v<en0_t, MetaList<int, float, double>>, "1");
		static_assert(std::is_same_v<en0_alt, MetaList<int, float, double>>, "1");

		using en1 = repeat_n<wrap<pop>, test_rp_list, 1>;
		using en1_t = typename repeat_n<wrap<pop>, test_rp_list, 1>::type;

		using en1_alt = detail::repeat_impl<void, wrap<pop>, test_rp_list, test_range_1>;
		using en1_alt_t = typename en1_alt::type;

		using raw_en1 = invoke_r<bind<wrap<pop>, test_rp_list>>;

		static_assert(std::is_same_v<en1_t, MetaList<float, double>>, "1");
		using en1_top = front_t<en1_t>;
		using en1_bot = back_t<en1_t>;

		using en2 = detail::repeat_impl<void, wrap<pop>, test_rp_list, test_range_2>;
		using en2_t_alt = typename en2::type;
		using en2_t = detail::repeat_impl<void, wrap<pop>, test_rp_list, test_range_2>::type;
		using raw_en2 = invoke_r<bind<wrap<pop>, raw_en1>>;

		static_assert(std::is_same_v<en2_t, MetaList<double>>, "2");
		using en2_top = front_t<en2_t>;
		using en2_bot = back_t<en2_t>;

		using en3 = detail::repeat_impl<void, wrap<pop>, test_rp_list, test_range_3>;
		using en3_t = en3::type;

		static_assert(std::is_same_v<en3_t, MetaList<>>, "3");
		//using en3_top = front_t<en3_t>;
		//using en3_bot = back_t<en3_t>;

		using en4_t = detail::repeat_impl<void, wrap<pop>, test_rp_list, test_fullrange>::type;

		static_assert(std::is_same_v<en4_t, MetaList<>>, "4");
		//using en4_top = front_t<en4_t>;
		//using en4_bot = back_t<en4_t>;

		constexpr size_t seek_int = seek_range<int, test_rp_list>;
		constexpr size_t seek_float = seek_range<float, test_rp_list>;
		constexpr size_t seek_double = seek_range<double, test_rp_list>;
		//constexpr size_t seek_char = seek_range<char, test_rp_list>;

		using local_fn = wrap<pop>;
		using local_rp_pop = repeat_n<local_fn, test_rp_list, seek_float + 1>;
		using local_rp_pop_t = typename local_rp_pop::type;
	}

	void test_enumerators() noexcept
	{
		using test_rp_list = MetaList<unsigned long long, void, float, int, bool, unsigned>;
		using test_fullrange = std::make_index_sequence<tsize_v<test_rp_list>>;
		using test_range_0 = std::make_index_sequence<0>;
		using test_range_1 = std::make_index_sequence<1>;
		using test_range_2 = std::make_index_sequence<2>;
		using test_range_3 = std::make_index_sequence<3>;

		using taken_fn1 = wrap<pop>;
		using fn1_r = invoke_r<taken_fn1, test_rp_list>;

		using taken_fn2 = bind<taken_fn1, test_rp_list>;
		using fn2_r = invoke_r<taken_fn2>;

		static_assert(std::is_same_v<fn1_r, fn2_r>, "functors");

		//using takw_r0 = take_while<taken_fn2, test_rp_list, 0>;
		//using takw_r0_t = take_while_t<taken_fn2, test_rp_list, 0>;
		//takw_r0::type;

		//using takw_r1 = take_while<taken_fn2, test_rp_list, 1>;
		//using takw_r1_t = take_while_t<taken_fn2, test_rp_list, 1>;
		//takw_r1::type;
	}
}
#pragma warning(pop)
