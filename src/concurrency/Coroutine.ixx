export module Utility.Coroutine;
import <tuple>;
export import <coroutine>;
export import :CoIterator;
export import :Promise;
export import :Task;
export import Utility;
export import Utility.Constraints;

export namespace util::coroutine
{
	template<coexecution Policy>
	using Cowork = conditional_t<Policy == coexecution::Now, RelaxedTask, DeferredTask>;
}
