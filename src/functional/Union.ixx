export module Utility.Union;
import <variant>;
import Utility;
import Utility.Constraints;
import Utility.Meta;

namespace util::detail
{
	template <typename Indexer = integral_constant<size_t, 0>, typename... Ts>
	class PlacedVariant;

	template <size_t Place>
	class PlacedVariant<integral_constant<size_t, Place>>
	{};

	struct void_guard { explicit constexpr void_guard() noexcept = default; };

	template <typename Variant>
	struct get_next;

	template <size_t Place, typename T, typename... Ts>
	struct get_next<PlacedVariant<integral_constant<size_t, Place>, T, Ts...>>
	{
		using type = PlacedVariant<integral_constant<size_t, Place + 1>, Ts...>;
	};

	template <size_t End>
	struct get_next<PlacedVariant<integral_constant<size_t, End>>>
	{};

	template <size_t Index, typename Variant>
	struct get_node_at;

	template <size_t Index, size_t Place, typename T, typename... Ts>
	struct get_node_at<Index, PlacedVariant<integral_constant<size_t, Place>, T, Ts...>>
	{
		using type = typename get_node_at<Index - 1, PlacedVariant<integral_constant<size_t, Place + 1>, Ts...>>::type;
	};

	template <size_t Place, typename T, typename... Ts>
	struct get_node_at<0, PlacedVariant<integral_constant<size_t, Place>, T, Ts...>>
	{
		using type = PlacedVariant<integral_constant<size_t, Place>, T, Ts...>;
	};

	template <size_t Place, typename Fty, typename... Rty>
	class PlacedVariant<integral_constant<size_t, Place>, Fty, Rty...>
	{
	public:
		static_assert(!same_as<Fty, nullopt_t>, "Fty must not be nullopt_t.");
		static_assert(!same_as<Fty, in_place_t>, "Fty must not be in_place_t.");
		static_assert(!is_specialization_v<Fty, in_place_type_t>, "Fty must not be in_place_type_t.");
		static_assert(!is_indexed_v<Fty, in_place_index_t>, "Fty must not be in_place_index_t.");

		using impl_type = conditional_t<notvoids<Fty>, remove_const_t<Fty>, void_guard>;
		using node_type = PlacedVariant<integral_constant<size_t, Place>, Fty, Rty...>;
		using next_type = typename get_next<node_type>::type;

		static inline constexpr size_t mySize = 1 + sizeof...(Rty);
		static inline constexpr size_t myPlace = Place;

		template <size_t Index>
		static inline constexpr size_t relativeIndex = Index - Place;

		template <size_t Index>
		using element_type = std::tuple_element_t<relativeIndex<Index>, std::tuple<Fty, Rty...>>;
		template <size_t Index>
		using value_type = element_type<Index>;
		template <size_t Index>
		using const_value_type = add_const_t<value_type<Index>>;
		template <size_t Index>
		using reference_type = add_lvalue_reference_t<value_type<Index>>;
		template <size_t Index>
		using const_reference_type = add_lvalue_reference_t<value_type<Index>>;
		template <size_t Index>
		using rvalue_type = add_rvalue_reference_t<value_type<Index>>;
		template <size_t Index>
		using const_rvalue_type = add_rvalue_reference_t<value_type<Index>>;

		template <size_t Index, template<size_t> typename Indexer>
		static inline constexpr bool nothrowCopyPursuer = (Index == myPlace && nothrow_copy_constructibles<Fty>)
			|| (Index != myPlace && 1 < mySize && nothrow_copy_constructibles<Indexer<Index>>);

		template <size_t Index, template<size_t> typename Indexer>
		static inline constexpr bool nothrowMovePursuer = (Index == myPlace && nothrow_move_constructibles<Fty>)
			|| (Index != myPlace && 1 < mySize && nothrow_move_constructibles<Indexer<Index>>);

		// no initialization (no active member)
		constexpr PlacedVariant() noexcept
		{}

		// no initialization (no active member)
		constexpr PlacedVariant(nullopt_t) noexcept
		{}

		// Initialize my value with Args (not void)
		template <typename... Args>
			requires (notvoids<Fty>)
		constexpr PlacedVariant(in_place_t, Args&&... args)
			noexcept(nothrow_constructibles<Fty, Args...>)
			: myValue(static_cast<Args&&>(args)...)
			, hasValue(true)
		{}

