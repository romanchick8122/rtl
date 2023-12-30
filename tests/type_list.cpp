#include "util.hpp"
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <type_list.hpp>
#include <tuple>
#include <type_traits>
#include <utility>

using namespace rtl::type_list;
template<type_list TL>
using to_std_tuple = to_tuple<std::tuple, TL>;
static_assert(
    std::same_as
    < to_std_tuple<cons<int, cons<bool, cons<float, nil>>>>
    , std::tuple<int, bool, float>
    >
);
static_assert(
    std::same_as
    < to_std_tuple<from_pack<int, bool, float>>
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
    < to_std_tuple<take<3, repeat<int>>>
    , std::tuple<int, int, int>
    >
);
static_assert(
    std::same_as
    < to_std_tuple<take<2, from_tuple<std::tuple<int, bool, float>>>>
    , std::tuple<int, bool>
    >
);
static_assert(
    std::same_as
    < to_std_tuple<take<0, from_pack<int, bool, float, double>>>
    , std::tuple<>
    >
);
static_assert(
    std::same_as
    < to_std_tuple<take<10, from_pack<int, bool, float, double>>>
    , std::tuple<int, bool, float, double>
    >
);
static_assert(
    std::same_as
    < to_std_tuple<drop<3, from_pack<float, int, float, int>>>
    , std::tuple<int>
    >
);
static_assert(
    std::same_as
    < to_std_tuple<drop<0, from_pack<float, int, float, int>>>
    , std::tuple<float, int, float, int>
    >
);
static_assert(
    std::same_as
    < to_std_tuple<drop<10, from_pack<float, int, float, int>>>
    , std::tuple<>
    >
);
static_assert(
    std::same_as
    < to_std_tuple<replicate<3, int>>
    , std::tuple<int, int, int>
    >
);
template <class T>
using make_ptr = T*;
static_assert(
    std::same_as
    < to_std_tuple<take<4, iterate<make_ptr, int>>>
    , std::tuple<int, int*, int**, int***>
    >
);
static_assert(
    std::same_as
    < to_std_tuple<take<5, cycle<from_pack<int, bool>>>>
    , std::tuple<int, bool, int, bool, int>
    >
);
static_assert(
    std::same_as
    < to_std_tuple<map<make_ptr, from_pack<int, bool, float>>>
    , std::tuple<int*, bool*, float*>
    >
);
template<std::size_t N>
struct curry_take {
    template<type_list TL>
    using type = take<N, TL>;
};
static_assert(
    std::same_as
    < to_std_tuple
      < map
        < to_std_tuple
        , take
          < 3
          , map
            < curry_take<3>::template type
            , repeat<repeat<float>>
            >
          >
        >
      >
    , std::tuple
      < std::tuple<float, float, float>
      , std::tuple<float, float, float>
      , std::tuple<float, float, float>
      >
    >
);
static_assert(
    std::same_as
    < to_std_tuple<map<make_ptr, nil>>
    , std::tuple<>
    >
);
static_assert(
    std::same_as
    < to_std_tuple<map<to_std_tuple, inits<from_pack<int, char, bool, float>>>>
    , std::tuple
      < std::tuple<>
      , std::tuple<int>
      , std::tuple<int, char>
      , std::tuple<int, char, bool>
      , std::tuple<int, char, bool, float>
      >
    >
);
static_assert(
    std::same_as
    < to_std_tuple<map<to_std_tuple, inits<nil>>>
    , std::tuple<std::tuple<>>
    >
);
static_assert(
    std::same_as
    < to_std_tuple<map<to_std_tuple, tails<from_pack<int, char, bool, float>>>>
    , std::tuple
      < std::tuple<int, char, bool, float>
      , std::tuple<char, bool, float>
      , std::tuple<bool, float>
      , std::tuple<float>
      , std::tuple<>
      >
    >
);
static_assert(
    std::same_as
    < to_std_tuple<map<to_std_tuple, tails<nil>>>
    , std::tuple<std::tuple<>>
    >
);
static_assert(empty<cycle<nil>>);
template<type_list TL>
using first3 = to_std_tuple<take<3, TL>>;
static_assert(
    std::same_as
    < first3
      < map
        < first3
        , map
          < cycle
          , inits<iterate<make_ptr, int>>
          >
        >
      >
    , std::tuple
      < std::tuple<>
      , std::tuple<int, int, int>
      , std::tuple<int, int*, int>
      >
    >
);
static_assert(
    std::same_as
    < to_std_tuple
      < take
        < 4
        , map
          < to_std_tuple
          , inits<iterate<make_ptr, int>>
          >
        >
      >
    , std::tuple
      < std::tuple<>
      , std::tuple<int>
      , std::tuple<int, int*>
      , std::tuple<int, int*, int**>
      >
    >
);
static_assert(
    std::same_as
    < first3
      < map
        < first3
        , tails<iterate<make_ptr, int>>
        >
      >
    , std::tuple
      < std::tuple<int, int*, int**>
      , std::tuple<int*, int**, int***>
      , std::tuple<int**, int***, int****>
      >
    >
);
template<class A, class B>
using max_by_value = std::conditional_t<(A::value > B::value), A , B>;
static_assert(
    std::same_as
    < to_value_tuple
      < std::index_sequence
      , scanl
        < max_by_value
        , rtl::value_tag<1>
        , from_value_tuple<std::index_sequence<1, 1, 2, 1, 4, 2, 8, 4>>
        >
      >
    , std::index_sequence<1, 1, 1, 2, 2, 4, 4, 8, 8>
    >
);
static_assert(
    std::same_as
    < foldl
      < max_by_value
      , rtl::value_tag<1>
      , from_value_tuple<std::index_sequence<1, 1, 2, 1, 4, 2, 8, 4>>
      >
    , rtl::value_tag<std::size_t{8}>
    >
);
static_assert(
    std::same_as
    < foldl
      < max_by_value
      , rtl::value_tag<1>
      , nil
      >
    , rtl::value_tag<1>
    >
);
template<class T>
using small_type = std::integral_constant<bool, sizeof(T) <= 4>;
static_assert(
    std::same_as
    < to_std_tuple
      < filter
        < small_type
        , from_pack<std::int32_t, bool, std::uint64_t, std::int8_t, std::int64_t, std::int16_t>
        >
      >
    , std::tuple<std::int32_t, bool, std::int8_t, std::int16_t>
    >
);
static_assert(
    std::same_as
    < to_std_tuple<filter<small_type, nil>>
    , std::tuple<>
    >
);
static_assert(
    std::same_as
    < to_std_tuple
      < take
        < 5
        , filter
          < small_type
          , cycle<from_pack<std::int32_t, bool, std::int64_t>>
          >
        >
      >
    , std::tuple<std::int32_t, bool, std::int32_t, bool, std::int32_t>
    >  
);
static_assert(
    std::same_as
    < to_std_tuple
      < map
        < tuple_cast_to<std::tuple>::from
        , take<10, zip<repeat<int>, repeat<float>>>
        >
      >
      , to_std_tuple<take<10, repeat<std::tuple<int, float>>>>
    >
);
static_assert(
    std::same_as
    < to_std_tuple
      < map
        < tuple_cast_to<std::tuple>::from
        , take<10, zip<repeat<int>, repeat<float>, repeat<double>>>
        >
      >
      , to_std_tuple<take<10, repeat<std::tuple<int, float, double>>>>
    >
);
static_assert(
    std::same_as
    < to_std_tuple<take<10, zip<from_pack<bool, char, int>, nil>>>
    , std::tuple<>
    >
);
static_assert(
    std::same_as
    < to_std_tuple
      < take
        < 10
        , map
          < tuple_cast_to<std::tuple>::from
          , zip<from_pack<bool, char, int>, repeat<float>>
          >
        >
      >
    , std::tuple
      < std::tuple<bool, float>
      , std::tuple<char, float>
      , std::tuple<int, float>
      >
    >  
);
int main(int, char**) {
}