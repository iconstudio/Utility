export module Utility.Singleton;

export namespace util
{
	template<typename T>
	struct Singleton
	{
		constexpr Singleton(T* instance) noexcept
		{
			Instance = instance;
		}

		constexpr ~Singleton() noexcept = default;

		Singleton(const Singleton& other) noexcept = delete;
		Singleton& operator=(const Singleton& other) noexcept = delete;
		Singleton(Singleton&& other) noexcept = default;
		Singleton& operator=(Singleton&& other) noexcept = default;

		static inline T* Instance = nullptr;
	};
}
