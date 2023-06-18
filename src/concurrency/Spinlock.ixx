export module Utility.Concurrency.Spinlock;
import <atomic>;

export namespace util
{
	class Spinlock
	{
	public:
		constexpr Spinlock() noexcept
			: mySwitch()
		{}

		~Spinlock() noexcept
		{
			mySwitch.clear();
		}

		inline void lock(const memory_order& order = memory_order_acquire) &
			noexcept
		{
			while (mySwitch.test_and_set(order));
		}

		inline void lock(const memory_order& order = memory_order_acquire) volatile&
			noexcept
		{
			while (mySwitch.test_and_set(order));
		}

		inline void unlock(const memory_order& order = memory_order_release) &
			noexcept
		{
			mySwitch.clear(order);
		}

		inline void unlock(const memory_order& order = memory_order_release) volatile&
			noexcept
		{
			mySwitch.clear(order);
		}

		inline bool try_lock(const memory_order& order = memory_order_relaxed) &
			noexcept
		{
			return !mySwitch.test_and_set(order);
		}

		inline bool try_lock(const memory_order& order = memory_order_relaxed) volatile&
			noexcept
		{
			return !mySwitch.test_and_set(order);
		}

		inline void wait_lock(const memory_order& order = memory_order_relaxed) &
			noexcept
		{
			return mySwitch.wait(true, order);
		}

		inline void wait_lock(const memory_order& order = memory_order_relaxed) volatile&
			noexcept
		{
			return mySwitch.wait(true, order);
		}

		inline void wait_unlock(const memory_order& order = memory_order_relaxed) &
			noexcept
		{
			return mySwitch.wait(false, order);
		}

		inline void wait_unlock(const memory_order& order = memory_order_relaxed) volatile&
			noexcept
		{
			return mySwitch.wait(false, order);
		}

		inline void notify() & noexcept
		{
			return mySwitch.notify_one();
		}

		inline void notify() volatile& noexcept
		{
			return mySwitch.notify_one();
		}

		inline void awake() & noexcept
		{
			return mySwitch.notify_all();
		}

		inline void awake() volatile& noexcept
		{
			return mySwitch.notify_all();
		}

		inline bool is_locked() const& noexcept
		{
			return mySwitch.test(memory_order_relaxed);
		}

		inline bool is_locked() const volatile& noexcept
		{
			return mySwitch.test(memory_order_relaxed);
		}

		Spinlock(const Spinlock&) = delete;
		Spinlock(Spinlock&&) noexcept = delete;
		Spinlock& operator=(const Spinlock&) = delete;
		Spinlock& operator=(Spinlock&&) noexcept = delete;

	private:
		std::atomic_flag mySwitch;
	};
}

export using std::memory_order;
export using std::memory_order_relaxed;
export using std::memory_order_consume;
export using std::memory_order_acquire;
export using std::memory_order_release;
export using std::memory_order_acq_rel;
export using std::memory_order_seq_cst;
