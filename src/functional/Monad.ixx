export module Utility.Monad;
export import Utility;
import Utility.Traits;
import Utility.Constraints;
import Utility.Identity;

export namespace util
{
	template<typename T>
	class [[nodiscard]] Monad;

	template<notvoids T>
	class [[nodiscard]] Monad<T>
	{
	public:
		static_assert(!same_as<T, nullopt_t>, "T must not be nullopt_t.");

		using base_type = T;
		using value_type = Identity<base_type>;

		template<typename U>
		using rebind_base = Identity<U>;
		template<typename U>
		using rebind = Monad<U>;

		constexpr Monad()
			noexcept(nothrow_constructibles<T>)
			: myStorage()
		{}

		constexpr Monad(const Monad& other)
			noexcept(nothrow_copy_constructibles<T>) requires(copy_constructibles<T>)
			: myStorage(other.myStorage)
			, hasValue(other.hasValue)
		{}

		constexpr Monad(Monad&& other)
			noexcept(nothrow_move_constructibles<T>) requires(move_constructibles<T>)
			: myStorage(static_cast<rebind_base<T>&&>(other.myStorage))\
			, hasValue(static_cast<bool&&>(other.hasValue))
		{}

		constexpr Monad(nullopt_t)
			noexcept(nothrow_constructibles<T>)
			: myStorage()
		{}

		constexpr Monad(const T& fwd)
			noexcept(nothrow_copy_constructibles<T>)
			: myStorage(fwd)
			, hasValue(true)
		{}

		constexpr Monad(T&& fwd)
			noexcept(nothrow_move_constructibles<T>)
			: myStorage(static_cast<T&&>(fwd))
			, hasValue(true)
		{}

		constexpr Monad& operator=(const Monad& other)
			noexcept(nothrow_copy_assignables<T>) requires(copy_assignables<T>)
		{
			myStorage = other.myStorage;
			hasValue = other.hasValue;
			return *this;
		}

		constexpr Monad& operator=(Monad&& other)
			noexcept(nothrow_move_assignables<T>) requires(move_assignables<T>)
		{
			myStorage = static_cast<rebind_base<T>&&>(other.myStorage);
			hasValue = static_cast<bool&&>(other.hasValue);
			return *this;
		}

		constexpr ~Monad()
			noexcept(nothrow_destructibles<T>)
			requires(trivially_destructibles<T>)
		= default;

		constexpr ~Monad()
			noexcept(nothrow_destructibles<T>)
			requires(!trivially_destructibles<T>)
		{}

		template<invocables<T&> Fn>
		inline constexpr
			Monad&
			if_then(Fn&& action) &
			noexcept(noexcept(forward<Fn>(action)(declval<T&>())))
		{
			if (has_value())
			{
				forward<Fn>(action)(value());
			}

			return *this;
		}

		template<invocables<const T&> Fn>
		inline constexpr
			const Monad&
			if_then(Fn&& action) const&
			noexcept(noexcept(forward<Fn>(action)(declval<const T&>())))
		{
			if (has_value())
			{
				forward<Fn>(action)(value());
			}

			return *this;
		}

		template<invocables<T&&> Fn>
		inline constexpr
			Monad&&
			if_then(Fn&& action) &&
			noexcept(noexcept(forward<Fn>(action)(declval<T&&>())))
		{
			if (has_value())
			{
				forward<Fn>(action)(static_cast<T&&>(value()));
			}

			return move(*this);
		}

		template<invocables<const T&&> Fn>
		inline constexpr
			const Monad&&
			if_then(Fn&& action) const&&
			noexcept(noexcept(forward<Fn>(action)(declval<const T&&>())))
		{
			if (has_value())
			{
				forward<Fn>(action)(static_cast<const T&&>(value()));
			}

			return move(*this);
		}

		template<invocables<T&> Fn>
		inline constexpr
			monad_result_t<Fn, T&>
			operator>>(Fn&& action) &
			noexcept(noexcept(forward<Fn>(action)(declval<T&>())))
		{
			using fwd_result_t = monad_result_t<Fn, T&>;

			static_assert(!is_same_v<fwd_result_t, void>, "Result must not be void.");
			static_assert(is_specialization_v<fwd_result_t, Monad>);

			if (has_value())
			{
				return forward<Fn>(action)(value());
			}
			else
			{
				return fwd_result_t{};
			}
		}