		// Initialize my value with Args (void)
		template <typename... Args>
			requires (!notvoids<Fty>)
		constexpr PlacedVariant(in_place_t, Args&&... args)
			noexcept(nothrow_constructibles<Fty, Args...>)
			: voidData()
			, hasValue(true)
		{}

		// Initialize my value with Args by a Index (not void)
		template <typename... Args>
			requires (notvoids<Fty>)
		constexpr PlacedVariant(in_place_index_t<Place>, Args&&... args)
			noexcept(nothrow_constructibles<Fty, Args...>)
			: PlacedVariant(in_place, static_cast<Args&&>(args)...)
		{}

		// Initialize my value with Args by a Index (void)
		template <typename... Args>
			requires (!notvoids<Fty>)
		constexpr PlacedVariant(in_place_index_t<Place>, Args&&... args)
			noexcept
			: PlacedVariant(in_place, nullopt)
		{}

		// Initialize my value with Args by a Type (not void)
		template <typename T, typename... Args>
			requires (same_as<clean_t<T>, Fty>&& notvoids<Fty>)
		explicit(is_explicit_constructible_v<T>)
			constexpr
			PlacedVariant(in_place_type_t<T>, Args&&... args)
			noexcept(nothrow_constructibles<Fty, Args...>)
			: PlacedVariant(in_place, static_cast<Args&&>(args)...)
		{}

		// Initialize my value with Args by a Type (void)
		template <typename T, typename... Args>
			requires (same_as<clean_t<T>, Fty> && !notvoids<Fty>)
		explicit constexpr
			PlacedVariant(in_place_type_t<T>, Args&&... args)
			noexcept
			: PlacedVariant(in_place, nullopt)
		{}

		// Recursively find the place onto Tail
		template <size_t Target, typename... Args>
			requires (Target < Place + 1 + sizeof...(Rty))
		explicit constexpr
			PlacedVariant(in_place_index_t<Target>, Args&&... args)
			noexcept(nothrow_constructibles<next_type, in_place_index_t<Target>, Args...>)
			: _Tail(in_place_index<Target>, static_cast<Args&&>(args)...)
			, isExtended(true)
		{}

		// Recursively find the specified type
		template <typename T, typename... Args>
			requires (!same_as<clean_t<T>, Fty>&& meta::included_v<T, Fty, Rty...>)
		explicit
			constexpr
			PlacedVariant(in_place_type_t<T>, Args&&... args)
			noexcept(nothrow_constructibles<T, Args...>)
			: _Tail(in_place_type<T>, static_cast<Args&&>(args)...)
			, isExtended(true)
		{}

		// When cannot find the place onto Tail
		template <size_t Target, typename... Args>
			requires (Place + 1 + sizeof...(Rty) <= Target)
		explicit constexpr PlacedVariant(in_place_index_t<Target>, Args&&... args)
		{
			static_assert(always_false<in_place_index_t<Target>>, "Target index is out of range.");
		}

		// When cannot find the specified type
		template <typename T, typename... Args>
			requires (!same_as<clean_t<T>, Fty> && !meta::included_v<T, Fty, Rty...>)
		explicit
			constexpr
			PlacedVariant(in_place_type_t<T>, Args&&... args)
		{
			static_assert(always_false<in_place_type_t<T>>, "Target type is not included.");
		}

		constexpr ~PlacedVariant()
			noexcept(nothrow_destructibles<Fty, Rty...>)
			requires(trivially_destructibles<Fty, Rty...>)
		= default;

		constexpr ~PlacedVariant()
			noexcept(nothrow_destructibles<Fty, Rty...>)
			requires(!trivially_destructibles<Fty, Rty...>)
		{}

		constexpr PlacedVariant& operator=(nullopt_t) noexcept
		{
			hasValue = false;
			return *this;
		}

		// getter (not void)
		[[nodiscard]]
		constexpr make_lvalue_t<Fty> value() &
			noexcept(nothrow_copy_constructibles<Fty>)
		{
			static_assert(!same_as<Fty, void>, "Cannot get void type.");

			return myValue;
		}

