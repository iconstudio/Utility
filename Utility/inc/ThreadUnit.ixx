export module Utility.Concurrency.Thread.Unit;
import <exception>;
import Utility;
import Utility.Constraints;
import Utility.Concurrency.Thread;

export namespace util
{
	/// <summary>
	/// Implementation of std::thread + std::stop_token
	/// </summary>
	class [[nodiscard]] ThreadUnit
	{
	public:
		ThreadUnit() noexcept
			: myHandle(), stopToken()
		{}

		template<typename Fn, typename... Args>
			requires (!same_as<clean_t<Fn>, thread, jthread, in_place_t>&& invocables<Fn, unwrap_ref_decay_t<Args>...>)
		explicit(0 < sizeof...(Args))
			ThreadUnit(Fn&& functor, CancellationToken&& token, Args&&... args) noexcept
			: ThreadUnit(in_place, thread{ forward<Fn>(functor), forward<Args>(args)... }, static_cast<CancellationToken&&>(token))
		{}

		template<typename Fn, typename... Args>
			requires (!same_as<clean_t<Fn>, thread, jthread, in_place_t>&& invocables<Fn, unwrap_ref_decay_t<Args>...>)
		explicit(0 < sizeof...(Args))
			ThreadUnit(Fn&& functor, CancellationSource& ssource, Args&&... args) noexcept
			: ThreadUnit(in_place, thread{ forward<Fn>(functor), forward<Args>(args)... }, ssource.get_token())
		{}

		explicit ThreadUnit([[maybe_unused]] in_place_t, thread&& unit, CancellationSource& ssource)
			noexcept
			: ThreadUnit(in_place, static_cast<thread&&>(unit), ssource.get_token())
		{}

		explicit ThreadUnit([[maybe_unused]] in_place_t, thread&& unit, CancellationToken&& token)
			noexcept
			: myHandle(static_cast<thread&&>(unit))
			, stopToken(static_cast<CancellationToken&&>(token))
		{}

		~ThreadUnit()
		{
			if (myHandle.joinable())
			{
				join();
			}
		}

		inline void swap(ThreadUnit& other) noexcept
		{
			if (addressof(other) == this)
			{
				return;
			}

			myHandle.swap(other.myHandle);
			stopToken.swap(other.stopToken);
		}

		inline void join()
		{
			myHandle.join();
		}

		inline void detach()
		{
			myHandle.detach();
		}

		[[nodiscard]]
		inline thread::id get_id() const noexcept
		{
			return myHandle.get_id();
		}

		[[nodiscard]]
		inline thread::native_handle_type native_handle() noexcept
		{
			return myHandle.native_handle();
		}

		[[nodiscard]]
		inline bool joinable() const noexcept
		{
			return myHandle.joinable();
		}

		[[nodiscard]]
		inline bool stop_requested() const noexcept
		{
			return stopToken.stop_requested();
		}

		[[nodiscard]]
		inline bool stop_possible() const noexcept
		{
			return stopToken.stop_possible();
		}

		ThreadUnit(const ThreadUnit& other) = delete;
		ThreadUnit(ThreadUnit&& other) noexcept = default;
		ThreadUnit& operator=(const ThreadUnit& other) = delete;
		ThreadUnit& operator=(ThreadUnit&& other) = default;

		thread myHandle;
		CancellationToken stopToken;
	};
}
