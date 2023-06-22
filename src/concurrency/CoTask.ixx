export module Utility.Coroutine:Task;
import <coroutine>;
import Utility.Constraints;
import :Promise;

export namespace util::coroutine
{
	class [[nodiscard]] DeferredTask
	{
	public:
		using promise_type = DeferredPromise<DeferredTask>;
		using handle_type = std::coroutine_handle<promise_type>;

		explicit constexpr DeferredTask(const handle_type& handle) noexcept
			: myHandle(handle)
		{}

		explicit constexpr DeferredTask(handle_type&& handle) noexcept
			: myHandle(static_cast<handle_type&&>(handle))
		{}

		[[nodiscard]]
		bool Done() const noexcept
		{
			return myHandle.done();
		}

		void Resume() const noexcept
		{
			if (!myHandle.done())
			{
				myHandle.resume();
			}
		}

		void operator()() const noexcept
		{
			Resume();
		}

		~DeferredTask() noexcept
		{
			if (myHandle.done())
			{
				myHandle.destroy();
			}
		}

		handle_type myHandle;
	};

	class [[nodiscard]] RelaxedTask
	{
	public:
		using promise_type = RelaxedPromise<RelaxedTask>;
		using handle_type = coroutine_handle<promise_type>;

		explicit constexpr RelaxedTask(const handle_type& handle) noexcept
			: myHandle(handle)
		{}

		explicit constexpr RelaxedTask(handle_type&& handle) noexcept
			: myHandle(move(handle))
		{}

		[[nodiscard]]
		bool Done() const noexcept
		{
			return myHandle.done();
		}

		void Resume() const noexcept
		{
			if (!myHandle.done())
			{
				myHandle.resume();
			}
		}

		void operator()() const noexcept
		{
			Resume();
		}

		~RelaxedTask() noexcept
		{
			if (myHandle.done())
			{
				myHandle.destroy();
			}
		}

		handle_type myHandle;
	};
}
