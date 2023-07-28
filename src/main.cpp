#include <vector>
#include <array>
#include <initializer_list>

import Utility;
import Utility.Print;
import Utility.Coroutine;
import Utility.Coroutine.Generator;
import Utility.Coroutine.Enumerator;
import Utility.Coroutine.Logic;
import Utility.Atom;

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

	for (auto&& val : util::cogenerate('A', 'Z' + 1))
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

	util::Println("[{}]", "Const Left value");
	for (auto&& vv : util::coenumerate(test3_vec))
	{
		util::Print("{} ", vv);
	}
	util::Println("\n");

	util::Println("Right value");
	util::Print("ref: ");
	std::vector<int> test3_vec2({ 131233, 4, 5, 6, 7, 8, 353463 });
	for (auto&& vv : util::coenumerate(test3_vec2))
	{
		//vv += 40;

		util::Print("{} ", vv);
	}
	util::Println("");

	util::Print("with view({}): ", 5);
	for (auto&& vv : util::coenumerate(test3_vec2) | std::views::take(5))
	{
		vv += 40;

		util::Print("{} ", vv);
	}
	util::Println("\n");

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
	util::Println("\n");

	util::Println("Copied Pure Right value");
	// ???
	for (auto&& vv : util::coenumerate(std::vector(test3_vec2)))
	{
		util::Print("{} ", vv);
	}
	util::Println("\n");

	util::Println("Const Right value");
	for (auto&& vv : util::coenumerate(std::move(test3_vec)))
	{
		util::Print("{} ", vv);
	}
	util::Println("\n");

	// test 4
	util::Println("Co-Generate");
	const util::coroutine::Generator aa = util::cogenerate(1);

	util::Print("from 1 to {}: ", 30);
	for (auto&& val : aa | std::views::take(30))
	{
		util::Print("{} ", val);
	}
	util::Println("");

	int cogen_first = 10;
	int cogen_addition = 30;
	util::Print("from {} plus 30: ", cogen_first, cogen_addition);
	for (auto&& val : util::cogenerate([&](const int& add) noexcept -> int { return add + cogen_first++; }, cogen_addition) | std::views::take(20))
	{
		util::Print("{} ", val);
	}
	util::Println("\n");

	// test 5
	util::Println("Algorithm 1 - iota");
	auto io_range1 = util::cogenerate(0, 15);
	auto io_range2 = util::coccumulate(1ULL
		, [](const unsigned long long& curr) {
		return curr * 2;
	});

	for (auto&& val : io_range1)
	{
		util::Print("{} ", val);
	}
	util::Println("");

	for (auto&& val : util::cogenerate(10, 50) | std::views::take(20))
	{
		util::Print("{} ", val);
	}
	util::Println("");
	for (auto&& val : io_range2 | std::views::take(20))
	{
		util::Print("{} ", val);
	}
	util::Println("\n");

	// test 5
	util::Atom atom00{ 400 };
	const util::Atom atom01{ 400 };
	const bool cmp_atom00_01 = atom00 == atom01;

	constexpr util::Atom atom02{ 400 };
	const bool cmp_atom01_02 = atom01 == atom02;

	util::Atom<int> atom03{ 400 };
	const util::Atom<int> atom04{ 400 };
	constexpr util::Atom<int> atom05{ 400 };

	util::Atom<const int> atom06{ 400 };
	const util::Atom<const int> atom07{ 400 };
	constexpr util::Atom<const int> atom08{ 400 };

	util::Atom<int*> atom09{ nullptr };
	const util::Atom<int*> atom10{ nullptr };
	constexpr util::Atom<int*> atom11{ nullptr };

	util::Atom<const int*> atom12{ nullptr };
	const util::Atom<const int*> atom13{ nullptr };
	constexpr util::Atom<const int*> atom14{ nullptr };

	const bool cmp_atom02_08 = atom02 == atom08;
	//const bool cmp_atom02_14 = atom02 == atom14;

	return 0;
}
