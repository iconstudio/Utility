export module Utility.Named;
import Utility.String;

export namespace util
{
	/// <summary>
	/// Make a named object.
	/// </summary>
	class Named
	{
	public:
		constexpr Named() noexcept
			: myName()
		{}

		explicit constexpr Named(const string& name) noexcept
			: myName(name)
		{}

		explicit constexpr Named(string&& name) noexcept
			: myName(static_cast<string&&>(name))
		{}

		explicit constexpr Named(string_view name) noexcept
			: myName(name)
		{}

		constexpr void SetName(const string& name) noexcept
		{
			myName = name;
		}

		constexpr void SetName(string&& name) noexcept
		{
			myName = static_cast<string&&>(name);
		}

		constexpr void SetName(string_view name) noexcept
		{
			myName = name;
		}

		[[nodiscard]]
		constexpr const string& GetName() const& noexcept
		{
			return myName;
		}

		[[nodiscard]]
		constexpr string&& GetName() && noexcept
		{
			return static_cast<string&&>(myName);
		}

		constexpr Named(const Named& other) noexcept = default;
		constexpr Named(Named&& other) noexcept = default;
		constexpr Named& operator=(const Named& other) noexcept = default;
		constexpr Named& operator=(Named&& other) noexcept = default;
		constexpr ~Named() noexcept = default;

	private:
		string myName;
	};
}
