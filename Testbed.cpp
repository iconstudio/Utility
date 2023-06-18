#include <WS2tcpip.h>
#include <charconv>
#include <tuple>

#define __TEST__ 0

#if __TEST__
// 가변 템플릿, constexpr, 조건부 noexcept, 후속 반환형 사용
template<typename... Ts>
constexpr auto ForwardAsTuple(Ts&&... args) noexcept(std::conjunction_v<std::is_nothrow_constructible<Ts, Ts&&>...>)
-> std::tuple<Ts&&...>
{
    // 완벽한 전달(Perfect Forwarding) 사용
    return std::tuple<Ts&&...>{ std::forward<Ts>(args)... };
}

constexpr void FFunction1(auto value) noexcept
{}
constexpr void FFunction2(auto& value) noexcept
{}
constexpr void FFunction3(auto&& value) noexcept
{}

class Squirrel
{
public:
    const char* myName;
};

int main()
{
    constexpr long A = 0; // 0
    unsigned long long B = 93140583732; // 1
    bool C = false; // 2
    bool& D = C; // 3
    Squirrel E{}; // 4
    const Squirrel& F = E; // 5
    constexpr unsigned G = 34275860428; // 6

    auto BB = std::move(B);

    FFunction1(A);
    FFunction1(std::move(A));
    FFunction2(A);
    FFunction2(std::move(A));
    FFunction3(A);
    FFunction3(std::move(A));

    FFunction1(4000);
    FFunction2(4000);
    FFunction3(4000);

    auto tuple1 = std::make_tuple(A, B, C, D, E, F, G);
    std::get<2>(tuple1) = true; // 복사본은 원본 C, D에 영향을 주지 못함

    auto tuple2 = ForwardAsTuple(A, std::move(B), C, D, E, F, Squirrel{}, std::move(G));

    std::get<2>(tuple2) = true; // C, D가 true가 됨
    E.myName = "new name"; // E, F의 myName이 "new name"이 됨

    return 0;
}
#endif