		template<invocables<const T&> Fn>
		inline constexpr
			monad_result_t<Fn, const T&>
			operator>>(Fn&& action) const&
			noexcept(noexcept(forward<Fn>(action)(declval<const T&>())))
		{
			using fwd_result_t = monad_result_t<Fn, const T&>;

			static_assert(!is_same_v<fwd_result_t, void>, "Result must not be void.");
			static_assert(is_specialization_v<fwd_result_t, Monad>);

			if (has_value())
			{
				return forward<Fn>(action)(value());
			}
			else
			{
				return fwd_result_t{};
			}
		}

		template<invocables<T&&> Fn>
		inline constexpr
			monad_result_t<Fn, T&&>
			operator>>(Fn&& action) &&
			noexcept(noexcept(forward<Fn>(action)(declval<T&&>())))
		{
			using fwd_result_t = monad_result_t<Fn, T&&>;

			static_assert(!is_same_v<fwd_result_t, void>, "Result must not be void.");
			static_assert(is_specialization_v<fwd_result_t, Monad>);

			if (has_value())
			{
				return forward<Fn>(action)(static_cast<T&&>(value()));
			}
			else
			{
				return fwd_result_t{};
			}
		}

		template<invocables<const T&&> Fn>
		inline constexpr
			monad_result_t<Fn, const T&&>
			operator>>(Fn&& action) const&&
			noexcept(noexcept(forward<Fn>(action)(declval<const T&&>())))
		{
			using fwd_result_t = monad_result_t<Fn, const T&&>;

			static_assert(!is_same_v<fwd_result_t, void>, "Result must not be void.");
			static_assert(is_specialization_v<fwd_result_t, Monad>);

			if (has_value())
			{
				return forward<Fn>(action)(static_cast<const T&&>(value()));
			}
			else
			{
				return fwd_result_t{};
			}
		}

		template<invocables<T&> Fn>
		inline constexpr
			Monad&
			operator<<(Fn&& action) &
			noexcept(noexcept(forward<Fn>(action)(declval<T&>())))
		{
			if (!has_value())
			{
				forward<Fn>(action)(value());
			}

			return *this;
		}

		template<invocables<const T&> Fn>
		inline constexpr
			const Monad&
			operator<<(Fn&& action) const&
			noexcept(noexcept(forward<Fn>(action)(declval<const T&>())))
		{
			if (!has_value())
			{
				forward<Fn>(action)(value());
			}

			return *this;
		}

		template<invocables<T&&> Fn>
		inline constexpr
			Monad&&
			operator<<(Fn&& action) &&
			noexcept(noexcept(forward<Fn>(action)(declval<T&&>())))
		{
			if (!has_value())
			{
				forward<Fn>(action)(static_cast<T&&>(value()));
			}

			return static_cast<Monad&&>(*this);
		}

		template<invocables<const T&&> Fn>
		inline constexpr
			const Monad&&
			operator<<(Fn&& action) const&&
			noexcept(noexcept(forward<Fn>(action)(declval<const T&&>())))
		{
			if (!has_value())
			{
				forward<Fn>(action)(static_cast<const T&&>(value()));
			}

			return static_cast<const Monad&&>(*this);
		}

		template<invocables<T&> Fn>
		inline constexpr
			monad_result_t<Fn, T&>
			and_then(Fn&& action) &
			noexcept(noexcept(forward<Fn>(action)(declval<T&>())))
		{
			using fwd_result_t = monad_result_t<Fn, T&>;

			static_assert(!is_same_v<fwd_result_t, void>, "Result must not be void.");
			static_assert(is_specialization_v<fwd_result_t, Monad>);

			using fwd_value_t = fwd_result_t::base_type;

			if (has_value())
			{
				return forward<Fn>(action)(value());
			}
			else
			{
				return Monad<fwd_value_t>{ nullopt };
			}
		}

