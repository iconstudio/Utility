export module Utility.Classifier;

export namespace util
{
	template<typename T>
	struct class_id;

	template<typename T = void>
	class Classifier;

	template<typename T>
	class Classifier
	{
	public:
		constexpr Classifier() noexcept = default;
		constexpr Classifier(const Classifier& other) noexcept = default;
		constexpr Classifier(Classifier&& other) noexcept = default;
		constexpr Classifier& operator=(const Classifier& other) noexcept = default;
		constexpr Classifier& operator=(Classifier&& other) noexcept = default;
		constexpr ~Classifier() noexcept = default;

		[[nodiscard]]
		static consteval size_t GetClassID() noexcept
		{
			return class_id<T>::template id;
		}
	};

	template<>
	class Classifier<void>
	{
		constexpr Classifier() noexcept = default;
		constexpr Classifier(const Classifier& other) noexcept = default;
		constexpr Classifier(Classifier&& other) noexcept = default;
		constexpr Classifier& operator=(const Classifier& other) noexcept = default;
		constexpr Classifier& operator=(Classifier&& other) noexcept = default;
		constexpr ~Classifier() noexcept = default;

		template<typename T>
		[[nodiscard]]
		static consteval size_t GetClassID() noexcept
		{
			return class_id<T>::template id;
		}
	};
}
