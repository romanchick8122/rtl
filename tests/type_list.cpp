#include <concepts>
#include <type_list.hpp>
#include <tuple>

using namespace rtl::type_list;
static_assert(
    std::same_as
    < to_tuple<std::tuple, cons<int, cons<bool, cons<float, nil>>>>
    , std::tuple<int, bool, float>
    >
);
static_assert(
    std::same_as
    < to_tuple<std::tuple, from_pack<int, bool, float>>
    , std::tuple<int, bool, float>
    >
);
static_assert(
    requires {
        requires std::same_as<typename repeat<int>::head, int>;
        requires std::same_as<typename repeat<int>::tail::head, int>;
        requires std::same_as<typename repeat<int>::tail::tail::head, int>;
    }
);
static_assert(
    std::same_as
    < to_tuple<std::tuple, take<3, repeat<int>>>
    , std::tuple<int, int, int>
    >
);
static_assert(
    std::same_as
    < to_tuple<std::tuple, take<2, from_tuple<std::tuple, std::tuple<int, bool, float>>>>
    , std::tuple<int, bool>
    >
);
static_assert(
    std::same_as
    < to_tuple<std::tuple, take<0, from_pack<int, bool, float, double>>>
    , std::tuple<>
    >
);
static_assert(
    std::same_as
    < to_tuple<std::tuple, take<10, from_pack<int, bool, float, double>>>
    , std::tuple<int, bool, float, double>
    >
);
static_assert(
    std::same_as
    < to_tuple<std::tuple, drop<3, from_pack<float, int, float, int>>>
    , std::tuple<int>
    >
);
static_assert(
    std::same_as
    < to_tuple<std::tuple, drop<0, from_pack<float, int, float, int>>>
    , std::tuple<float, int, float, int>
    >
);
static_assert(
    std::same_as
    < to_tuple<std::tuple, drop<10, from_pack<float, int, float, int>>>
    , std::tuple<>
    >
);
int main(int, char**) {
}