		template<invocables<const T&> Fn>
		inline constexpr
			monad_result_t<Fn, const T&>
			and_then(Fn&& action) const&
			noexcept(noexcept(forward<Fn>(action)(declval<const T&>())))
		{
			using fwd_result_t = monad_result_t<Fn, const T&>;

			static_assert(!is_same_v<fwd_result_t, void>, "Result must not be void.");
			static_assert(is_specialization_v<fwd_result_t, Monad>);

			if (has_value())
			{
				return forward<Fn>(action)(value());
			}
			else
			{
				return fwd_result_t{ nullopt };
			}
		}

		template<invocables<T&&> Fn>
		inline constexpr
			monad_result_t<Fn, T&&>
			and_then(Fn&& action) &&
			noexcept(noexcept(forward<Fn>(action)(declval<T&&>())))
		{
			using fwd_result_t = monad_result_t<Fn, T&&>;

			static_assert(!is_same_v<fwd_result_t, void>, "Result must not be void.");
			static_assert(is_specialization_v<fwd_result_t, Monad>);

			if (has_value())
			{
				return forward<Fn>(action)(static_cast<T&&>(value()));
			}
			else
			{
				return fwd_result_t{ nullopt };
			}
		}

		template<invocables<const T&&> Fn>
		inline constexpr
			monad_result_t<Fn, const T&&>
			and_then(Fn&& action) const&&
			noexcept(noexcept(forward<Fn>(action)(declval<const T&&>())))
		{
			using fwd_result_t = monad_result_t<Fn, const T&&>;

			static_assert(!is_same_v<fwd_result_t, void>, "Result must not be void.");
			static_assert(is_specialization_v<fwd_result_t, Monad>);

			if (has_value())
			{
				return forward<Fn>(action)(static_cast<const T&&>(value()));
			}
			else
			{
				return fwd_result_t{ nullopt };
			}
		}

		template<invocables<T&> Lfn, invocables Rfn>
		inline constexpr
			monad_result_t<Lfn, T&>
			transform(Lfn&& safe_action, Rfn&& fail_action) &
			noexcept(noexcept(forward<Lfn>(safe_action)(declval<T&>())) && noexcept(forward<Rfn>(fail_action)()))
		{
			using safe_result_t = monad_result_t<Lfn, T&>;
			using fail_result_t = monad_result_t<Rfn>;

			static_assert(same_as<safe_result_t, fail_result_t> || (same_as<safe_result_t, void> && same_as<fail_result_t, void>));

			if (has_value())
			{
				if constexpr (is_same_v<safe_result_t, void>)
				{
					forward<Lfn>(safe_action)(value());
				}
				else
				{
					return forward<Lfn>(safe_action)(value());
				}
			}
			else
			{
				if constexpr (is_same_v<fail_result_t, void>)
				{
					forward<Rfn>(fail_action)();
				}
				else
				{
					return forward<Rfn>(fail_action)();
				}
			}
		}

		template<invocables<const T&> Lfn, invocables Rfn>
		inline constexpr
			monad_result_t<Lfn, const T&&>
			transform(Lfn&& safe_action, Rfn&& fail_action) const&
			noexcept(noexcept(forward<Lfn>(safe_action)(declval<const T&>())) && noexcept(forward<Rfn>(fail_action)()))
		{
			using safe_result_t = monad_result_t<Lfn, const T&>;
			using fail_result_t = monad_result_t<Rfn>;

			static_assert(same_as<safe_result_t, fail_result_t> || (same_as<safe_result_t, void> && same_as<fail_result_t, void>));

			if (has_value())
			{
				if constexpr (is_same_v<safe_result_t, void>)
				{
					forward<Lfn>(safe_action)(value());
				}
				else
				{
					return forward<Lfn>(safe_action)(value());
				}
			}
			else
			{
				if constexpr (is_same_v<fail_result_t, void>)
				{
					forward<Rfn>(fail_action)();
				}
				else
				{
					return forward<Rfn>(fail_action)();
				}
			}
		}

