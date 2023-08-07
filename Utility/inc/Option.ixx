export module Utility.Option;
import <vector>;
import <functional>;
import Utility;
import Utility.Constraints;

export namespace util
{
	template<typename InternalOpt>
	class Option
	{
	public:
		using OptHandler = std::function<void(const InternalOpt& option)>;

		constexpr Option() noexcept
			: myOption()
			, myHandlers()
		{
			myHandlers.reserve(4);
		}

		constexpr Option(const InternalOpt init) noexcept(nothrow_copy_constructibles<InternalOpt>)
			: myOption(init)
			, myHandlers()
		{}

		constexpr Option(const InternalOpt init, const OptHandler handler) noexcept(nothrow_copy_constructibles<InternalOpt>)
			: Option(init)
		{
			myHandlers.reserve(4);
			myHandlers.push_back(handler);
		}

		constexpr ~Option() noexcept
		{
			myHandlers.clear();
			myHandlers.shrink_to_fit();
		}

		constexpr Option& operator=(const InternalOpt& option) noexcept
		{
			if (option != myOption)
			{
				for (OptHandler& handler : myHandlers)
				{
					handler(option);
				}

				myOption = option;
			}

			return *this;
		}

		constexpr InternalOpt& operator*() & noexcept
		{
			return myOption;
		}

		constexpr const InternalOpt& operator*() const& noexcept
		{
			return myOption;
		}

		constexpr InternalOpt&& operator*() && noexcept
		{
			return util::move(myOption);
		}

		constexpr const InternalOpt&& operator*() const&& noexcept
		{
			return util::move(myOption);
		}

		constexpr void AddListener(const OptHandler handler) noexcept
		{
			myHandlers.push_back(handler);
		}

		constexpr InternalOpt& Get() & noexcept
		{
			return myOption;
		}

		constexpr const InternalOpt& Get() const& noexcept
		{
			return myOption;
		}

		constexpr InternalOpt&& Get() && noexcept
		{
			return util::move(myOption);
		}

		constexpr const InternalOpt&& Get() const&& noexcept
		{
			return util::move(myOption);
		}

		Option(const Option&) = delete;
		constexpr Option(Option&&) noexcept = default;
		Option& operator=(const Option&) = delete;
		constexpr Option& operator=(Option&&) noexcept = default;

	private:
		InternalOpt myOption;
		std::vector<OptHandler> myHandlers;
	};


	template<typename InternalOpt>
	class Option<InternalOpt&>
	{
	public:
		using OptHandler = std::function<void(InternalOpt& option)>;

		constexpr Option() noexcept
			: myHandlers()
		{
			myHandlers.reserve(4);
		}

		constexpr Option(const std::initializer_list<OptHandler> handler) noexcept
			: Option()
		{
			myHandlers.append_range(handler);
		}

		constexpr ~Option() noexcept
		{
			myHandlers.clear();
			myHandlers.shrink_to_fit();
		}

		constexpr Option& operator=(InternalOpt& option) noexcept
		{
			for (OptHandler& handler : myHandlers)
			{
				handler(option);
			}

			return *this;
		}

		constexpr void AddListener(const OptHandler handler) noexcept
		{
			myHandlers.push_back(handler);
		}

		Option(const Option&) = delete;
		constexpr Option(Option&&) noexcept = default;
		Option& operator=(const Option&) = delete;
		constexpr Option& operator=(Option&&) noexcept = default;

	private:
		std::vector<OptHandler> myHandlers;
	};
}