		// getter (not void)
		[[nodiscard]]
		constexpr make_clvalue_t<Fty> value() const&
			noexcept(nothrow_copy_constructibles<const Fty>)
		{
			static_assert(!same_as<Fty, void>, "Cannot get void type.");

			return myValue;
		}

		// getter (not void)
		[[nodiscard]]
		constexpr make_rvalue_t<Fty> value() &&
			noexcept(nothrow_move_constructibles<Fty>)
		{
			static_assert(!same_as<Fty, void>, "Cannot get void type.");

			return static_cast<Fty&&>(myValue);
		}

		// getter (not void)
		[[nodiscard]]
		constexpr make_crvalue_t<Fty> value() const&&
			noexcept(nothrow_move_constructibles<const Fty>)
		{
			static_assert(!same_as<Fty, void>, "Cannot get void type.");

			return static_cast<const Fty&&>(myValue);
		}

		// index getter
		template <size_t Index>
		[[nodiscard]]
		constexpr decltype(auto)
			get() &
			//noexcept(nothrowCopyPursuer<Index, value_type>)
			noexcept(Index == Place)
		{
			if constexpr (Index == Place)
			{
				return value();
			}
			else if constexpr (1 < mySize)
			{
				if (isExtended)
				{
					return _Tail.template get<Index>();
				}
				else
				{
					//std::unreachable();
					throw std::bad_variant_access{};
				}
			}
			else
			{
				static_assert(always_false<Fty>, "This Monad does not have the indexed type.");
			}
		}

		// index getter
		template <size_t Index>
		[[nodiscard]]
		constexpr decltype(auto)
			get() const&
			//noexcept(nothrowCopyPursuer<Index, const_value_type>)
			noexcept(Index == Place)
		{
			if constexpr (Index == Place)
			{
				return value();
			}
			else if constexpr (1 < mySize)
			{
				if (isExtended)
				{
					return _Tail.template get<Index>();
				}
				else
				{
					//std::unreachable();
					throw std::bad_variant_access{};
				}
			}
			else
			{
				static_assert(always_false<Fty>, "This Monad does not have the indexed type.");
			}
		}

		// index getter
		template <size_t Index>
		[[nodiscard]]
		constexpr decltype(auto)
			get() &&
			//noexcept(nothrowMovePursuer<Index, value_type>)
			noexcept(Index == Place)
		{
			if constexpr (Index == Place)
			{
				return move(*this).value();
			}
			else if constexpr (1 < mySize)
			{
				if (isExtended)
				{
					return move(_Tail).template get<Index>();
				}
				else
				{
					//std::unreachable();
					throw std::bad_variant_access{};
				}
			}
			else
			{
				static_assert(always_false<Fty>, "This Monad does not have the indexed type.");
			}
		}

		// index getter
		template <size_t Index>
		[[nodiscard]]
		constexpr decltype(auto)
			get() const&&
			//noexcept(nothrowMovePursuer<Index, const_value_type>)
			noexcept(Index == Place)
		{
			if constexpr (Index == Place)
			{
				return move(*this).value();
			}
			else if constexpr (1 < mySize)
			{
				if (isExtended)
				{
					return move(_Tail).template get<Index>();
				}
				else
				{
					//std::unreachable();
					throw std::bad_variant_access{};
				}
			}
			else
			{
				static_assert(always_false<Fty>, "This Monad does not have the indexed type.");
			}
		}

		// type getter
		template <typename T>
			requires (meta::included_v<T, Fty, Rty...>)
		[[nodiscard]]
		constexpr decltype(auto)
			get()&
		{
			if constexpr (same_as<T, Fty>)
			{
				return value();
			}
			else if constexpr (1 < mySize)
			{
				if (isExtended)
				{
					return _Tail.template get<T>();
				}
				else
				{
					//std::unreachable();
					throw std::bad_variant_access{};
				}
			}
			else
			{
				static_assert(always_false<Fty>, "This Monad does not have the indexed type.");
			}
		}

		// type getter
		template <typename T>
			requires (meta::included_v<T, Fty, Rty...>)
		[[nodiscard]]
		constexpr decltype(auto)
			get() const&
		{
			if constexpr (same_as<T, Fty>)
			{
				return value();
			}
			else if constexpr (1 < mySize)
			{
				if (isExtended)
				{
					return _Tail.template get<T>();
				}
				else
				{
					//std::unreachable();
					throw std::bad_variant_access{};
				}
			}
			else
			{
				static_assert(always_false<Fty>, "This Monad does not have the indexed type.");
			}
		}

