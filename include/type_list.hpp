#pragma once

#include <concepts>
#include "util.hpp"
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

template<class TT>
using from_tuple = decltype(
    []<template<class...> class TTuple, class... TData>(
        TTuple<TData...>
    ) -> from_pack<TData...> {
    }(std::declval<TT>())
);

template<class VT>
using from_value_tuple = decltype(
    []<class T, template<class, T...> class VTuple, T... Data>(
        VTuple<T, Data...>
    ) -> from_pack<value_tag<Data>...> {
    }(std::declval<VT>())
);

namespace __detail {
    template<template <class...> class TTuple, type_list, class... TData>
    struct to_tuple {
        using type = TTuple<TData...>;
    };
    template<template <class...> class TTuple, type_sequence TL, class... TData>
    struct to_tuple<TTuple, TL, TData...> {
        using type =
            to_tuple
            < TTuple
            , typename TL::tail
            , TData...
            , typename TL::head
            >::type;
    };
}
template<template <class...> class TTuple, type_list TL>
using to_tuple = __detail::to_tuple<TTuple, TL>::type;

namespace __detail {
    template<class...>
    struct TTuple{};
};
template<template <auto...> class TTuple, type_list TL>
using to_value_tuple = decltype(
    []<class... TData>(
        __detail::TTuple<TData...>
    ) -> TTuple<TData::value...> {
    }(std::declval<to_tuple<__detail::TTuple, TL>>())
);

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

template<std::size_t N, class T>
using replicate = take<N, repeat<T>>;

template<template<class> class F, class T>
struct iterate {
    using head = T;
    using tail = iterate<F, F<T>>;
};

namespace __detail {
    template<type_list TL, type_list TOriginal>
    struct cycle : nil {};
    template<empty TL, type_sequence TOriginal>
    struct cycle<TL, TOriginal> {
        using head = TOriginal::head;
        using tail = cycle<typename TOriginal::tail, TOriginal>;
    };
    template<type_sequence TL, type_sequence TOriginal>
    struct cycle<TL, TOriginal> {
        using head = TL::head;
        using tail = cycle<typename TL::tail, TOriginal>;
    };
}
template<type_list TL>
using cycle = __detail::cycle<TL, TL>;

template<template<class> class F, class TL>
struct map : nil {};
template<template<class> class F, type_sequence TL>
struct map<F, TL> {
    using head = F<typename TL::head>;
    using tail = map<F, typename TL::tail>;
};

template<type_list TL>
struct inits {
    using head = nil;
    using tail = nil;
};
template<type_sequence TL>
struct inits<TL> {
    template<type_list TArg>
    using prepend = cons<typename TL::head, TArg>;
    using head = nil;
    using tail = map<prepend, inits<typename TL::tail>>;
};

template<type_list TL>
struct tails {
    using head = nil;
    using tail = nil;
};
template<type_sequence TL>
struct tails<TL> {
    using head = TL;
    using tail = tails<typename TL::tail>;
};

template<template<class, class> class OP, class T, class TL>
struct scanl {
    using head = T;
    using tail = nil;
};
template<template<class, class> class OP, class T, type_sequence TL>
struct scanl<OP, T, TL> {
    using head = T;
    using tail = scanl<OP, OP<T, typename TL::head>, typename TL::tail>;
};
}