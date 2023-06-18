export module Utility.Memory.Watcher;
import Utility.Constraints;

export namespace util
{
	template<typename T>
	struct Watcher
	{
		constexpr Watcher(T* const& ptr) noexcept
			: handle(ptr)
		{}

		constexpr ~Watcher() noexcept(util::trivially_destructibles<T>)
		{
			if (!ok && handle)
			{
				delete handle;

			}
		}

		constexpr void Ok(bool value = true) noexcept
		{
			ok = value;
		}

	private:
		T* const& handle;
		bool ok = false;
	};

	template<typename T>
	Watcher(util::add_pointer_t<T>) -> Watcher<T>;
}