		// type getter
		template <typename T>
			requires (meta::included_v<T, Fty, Rty...>)
		[[nodiscard]]
		constexpr decltype(auto)
			get()&&
		{
			if constexpr (same_as<T, Fty>)
			{
				return move(*this).value();
			}
			else if constexpr (1 < mySize)
			{
				if (isExtended)
				{
					return move(_Tail).template get<T>();
				}
				else
				{
					//std::unreachable();
					throw std::bad_variant_access{};
				}
			}
			else
			{
				static_assert(always_false<Fty>, "This Monad does not have the indexed type.");
			}
		}

		// type getter
		template <typename T>
			requires (meta::included_v<T, Fty, Rty...>)
		[[nodiscard]]
		constexpr decltype(auto)
			get() const&&
		{
			if constexpr (same_as<T, Fty>)
			{
				return move(*this).value();
			}
			else if constexpr (1 < mySize)
			{
				if (isExtended)
				{
					return move(_Tail).template get<T>();
				}
				else
				{
					//std::unreachable();
					throw std::bad_variant_access{};
				}
			}
			else
			{
				static_assert(always_false<Fty>, "This Monad does not have the indexed type.");
			}
		}

		// unsafe
		template<typename Uty>
		constexpr void set(Uty&& value)
			noexcept(nothrow_assignables<util::add_rvalue_reference_t<Uty>, Fty>)
		{
			myValue = static_cast<Uty&&>(value);
			hasValue = true;
		}

		template <size_t Index, typename Uty>
			requires (Index == Place)
		constexpr PlacedVariant& try_set(Uty&& value)
			noexcept(nothrow_assignables<util::add_rvalue_reference_t<Uty>, Fty>)
		{
			static_assert(!same_as<Fty, void>, "Cannot set the value of a void type.");
			static_assert(!assignable_from<Uty, Fty>, "Uty is not assignable to Fty.");

			set(static_cast<Uty&&>(value));

			return *this;
		}

		template <size_t Index, typename Uty>
			requires (Index != Place)
		constexpr
			typename get_node_at<relativeIndex<Index>, node_type>::type&
			try_set(Uty&& value)
		{
			if constexpr (1 < mySize && Index <= Place + mySize)
			{
				return _Tail.template try_set<Index>(static_cast<Uty&&>(value));
			}
			else
			{
				static_assert(always_false<Uty>, "This Monad does not have the indexed type.");
			}
		}

		template <typename... Args>
		constexpr PlacedVariant& emplace(Args&&... args)
			noexcept(nothrow_constructibles<Fty, Args&&...>)
		{
			myValue = Fty{ forward<Args>(args)... };
			hasValue = true;

			return *this;
		}

		constexpr void reset() noexcept
		{
			if constexpr (1 < mySize)
			{
				if (isExtended)
				{
					_Tail.reset();
				}
			}

			hasValue = false;
			isExtended = false;
		}

		[[nodiscard]]
		constexpr bool has_value() const noexcept
		{
			return hasValue;
		}

		template <size_t Index>
			requires (Index == Place)
		[[nodiscard]]
		constexpr bool has_value() const noexcept
		{
			return hasValue;
		}