		template<invocables<T&&> Lfn, invocables Rfn>
		inline constexpr
			monad_result_t<Lfn, T&>
			transform(Lfn&& safe_action, Rfn&& fail_action) &&
			noexcept(noexcept(forward<Lfn>(safe_action)(declval<T&&>())) && noexcept(forward<Rfn>(fail_action)()))
		{
			using safe_result_t = monad_result_t<Lfn, T&&>;
			using fail_result_t = monad_result_t<Rfn>;

			static_assert(same_as<safe_result_t, fail_result_t> || (same_as<safe_result_t, void> && same_as<fail_result_t, void>));

			if (has_value())
			{
				if constexpr (is_same_v<safe_result_t, void>)
				{
					forward<Lfn>(safe_action)(static_cast<T&&>(value()));
				}
				else
				{
					return forward<Lfn>(safe_action)(static_cast<T&&>(value()));
				}
			}
			else
			{
				if constexpr (is_same_v<fail_result_t, void>)
				{
					forward<Rfn>(fail_action)();
				}
				else
				{
					return forward<Rfn>(fail_action)();
				}
			}
		}

		template<invocables<const T&&> Lfn, invocables Rfn>
		inline constexpr
			monad_result_t<Lfn, const T&&>
			transform(Lfn&& safe_action, Rfn&& fail_action) const&&
			noexcept(noexcept(forward<Lfn>(safe_action)(declval<const T&&>())) && noexcept(forward<Rfn>(fail_action)()))
		{
			using safe_result_t = monad_result_t<Lfn, const T&&>;
			using fail_result_t = monad_result_t<Rfn>;

			static_assert(same_as<safe_result_t, fail_result_t> || (same_as<safe_result_t, void> && same_as<fail_result_t, void>));

			if (has_value())
			{
				if constexpr (is_same_v<safe_result_t, void>)
				{
					forward<Lfn>(safe_action)(static_cast<const T&&>(value()));
				}
				else
				{
					return forward<Lfn>(safe_action)(static_cast<const T&&>(value()));
				}
			}
			else
			{
				if constexpr (is_same_v<fail_result_t, void>)
				{
					forward<Rfn>(fail_action)();
				}
				else
				{
					return forward<Rfn>(fail_action)();
				}
			}
		}

		template<invocables Fn>
		inline constexpr
			Monad&
			else_then(Fn&& action) &
			noexcept(noexcept(forward<Fn>(action)()))
		{
			if (!has_value())
			{
				forward<Fn>(action)();
			}

			return *this;
		}

		template<invocables Fn>
		inline constexpr
			const Monad&
			else_then(Fn&& action) const&
			noexcept(noexcept(forward<Fn>(action)()))
		{
			if (!has_value())
			{
				forward<Fn>(action)();
			}

			return *this;
		}

		template<invocables Fn>
		inline constexpr
			Monad&&
			else_then(Fn&& action) &&
			noexcept(noexcept(forward<Fn>(action)()))
		{
			if (!has_value())
			{
				forward<Fn>(action)();
			}

			return util::move(*this);
		}

		template<invocables Fn>
		inline constexpr
			const Monad&&
			else_then(Fn&& action) const&&
			noexcept(noexcept(forward<Fn>(action)()))
		{
			if (!has_value())
			{
				forward<Fn>(action)();
			}

			return util::move(*this);
		}

		template<invocables Fn>
		inline constexpr
			monad_result_t<Fn>
			or_else(Fn&& action) &
			noexcept(noexcept(forward<Fn>(action)()))
		{
			using fwd_result_t = monad_result_t<Fn>;

			static_assert(!is_same_v<fwd_result_t, void>, "Result must not be void.");
			static_assert(is_specialization_v<fwd_result_t, Monad>);

			if (!has_value())
			{
				return forward<Fn>(action)();
			}
			else
			{
				return fwd_result_t{ nullopt };
			}
		}

		template<invocables Fn>
		inline constexpr
			monad_result_t<Fn>
			or_else(Fn&& action) const&
			noexcept(noexcept(forward<Fn>(action)()))
		{
			using fwd_result_t = monad_result_t<Fn>;

			static_assert(!is_same_v<fwd_result_t, void>, "Result must not be void.");
			static_assert(is_specialization_v<fwd_result_t, Monad>);

			if (!has_value())
			{
				return forward<Fn>(action)();
			}
			else
			{
				return fwd_result_t{ nullopt };
			}
		}

