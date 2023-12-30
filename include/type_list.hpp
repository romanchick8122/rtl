#pragma once

#include <concepts>
namespace rtl::type_list {
struct nil {};
template<class TL>
concept empty = std::derived_from<TL, nil>;
template<class TL>
concept type_sequence = requires {
    typename TL::head;
    typename TL::tail;
};
template<class TL>
concept type_list = empty<TL> || type_sequence<TL>;
template<class THead, type_list TTail>
struct cons {
    using head = THead;
    using tail = TTail;
};

template<class...>
struct from_pack : nil {};
template<class TFirst, class... TRest>
struct from_pack<TFirst, TRest...> {
    using head = TFirst;
    using tail = from_pack<TRest...>;
};

template<template<class...> class TTuple, class TT>
struct from_tuple;
template<template<class...> class TTuple, class... TData>
struct from_tuple<TTuple, TTuple<TData...>> : from_pack<TData...> {};

namespace __detail {
    template<template <class...> class TTuple, type_list, class... TData>
    struct to_tuple {
        using type = TTuple<TData...>;
    };
    template<template <class...> class TTuple, type_sequence TL, class... TData>
    struct to_tuple<TTuple, TL, TData...> {
        using type = to_tuple<TTuple, typename TL::tail, TData..., typename TL::head>::type;
    };
}
template<template <class...> class TTuple, type_list TL>
using to_tuple = __detail::to_tuple<TTuple, TL>::type;

template<class T>
struct repeat {
    using head = T;
    using tail = repeat<T>;
};

template<std::size_t N, type_list TL>
struct take : nil {};
template<std::size_t N, type_sequence TL>
requires (N > 0)
struct take<N, TL> {
    using head = TL::head;
    using tail = take<N - 1, typename TL::tail>;
};

template<std::size_t N, type_list TL>
struct drop : nil {};
template<type_list TL>
struct drop<0, TL> : TL {};
template<std::size_t N, type_sequence TL>
struct drop<N, TL> : drop<N - 1, typename TL::tail> {};
}