		template <size_t Index>
			requires (Place < Index)
		[[nodiscard]]
		constexpr bool has_value() const noexcept
		{
			if constexpr (1 < mySize)
			{
				if (isExtended)
				{
					try
					{
						return _Tail.template has_value<Index>();
					}
					catch (...)
					{
						return false;
					}
				}
				else
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}

		template <typename T>
			requires (same_as<clean_t<T>, Fty>)
		[[nodiscard]]
		constexpr bool has_value() const noexcept
		{
			return hasValue;
		}

		template <typename T>
			requires (!same_as<clean_t<T>, Fty>)
		[[nodiscard]]
		constexpr bool has_value() const noexcept
		{
			if constexpr (1 < mySize)
			{
				if (isExtended)
				{
					try
					{
						return _Tail.template has_value<T>();
					}
					catch (...)
					{
						return false;
					}
				}
				else
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}

		template <size_t Index>
			requires (Index == Place)
		[[nodiscard]]
		constexpr bool is_valueless() const noexcept
		{
			return !hasValue;
		}

		template <size_t Index>
			requires (Place < Index)
		[[nodiscard]]
		constexpr bool is_valueless() const noexcept
		{
			if constexpr (1 < mySize)
			{
				if (isExtended)
				{
					try
					{
						return _Tail.template is_valueless<Index>();
					}
					catch (...)
					{
						return false;
					}
				}
				else
				{
					return false;
				}
			}
			else
			{
				return true;
			}
		}

		constexpr PlacedVariant(const PlacedVariant& other) noexcept
		{
			(*this).operator=(other);
		}

		constexpr PlacedVariant(PlacedVariant&& other) noexcept
		{
			(*this).operator=(static_cast<PlacedVariant&&>(other));
		}

		constexpr PlacedVariant& operator=(const PlacedVariant& other) noexcept
		{
			// make empty itself
			reset();

			if (other.hasValue)
			{
				myValue = other.myValue;
				hasValue = true;
			}
			else if (other.isExtended)
			{
				_Tail.operator=(other._Tail);
			}

			return *this;
		}

		constexpr PlacedVariant& operator=(PlacedVariant&& other) noexcept
		{
			// make empty itself
			reset();

			if (other.hasValue)
			{
				myValue = static_cast<Fty&&>(other.myValue);
				hasValue = true;
			}
			else if (other.isExtended)
			{
				_Tail.operator=(move(other._Tail));
			}

			return *this;
		}

	private:
		union
		{
			union
			{
				std::monostate voidData;
				impl_type myValue;
			};
			next_type _Tail;
		};

		bool hasValue = false;
		bool isExtended = false;
	};

	using ::std::integral_constant;
	using FirstIndexer = integral_constant<size_t, 0>;

	struct hide_variant0
	{
		template <size_t I, typename... Ts>
		using type = PlacedVariant<integral_constant<size_t, I>, Ts...>;
	};

	struct hide_variant1
	{
		template <typename... Ts>
		using type = typename hide_variant0::template type<0, Ts...>;
	};

	struct hide_variant2
	{
		template <size_t I, typename... Ts>
		using type = typename hide_variant0::template type<I, Ts...>;
	};
}

export namespace util
{
	//template <typename... Ts>
	//using Union = typename detail::hide_variant1::template type<Ts...>;

	//template <size_t I, typename... Ts>
	//using DerivedUnion = typename detail::hide_variant2::template type<I, Ts...>;

	template <typename... Ts>
	using Union = typename detail::PlacedVariant<detail::FirstIndexer, Ts...>;

	template <size_t I, typename... Ts>
	using DerivedUnion = typename detail::PlacedVariant<integral_constant<size_t, I>, Ts...>;
}

export namespace std
{
	template<size_t Index, typename... Ts>
	struct variant_alternative<Index, util::Union<Ts...>>
	{
		using type = meta::at<util::Union<Ts...>, Index>;
	};

	template<typename... Ts>
	struct variant_size<util::Union<Ts...>>
		: integral_constant<size_t, sizeof...(Ts)>
	{};

	template<size_t Index, typename... Ts>
	constexpr decltype(auto)
		get(util::Union<Ts...>& _Val)
		noexcept(noexcept(_Val.template get<Index>()))
	{
		return _Val.template get<Index>();
	}

	template<size_t Index, typename... Ts>
	constexpr decltype(auto)
		get(const util::Union<Ts...>& _Val)
		noexcept(noexcept(_Val.template get<Index>()))
	{
		return _Val.template get<Index>();
	}

	template<size_t Index, typename... Ts>
	constexpr decltype(auto)
		get(util::Union<Ts...>&& _Val)
		noexcept(noexcept(move(_Val).template get<Index>()))
	{
		return move(_Val).template get<Index>();
	}

	template<size_t Index, typename... Ts>
	constexpr decltype(auto)
		get(const util::Union<Ts...>&& _Val)
		noexcept(noexcept(move(_Val).template get<Index>()))
	{
		return move(_Val).template get<Index>();
	}

