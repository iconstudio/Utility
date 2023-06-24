module;
#include <ctime>
#include <chrono>
export module Utility.Time;

export namespace util
{
	using ::std::get_time;

	using ::std::operator<<;
	using ::std::operator==;
	using ::std::operator<=>;

	using namespace ::std::chrono;
}
