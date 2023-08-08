export module Utility.Coroutine:Task;
import <coroutine>;
import :Promise;

export namespace util::coroutine
{
	template<typename Promise>
	class BasicTask
	{
	public:
		using promise_type = Promise;
		using handle_type = coroutine_handle<Promise>;

		[[nodiscard]]
		inline bool IsDone() const noexcept
		{
			return myHandle.done();
		}

		inline void Resume() const noexcept(noexcept(myHandle.resume()))
		{
			if (!myHandle.done())
			{
				myHandle.resume();
			}
		}

		inline void operator()() const noexcept
		{
			Resume();
		}

		inline ~BasicTask() noexcept(noexcept(myHandle.destroy()))
		{
			if (myHandle.done())
			{
				myHandle.destroy();
			}
		}

		handle_type myHandle;
	};

	class DeferredTask : public BasicTask<DeferredPromise<DeferredTask>>
	{};

	class RelaxedTask : public BasicTask<RelaxedPromise<RelaxedTask>>
	{};
}
