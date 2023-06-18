export module Utility.Atomic;
import <utility>;
export import <atomic>;

export namespace util
{
	namespace detail
	{
		template<typename T, size_t Size>
		using atomic_stg_t = ::std::_Atomic_storage<T, Size>;
		template<typename T>
		using atomic_s1_t = ::std::_Atomic_storage<T, 1>;
		template<typename T>
		using atomic_s2_t = ::std::_Atomic_storage<T, 2>;
		template<typename T>
		using atomic_s4_t = ::std::_Atomic_storage<T, 4>;
		template<typename T>
		using atomic_s8_t = ::std::_Atomic_storage<T, 8>;
		template<typename T>
		using atomic_s16_t = ::std::_Atomic_storage<T, 16>;

		template<typename T, size_t Size>
		using atomic_int_t = ::std::_Atomic_integral<T, Size>;
		template<typename T>
		using atomic_i1_t = ::std::_Atomic_integral<T, 1>;
		template<typename T>
		using atomic_i2_t = ::std::_Atomic_integral<T, 2>;
		template<typename T>
		using atomic_i4_t = ::std::_Atomic_integral<T, 4>;
		template<typename T>
		using atomic_i8_t = ::std::_Atomic_integral<T, 8>;

		template<typename T>
		using atomic_float_t = ::std::_Atomic_floating<T>;
	}

	using ::std::atomic;
	using ::std::atomic_ref;
	using ::std::atomic_flag;
	using ::std::atomic_bool;
	using ::std::atomic_char;
	using ::std::atomic_schar;
	using ::std::atomic_uchar;
	using ::std::atomic_short;
	using ::std::atomic_ushort;
	using ::std::atomic_int;
	using ::std::atomic_uint;
	using ::std::atomic_long;
	using ::std::atomic_ulong;
	using ::std::atomic_llong;
	using ::std::atomic_ullong;
	using ::std::atomic_char8_t;
	using ::std::atomic_char16_t;
	using ::std::atomic_char32_t;
	using ::std::atomic_wchar_t;
	using ::std::atomic_ptrdiff_t;
	using ::std::atomic_size_t;

	using ::std::memory_order;
	using ::std::memory_order_relaxed;
	using ::std::memory_order_consume;
	using ::std::memory_order_acquire;
	using ::std::memory_order_release;
	using ::std::memory_order_acq_rel;
	using ::std::memory_order_seq_cst;

	using ::std::atomic_is_lock_free;
	using ::std::atomic_fetch_add;
	using ::std::atomic_fetch_add_explicit;
	using ::std::atomic_fetch_sub;
	using ::std::atomic_fetch_sub_explicit;
	using ::std::atomic_fetch_xor;
	using ::std::atomic_fetch_xor_explicit;
	using ::std::atomic_fetch_and;
	using ::std::atomic_fetch_and_explicit;
	using ::std::atomic_fetch_or;
	using ::std::atomic_fetch_or_explicit;
	using ::std::atomic_compare_exchange_strong;
	using ::std::atomic_compare_exchange_strong_explicit;
	using ::std::atomic_compare_exchange_weak;
	using ::std::atomic_compare_exchange_weak_explicit;
	using ::std::atomic_exchange;
	using ::std::atomic_exchange_explicit;
	using ::std::atomic_wait;
	using ::std::atomic_wait_explicit;
	using ::std::atomic_flag_clear;
	using ::std::atomic_flag_clear_explicit;
	using ::std::atomic_flag_test;
	using ::std::atomic_flag_test_explicit;
	using ::std::atomic_flag_test_and_set;
	using ::std::atomic_flag_test_and_set_explicit;
	using ::std::atomic_flag_wait;
	using ::std::atomic_flag_wait_explicit;
	using ::std::atomic_flag_notify_one;
	using::std::atomic_flag_notify_all;

	template<std::copyable T, std::convertible_to<T> U>
	inline
		bool
		CompareAndSet(std::atomic<T>& atomic
		, T expected_value, const U& value
		, const std::memory_order& succ_order = std::memory_order_release
		, const std::memory_order& fail_order = std::memory_order_acquire)
		noexcept
	{
		if (!atomic.compare_exchange_strong(expected_value, value, succ_order, fail_order))
		{
			// expected_value == value;
			return atomic.compare_exchange_strong(expected_value, value, succ_order, fail_order);
		}
		else
		{
			return true;
		}
	}

	template<std::copyable T, std::convertible_to<T> U>
	inline
		bool
		CompareAndSet(volatile std::atomic<T>& atomic
		, T expected_value, const U& value
		, const std::memory_order& succ_order = std::memory_order_release
		, const std::memory_order& fail_order = std::memory_order_acquire)
		noexcept
	{
		if (!atomic.compare_exchange_strong(expected_value, value, succ_order, fail_order))
		{
			// expected_value == value;
			return atomic.compare_exchange_strong(expected_value, value, succ_order, fail_order);
		}
		else
		{
			return true;
		}
	}

	template<std::copyable T, std::convertible_to<T> U>
	inline
		bool
		CompareAndSet(std::atomic<T>* atomic
		, T expected_value, const U& value
		, const std::memory_order& succ_order = std::memory_order_release
		, const std::memory_order& fail_order = std::memory_order_acquire)
		noexcept
	{
		if (!atomic->compare_exchange_strong(expected_value, value, succ_order, fail_order))
		{
			// expected_value == value;
			return atomic->compare_exchange_strong(expected_value, value, succ_order, fail_order);
		}
		else
		{
			return true;
		}
	}

	template<std::copyable T, std::convertible_to<T> U>
	inline
		bool
		CompareAndSet(volatile std::atomic<T>* atomic
		, T expected_value, const U& value
		, const std::memory_order& succ_order = std::memory_order_release
		, const std::memory_order& fail_order = std::memory_order_acquire)
		noexcept
	{
		if (!atomic->compare_exchange_strong(expected_value, value, succ_order, fail_order))
		{
			// expected_value == value;
			return atomic->compare_exchange_strong(expected_value, value, succ_order, fail_order);
		}
		else
		{
			return true;
		}
	}
}
