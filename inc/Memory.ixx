module;
#include <memory>
export module Utility.Memory;

export namespace util
{
	using ::std::operator==;
	using ::std::operator!=;
	using ::std::operator<=>;

	using ::std::memcpy;
	using ::std::memmove;
	using ::std::memcmp;
	using ::std::memset;
	using ::std::align;

	using ::std::move;
	using ::std::forward;
	using ::std::move_if_noexcept;
	using ::std::declval;
	using ::std::addressof;
	using ::std::exchange;
	using ::std::as_const;
	using ::std::addressof;
	using ::std::to_address;

	using ::std::unique_ptr;
	using ::std::default_delete;
	using ::std::shared_ptr;
	using ::std::weak_ptr;
	using ::std::enable_shared_from_this;
	using ::std::owner_less;
	using ::std::make_unique;
	using ::std::make_shared;
	using ::std::get_deleter;
	using ::std::bad_weak_ptr;

	using ::std::static_pointer_cast;
	using ::std::dynamic_pointer_cast;
	using ::std::const_pointer_cast;
	using ::std::reinterpret_pointer_cast;

	using ::std::allocate_shared;
	using ::std::construct_at;
	using ::std::destroy_at;
	using ::std::destroy;
	using ::std::destroy_n;

	using ::std::reference_wrapper;
	using ::std::ref;
	using ::std::cref;
	using ::std::pointer_traits;
	using ::std::uninitialized_default_construct;
	using ::std::uninitialized_default_construct_n;
	using ::std::uninitialized_value_construct;
	using ::std::uninitialized_value_construct_n;
	using ::std::uninitialized_value_construct_n;
	using ::std::uninitialized_copy;
	using ::std::uninitialized_copy_n;
	using ::std::uninitialized_move;
	using ::std::uninitialized_move_n;
	using ::std::uninitialized_fill;
	using ::std::uninitialized_fill_n;
}
