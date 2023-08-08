export module Utility.Coroutine;
export import :CoIterator;
export import :Promise;
export import :Task;

export namespace util::coroutine
{
	template<coexecution Policy>
	using Cowork = conditional_t<Policy == coexecution::Now, RelaxedTask, DeferredTask>;
}
