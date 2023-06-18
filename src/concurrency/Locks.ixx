module;
#include <mutex>
#include <shared_mutex>
#include <latch>
#include <semaphore>
#include <barrier>
#include <condition_variable>
export module Utility.Concurrency.Locks;
import Utility.Constraints;

export namespace util
{
	template<typename T>
	concept lockables = movable<T> && requires(T t)
	{
		t.lock();
		t.unlock();
		{ t.try_lock() } -> same_as<bool>;
	};

	template<typename T>
	concept shared_lockables = movable<T> && lockables<T> && requires(T t)
	{
		t.lock_shared();
		t.unlock_shared();
		{ t.try_lock_shared() } -> convertible_to<bool>;
	};

	using ::std::operator<=>;
	using ::std::operator==;
	using ::std::swap;
	using ::std::lock;
	using ::std::try_lock;

	using ::std::call_once;
	using ::std::once_flag;

	using ::std::mutex;
	using ::std::recursive_mutex;
	using ::std::shared_mutex;
	using ::std::timed_mutex;
	using ::std::recursive_timed_mutex;
	using ::std::shared_timed_mutex;

	using ::std::defer_lock_t;
	using ::std::defer_lock;
	using ::std::try_to_lock_t;
	using ::std::try_to_lock;
	using ::std::adopt_lock_t;
	using ::std::adopt_lock;

	using ::std::lock_guard;
	using ::std::scoped_lock;
	using ::std::shared_lock;
	using ::std::unique_lock;

	using ::std::condition_variable;
	using ::std::condition_variable_any;
	using ::std::cv_status;

	using ::std::latch;
	using ::std::barrier;

	template <ptrdiff_t max_counts = 0xFFFFFFFE>
	using semaphore = ::std::counting_semaphore<max_counts>;
	using ::std::binary_semaphore;
	using ::std::counting_semaphore;

	/// <summary>
	/// std::unique_lock with std::try_to_lock
	/// </summary>
	template<typename Lockable>
	struct OwnedLock
	{
		explicit OwnedLock(Lockable& lock) noexcept(noexcept(lock.try_lock()))
			: my_lock(lock)
		{
			owned = my_lock.try_lock();
		}

		~OwnedLock() noexcept(noexcept(my_lock.unlock()))
		{
			if (owned)
			{
				my_lock.unlock();
			}
		}

		OwnedLock(const Lockable&& lock) = delete;

		explicit inline constexpr operator bool() const
		{
			return owned;
		}

		OwnedLock(const OwnedLock& other) = delete;
		OwnedLock& operator=(const OwnedLock& other) = delete;

	private:
		mutable bool owned;
		Lockable& my_lock;
	};

	template<lockables Lockable>
	OwnedLock(Lockable&) -> OwnedLock<Lockable>;
}

#pragma warning(push, 1)
namespace util::test
{
	void test_lockables() noexcept
	{
		mutex m{};

		unique_lock l(m);
		unique_lock l2(m, defer_lock);
		unique_lock l3(m, try_to_lock);
		unique_lock l4(m, adopt_lock);

		//unique_lock l5(move(l));
		//unique_lock l6(move(l2), defer_lock);
		//unique_lock l7(move(l3), try_to_lock);
		//unique_lock l8(move(l4), adopt_lock);

		//l = move(l5);
		//l2 = move(l6);
		//l3 = move(l7);
		//l4 = move(l8);

		l.swap(l2);
		swap(l, l2);

		//l = l2;
		//l = move(l2);

		//l2 = l;
		//l2 = move(l);

		(void)l.try_lock();
		l.unlock();

		l.release();
		l.release();
	}
}
#pragma warning(pop)
