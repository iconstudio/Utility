#include <vector>
#include <array>
#include <initializer_list>

import Utility;
import Utility.Print;
import Utility.Coroutine;
import Utility.Coroutine.Generator;
import Utility.Coroutine.Enumerator;
import Utility.Coroutine.Logic;

int main()
{
	util::Println("Hello, Utility!");

	constexpr auto sqr1 = util::sqr(3);
	constexpr auto sqr2 = util::sqr(3.0);
	constexpr auto sqr3 = util::sqr(2631701985);
	constexpr auto sqr4 = util::sqr(40735.61);
	util::Println("Testing squares: {}, {}, {}, {}", sqr1, sqr2, sqr3, sqr4);

	constexpr auto sqrt1 = util::sqrt(25.0);
	constexpr auto sqrt2 = util::sqrt(25.0f);
	constexpr auto sqrt3 = util::sqrt(1676.0);
	constexpr auto sqrt4 = util::sqrt(16.0f);
	util::Println("Testing square roots: {}, {}, {}, {}", sqrt1, sqrt2, sqrt3, sqrt4);

	for (auto&& val : util::coiota('A', 'Z' + 1))
	{
		util::Print("{} ", val);
	}
	util::Println("");

	const std::vector<int> tttvec{ 0, 1, 2, 4, 5 };

	// test 1
	auto it1 = tttvec.begin();
	auto task1 = util::corepeat(
		[&]() noexcept -> bool {
		util::Print("{} ", *it1);

		++it1;

		return it1 != tttvec.end();
	});

	while (!task1.IsDone())
	{
		task1();
	}
	util::Println("");

	// test 2
	auto it2 = tttvec.rbegin();
	auto task2 = util::corepeat_as_if<util::coexecution::Later>(
		[&]() noexcept {
		util::Print("{} ", *it2);
		++it2;
	}, [&]() noexcept -> bool {
		return it2 != tttvec.rend();
	});

	while (!task2.IsDone())
	{
		task2();
	}
	util::Println("");

	// test 3
	const std::vector<int> test3_vec({ 131233, 4, 5, 6, 7, 8, 353463 });
	auto task3 = util::coenumerate(test3_vec);
	auto task3_begin = std::begin(task3);
	auto task3_end = std::end(task3);

	auto task3_rngbegin = std::ranges::begin(task3);
	auto task3_rngend = std::ranges::end(task3);
	
	using task3_type = decltype(task3);
	//using task3_iter_type = std::ranges::iterator_t<decltype(task3)>;
	using task3_iter_type = task3_type::iterator;

	constexpr bool is_range_tk3 = std::ranges::range<task3_type>;
	constexpr bool has_adl_tk3 = std::ranges::_Begin::_Has_ADL<task3_type>;
	constexpr bool has_mem_tk3 = std::ranges::_Begin::_Has_member<task3_type>;

	constexpr bool it_tk3 = std::input_or_output_iterator<task3_iter_type>;
	constexpr bool it_tk3_incr = std::weakly_incrementable<task3_iter_type>;
	//constexpr bool has_iter_adl_tk3 = std::ranges::_Begin::_Has_ADL<task3_iter_type>;
	//constexpr bool has_mem_iter_tk3 = std::ranges::_Begin::_Has_member<task3_iter_type>;

	util::Println("Const Left value");
	for (auto&& vv : util::coenumerate(test3_vec))
	{
		util::Print("{} ", vv);
	}
	util::Println("");

	util::Println("Right value");
	util::Print("ref: ");
	std::vector<int> test3_vec2({ 131233, 4, 5, 6, 7, 8, 353463 });
	for (auto&& vv : util::coenumerate(test3_vec2))
	{
		//vv += 40;

		util::Print("{} ", vv);
	}
	util::Println("");

	util::Print("with view: ");
	//for (auto&& vv : util::coenumerate(test3_vec2) | std::views::take(5))
	{
		//vv += 40;

		//util::Print("{} ", vv);
	}
	util::Println("");

	util::Println("Pure Right value");
	// ???
	util::Print("vector: ");
	for (auto&& vv : util::coenumerate(std::vector<int>({ 131233, 4, 5, 6, 7, 8, 353463 })))
	{
		util::Print("{} ", vv);
	}
	util::Println("");

	util::Print("array: ");
	for (auto&& vv : util::coenumerate(std::array<int, 50>{ 131233, 4, 5, 6, 7, 8, 353463 }))
	{
		util::Print("{} ", vv);
	}
	util::Println("");

	util::Println("Copied Pure Right value");
	// ???
	for (auto&& vv : util::coenumerate(std::vector(test3_vec2)))
	{
		util::Print("{} ", vv);
	}
	util::Println("");

	util::Println("Const Right value");
	for (auto&& vv : util::coenumerate(std::move(test3_vec)))
	{
		util::Print("{} ", vv);
	}
	util::Println("");

	// test 4
	util::Println("Algorithm 1 - iota");
	auto io_range1 = util::coiota(0, 15);
	auto io_range2 = util::coiota(1ULL
		, [](const unsigned long long& curr) {
		return curr * 2;
	});

	for (auto&& val : io_range1)
	{
		util::Print("{} ", val);
	}
	util::Println("");

	for (auto&& val : util::coiota(10, 50) | std::views::take(20))
	{
		util::Print("{} ", val);
	}
	util::Println("");
	for (auto&& val : io_range2 | std::views::take(20))
	{
		util::Print("{} ", val);
	}
	util::Println("");

	return 0;
}