		template<invocables Fn>
		inline constexpr
			monad_result_t<Fn>
			or_else(Fn&& action) &&
			noexcept(noexcept(forward<Fn>(action)()))
		{
			using fwd_result_t = monad_result_t<Fn>;

			static_assert(!is_same_v<fwd_result_t, void>, "Result must not be void.");
			static_assert(is_specialization_v<fwd_result_t, Monad>);

			if (!has_value())
			{
				return forward<Fn>(action)();
			}
			else
			{
				return fwd_result_t{ nullopt };
			}
		}

		template<invocables Fn>
		inline constexpr
			monad_result_t<Fn>
			or_else(Fn&& action) const&&
			noexcept(noexcept(forward<Fn>(action)()))
		{
			using fwd_result_t = monad_result_t<Fn>;

			static_assert(!is_same_v<fwd_result_t, void>, "Result must not be void.");
			static_assert(is_specialization_v<fwd_result_t, Monad>);

			if (!has_value())
			{
				return forward<Fn>(action)();
			}
			else
			{
				return fwd_result_t{ nullopt };
			}
		}

		[[nodiscard]]
		constexpr T& value() & noexcept
		{
			return *myStorage;
		}

		[[nodiscard]]
		constexpr const T& value() const& noexcept
		{
			return *myStorage;
		}

		[[nodiscard]]
		constexpr T&& value() && noexcept
		{
			return static_cast<T&&>(*myStorage);
		}

		[[nodiscard]]
		constexpr const T&& value() const&& noexcept
		{
			return static_cast<const T&&>(*myStorage);
		}

		constexpr T& operator*() & noexcept
		{
			return value();
		}

		constexpr const T& operator*() const& noexcept
		{
			return value();
		}

		constexpr T&& operator*() && noexcept
		{
			return static_cast<T&&>(value());
		}

		constexpr const T&& operator*() const&& noexcept
		{
			return static_cast<const T&&>(value());
		}

		template<convertible_to<T> U>
		[[nodiscard]]
		constexpr T value_or(const U& failsafe) const& noexcept(nothrow_constructibles<T>)
		{
			if (has_value())
			{
				return value();
			}
			else
			{
				return static_cast<T>(failsafe);
			}
		}

		[[nodiscard]]
		constexpr T value_or(T&& failsafe) const& noexcept(nothrow_move_constructibles<T>)
		{
			if (has_value())
			{
				return value();
			}
			else
			{
				return failsafe;
			}
		}

		template<convertible_to<T> U>
		[[nodiscard]]
		constexpr T value_or(const U& failsafe) && noexcept(nothrow_constructibles<T>)
		{
			if (has_value())
			{
				return value();
			}
			else
			{
				return static_cast<T>(failsafe);
			}
		}

		[[nodiscard]]
		constexpr T&& value_or(T&& failsafe) && noexcept(nothrow_move_constructibles<T>)
		{
			if (has_value())
			{
				return value();
			}
			else
			{
				return move(failsafe);
			}
		}

		[[nodiscard]]
		constexpr bool has_value() const noexcept
		{
			return hasValue;
		}

		[[nodiscard]]
		explicit constexpr operator bool() const noexcept
		{
			return hasValue;
		}

		explicit constexpr operator T () const noexcept(nothrow_copy_constructibles<T>)
		{
			return value();
		}

		constexpr ~Monad()
			noexcept(nothrow_destructibles<T>) = default;

	private:
		value_type myStorage;
		bool hasValue = false;
	};

	template<>
	class [[nodiscard]] Monad<void>
	{
	public:
		using base_type = void;

		constexpr Monad() noexcept
			: hasValue(true)
		{}

		constexpr Monad(nullopt_t) noexcept
			: hasValue(false)
		{}

		template<typename Fn, typename... Args>
			requires invocables<Fn, Args...>
		inline constexpr
			Monad&
			if_then(Fn&& action, Args&&... args) &
			noexcept(noexcept(forward<Fn>(action)(declval<Args>()...)))
		{
			if (has_value())
			{
				forward<Fn>(action)(forward<Args>(args)...);
			}

			return *this;
		}