	template<typename T, typename... Ts>
	constexpr decltype(auto)
		get(util::Union<Ts...>& _Val)
		noexcept(noexcept(_Val.template get<T>()))
	{
		return _Val.template get<T>();
	}

	template<typename T, typename... Ts>
	constexpr decltype(auto)
		get(const util::Union<Ts...>& _Val)
		noexcept(noexcept(_Val.template get<T>()))
	{
		return _Val.template get<T>();
	}

	template<typename T, typename... Ts>
	constexpr decltype(auto)
		get(util::Union<Ts...>&& _Val)
		noexcept(noexcept(move(_Val).template get<T>()))
	{
		return move(_Val).template get<T>();
	}

	template<typename T, typename... Ts>
	constexpr decltype(auto)
		get(const util::Union<Ts...>&& _Val)
		noexcept(noexcept(move(_Val).template get<T>()))
	{
		return move(_Val).template get<T>();
	}
}

#pragma warning(push, 0)
namespace util::test
{
#if false
	void test_union() noexcept
	{
		using aa_t = Union<int, void, unsigned long, float>;

		constexpr aa_t aa{ in_place_type<unsigned long>, 600UL };
		using aa_0_t = aa_t::element_type<0>;
		static_assert(util::is_same_v<aa_0_t, int>, "int");
		using aa_1_t = aa_t::element_type<1>;
		//static_assert(is_same_v<aa_1_t, int>, "unsigned long");
		using aa_2_t = aa_t::element_type<2>;
		//static_assert(is_same_v<aa_2_t, int>, "float");
		//using aa_3_t = aa_t::element_type<3>;

		constexpr aa_0_t aa_ty_0_v = 0;
		//constexpr aa_1_t aa_ty_1_v;
		constexpr aa_2_t aa_ty_2_v = 0;

		aa.myPlace;
		aa.mySize;
		aa.relativeIndex<0>;
		aa.relativeIndex<1>;
		aa.relativeIndex<2>;
		aa.relativeIndex<3>;

		constexpr bool a_has_0 = aa.has_value<0>();
		constexpr bool a_has_1 = aa.has_value<1>();
		constexpr bool a_has_2 = aa.has_value<2>();
		constexpr bool a_has_3 = aa.has_value<3>();
		aa.~PlacedVariant();

		//constexpr Union<int> aa2{ in_place_type<unsigned long>, 600UL };
		constexpr Union<int> aa2{ in_place_type<int>, 600UL };

		using bb_t = Union<int, unsigned long, float, double, void, long long>;
		bb_t bb0{};
		//bb_t bb1{};
		bb_t bb1(in_place_type<float>, 4000.034124f);
		bb0.set(0);

		bb0 = bb1;
		bb0.reset();

		const auto getter0 = bb0.get<0>();
		const auto getter1 = bb0.get<1>();
		const auto getter2 = bb0.get<2>();

		// PlacedVariant<integral_constant<size_t, 0>, int, unsigned long, float, double>
		auto& rb0_tset_0 = bb0.try_set<0>(0);
		// PlacedVariant<integral_constant<size_t, 1>, unsigned long, float, double>
		auto& rb0_tset_1 = bb0.try_set<1>(500UL);
		decltype(auto) rb0_tset_2 = bb0.try_set<2>(500.0f);
		decltype(auto) rb0_tset_3 = bb0.try_set<3>(500.0);
		//decltype(auto) rb0_tset_4 = bb0.try_set<4>(0);
		decltype(auto) rb0_tset_5 = bb0.try_set<5>(0);
		//decltype(auto) rb0_tset_4 = bb0.try_set<4>(500.0);
		//decltype(auto) rb0_tset_5 = bb0.try_set<5>(500.0);

		constexpr Union<int, int, int> cc{};
		constexpr Union<bool, int, long> dd{};
		constexpr Union<float, unsigned long long, char> ee{};

		constexpr Union<double, unsigned char, short> ff{ in_place_type<unsigned char>, 'F' };
		const auto cgetter0 = ff.get<0>();
		const auto cgetter1 = ff.get<1>();
		const auto cgetter2 = ff.get<2>();
		//const auto cgetter3 = ff.get<3>();
	}
#endif // false
}
#pragma warning(pop)
