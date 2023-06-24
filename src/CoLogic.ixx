export module Utility.Coroutine.Logic;
import <algorithm>;
import <ranges>;
import <vector>;
import Utility;
import Utility.Coroutine;
import Utility.Coroutine.Enumerator;

export using ::std::operator|;

export namespace util
{
	template<enumerable Rng, typename Pred>
	inline coroutine::DeferredTask
		co_each(Rng&& rng, Pred&& predicate)
		noexcept
	{
		auto&& pred = forward<Pred>(predicate);

		for (auto&& value : util::coenumerate(forward<Rng>(rng)))
		{
			co_await pred(value);
		}

		const std::vector vb{ 0, 2, 34, 54, 56, 654, 75 };
		const auto filter = coenumerate(vb) | std::views::filter([](auto&& val) { return val > 50; });
	}
}