		template<typename Fn, typename... Args>
			requires invocables<Fn, Args...>
		inline constexpr
			const Monad&
			if_then(Fn&& action, Args&&... args) const&
			noexcept(noexcept(forward<Fn>(action)(declval<Args>()...)))
		{
			if (has_value())
			{
				forward<Fn>(action)(forward<Args>(args)...);
			}

			return *this;
		}

		template<typename Fn, typename... Args>
			requires invocables<Fn, Args...>
		inline constexpr
			Monad&&
			if_then(Fn&& action, Args&&... args) &&
			noexcept(noexcept(forward<Fn>(action)(declval<Args>()...)))
		{
			if (has_value())
			{
				forward<Fn>(action)(forward<Args>(args)...);
			}

			return move(*this);
		}

		template<typename Fn, typename... Args>
			requires invocables<Fn, Args...>
		inline constexpr
			const Monad&&
			if_then(Fn&& action, Args&&... args) const&&
			noexcept(noexcept(forward<Fn>(action)(declval<Args>()...)))
		{
			if (has_value())
			{
				forward<Fn>(action)(forward<Args>(args)...);
			}

			return move(*this);
		}

		template<typename Fn, typename... Args>
			requires invocables<Fn, Args...>
		inline constexpr
			auto
			and_then(Fn&& action, Args&&... args)
			noexcept(noexcept(forward<Fn>(action)(declval<Args>()...)))
		{
			using fwd_result_t = monad_result_t<Fn, Args...>;

			static_assert(is_specialization_v<fwd_result_t, Monad>);

			using fwd_value_t = fwd_result_t::base_type;

			if (has_value())
			{
				return forward<Fn>(action)(forward<Args>(args)...);
			}
			else
			{
				return Monad<fwd_value_t>{ nullopt };
			}
		}

		template<typename Fn, typename... Args>
			requires invocables<Fn, Args...>
		inline constexpr
			auto
			and_then(Fn&& action, Args&&... args) const
			noexcept(noexcept(forward<Fn>(action)(declval<Args>()...)))
		{
			using fwd_result_t = monad_result_t<Fn, Args...>;

			static_assert(is_specialization_v<fwd_result_t, Monad>);

			using fwd_value_t = fwd_result_t::base_type;

			if (has_value())
			{
				return forward<Fn>(action)(forward<Args>(args)...);
			}
			else
			{
				return Monad<fwd_value_t>{ nullopt };
			}
		}

		template<invocables Lfn, invocables Rfn>
		inline constexpr
			monad_result_t<Lfn>
			transform(Lfn&& safe_action, Rfn&& fail_action) const
			noexcept(noexcept(forward<Lfn>(safe_action)) && noexcept(forward<Rfn>(fail_action)))
		{
			using safe_result_t = monad_result_t<Lfn>;
			using fail_result_t = monad_result_t<Rfn>;

			static_assert(same_as<safe_result_t, fail_result_t> || (same_as<safe_result_t, void> && same_as<fail_result_t, void>));

			if (has_value())
			{
				if constexpr (is_same_v<safe_result_t, void>)
				{
					forward<Lfn>(safe_action)();
				}
				else
				{
					return forward<Lfn>(safe_action)();
				}
			}
			else
			{
				if constexpr (is_same_v<fail_result_t, void>)
				{
					forward<Rfn>(fail_action)();
				}
				else
				{
					return forward<Rfn>(fail_action)();
				}
			}
		}

		template<invocables Fn, typename Uty>
		inline constexpr
			Uty
			translate(Fn&& safe_action, Uty&& default_value)
			noexcept(noexcept(forward<Fn>(safe_action)()))
		{
			using fn_result_t = monad_result_t<Fn>;

			static_assert(convertible_to<fn_result_t, Uty>);

			if (has_value())
			{
				return forward<Fn>(safe_action)();
			}
			else
			{
				return forward<Uty>(default_value);
			}
		}

		template<typename Fn, typename... Args>
			requires invocables<Fn, Args...>
		inline constexpr
			Monad&
			else_then(Fn&& action, Args&&... args) &
			noexcept(noexcept(forward<Fn>(action)(declval<Args>()...)))
		{
			if (!has_value())
			{
				forward<Fn>(action)(forward<Args>(args)...);
			}

			return *this;
		}

