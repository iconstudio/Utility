#include <vector>

import Utility;
import Utility.Print;
import Utility.Coroutine;

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
	auto it = tttvec.begin();

	util::corepeat([&]() -> bool {
		util::Println("{} ", *it);

		++it;

		return it == tttvec.end();
	});
	util::Println("");

	return 0;
}