		template<typename Fn, typename... Args>
			requires invocables<Fn, Args...>
		inline constexpr
			const Monad&
			else_then(Fn&& action, Args&&... args) const&
			noexcept(noexcept(forward<Fn>(action)(declval<Args>()...)))
		{
			if (!has_value())
			{
				forward<Fn>(action)(forward<Args>(args)...);
			}

			return *this;
		}

		template<typename Fn, typename... Args>
			requires invocables<Fn, Args...>
		inline constexpr
			Monad&&
			else_then(Fn&& action, Args&&... args) &&
			noexcept(noexcept(forward<Fn>(action)(declval<Args>()...)))
		{
			if (!has_value())
			{
				forward<Fn>(action)(forward<Args>(args)...);
			}

			return move(*this);
		}

		template<typename Fn, typename... Args>
			requires invocables<Fn, Args...>
		inline constexpr
			const Monad&&
			else_then(Fn&& action, Args&&... args) const&&
			noexcept(noexcept(forward<Fn>(action)(declval<Args>()...)))
		{
			if (!has_value())
			{
				forward<Fn>(action)(forward<Args>(args)...);
			}

			return move(*this);
		}

		template<typename Fn, typename... Args>
			requires invocables<Fn, Args...>
		inline constexpr
			auto
			or_else(Fn&& action, Args&&... args) const
			noexcept(noexcept(forward<Fn>(action)(declval<Args>()...)))
		{
			using fwd_result_t = monad_result_t<Fn, Args...>;

			static_assert(!is_same_v<fwd_result_t, void>, "Result must not be void.");
			static_assert(is_specialization_v<fwd_result_t, Monad>);

			using fwd_value_t = fwd_result_t::base_type;

			if (!has_value())
			{
				return forward<Fn>(action)(forward<Args>(args)...);
			}
			else
			{
				return Monad<fwd_value_t>{ nullopt };
			}
		}

		constexpr bool has_value() const noexcept
		{
			return hasValue;
		}

		explicit constexpr operator bool() const noexcept
		{
			return hasValue;
		}

		constexpr Monad(const Monad&) noexcept = default;
		constexpr Monad(Monad&&) noexcept = default;
		constexpr Monad& operator=(const Monad&) noexcept = default;
		constexpr Monad& operator=(Monad&&) noexcept = default;
		constexpr ~Monad() noexcept = default;

	private:
		bool hasValue;
	};
}

namespace util
{
#if false
	constexpr void do_something() noexcept
	{}

	constexpr void test_monad() noexcept
	{
		Monad<int> monad4{ 1000 };
		const bool has4 = monad4.has_value();
		const Monad<int> monad5{ 1000 };
		const bool has5 = monad5.has_value();
		constexpr Monad<int> monad6{ 1000 };
		constexpr bool has6 = monad6.has_value();

		Monad<int> monad7{ int(80264.01954f) };
		const Monad<int> monad8{ false };
		constexpr Monad<int> monad9{ 1000ULL };

		const auto valor7 = monad7.value_or(42);

		const auto expr8_3 = monad8.transform(
			[](const int& v) -> float { return static_cast<float>(v); },
			[](const int& v) -> float { return v * 100.0f; }
		);

		const auto& expr8_5 = monad8.else_then([]() { do_something(); });

		Monad<int> monad1{ false };
		const Monad<int> monad2{ false };
		constexpr Monad<int> monad3{ false };

		constexpr int valor3 = monad3.value_or(3000.0);

		constexpr const auto& expr9_1 = monad9.if_then([](auto&&) -> float { return 3000.0; });

		constexpr Monad<float> expr9_2 = monad9.and_then(
			[](const int&) -> Monad<float> {
			return 3000.0f;
		});

		constexpr double expr9_3 = monad9.transform(
			[](const int& value) -> double { return value + 3000.0; },
			[](auto&&) -> double { return 0.0; }
		);

		constexpr const Monad<int>& expr9_5 = monad9.else_then(
			[]() {}
		);

		const auto rr = monad9 >> [](const int& val) -> Monad<float> { return 5020.0f; };

		const auto expr9_6 = monad7.or_else([]() -> Monad<float> {
			return { 5020.0f };
		});
	}
#endif
}
