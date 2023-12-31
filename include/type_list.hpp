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

template<template<class...> class TNew>
struct tuple_cast_to {
    template<class TT>
    using from = decltype(
        []<template<class...> class TOld, class... TData>(
            TOld<TData...>
        ) -> TNew<TData...> {
        }(std::declval<TT>())
    );
};

template<class TT>
using from_tuple = tuple_cast_to<from_pack>::from<TT>;

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
            typename to_tuple
            < TTuple
            , typename TL::tail
            , TData...
            , typename TL::head
            >::type;
    };
}
template<template <class...> class TTuple, type_list TL>
using to_tuple = typename __detail::to_tuple<TTuple, TL>::type;

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
    using head = typename TL::head;
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
        using head = typename TOriginal::head;
        using tail = cycle<typename TOriginal::tail, TOriginal>;
    };
    template<type_sequence TL, type_sequence TOriginal>
    struct cycle<TL, TOriginal> {
        using head = typename TL::head;
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

namespace __detail {
    template<template<class, class> class OP, class T, class TL>
    struct foldl {
        using type = T;
    };
    template<template<class, class> class OP, class T, type_sequence TL>
    struct foldl<OP, T, TL> {
        using type =
            typename foldl
            < OP
            , OP<T, typename TL::head>
            , typename TL::tail
            >::type;
    };
}
template<template<class, class> class OP, class T, class TL>
using foldl = typename __detail::foldl<OP, T, TL>::type;

namespace __detail {
    template<template<class> class P, type_list TL>
    struct filter_skip {
        using type = typename filter_skip<P, typename TL::tail>::type;
    };
    template<template<class> class P, type_list TL>
    requires (P<typename TL::head>::value)
    struct filter_skip<P, TL> {
        using type = TL;
    };
}
template<template<class> class P, class TL>
struct filter : nil {};
template<template<class> class P, type_sequence TL>
struct filter<P, TL> {
    using head = typename __detail::filter_skip<P, TL>::type::head;
    using tail = filter<P, typename __detail::filter_skip<P, TL>::type::tail>;
};

namespace __detail {
    template<class T>
    using flattenl = decltype(
        []
        < template<class...> class TupOuter
        , template<class...> class TupInner
        , class... TInner
        , class TOuter
        >(
            TupOuter<TupInner<TInner...>, TOuter>
        ) -> TupOuter<TInner..., TOuter>{
        }(std::declval<T>())
    );
    template<template<class...> class TImpl>
    struct zip_helpers {
        template<class L, class R>
        struct zip2 : nil {};
        template<type_sequence L, type_sequence R>
        struct zip2<L, R> {
            using head = TImpl<typename L::head, typename R::head>;
            using tail = zip2<typename L::tail, typename R::tail>;
        };
        template<class L, class R>
        using zip_fold_expr = map<flattenl, zip2<L, R>>;
        template<typename T>
        using TTupleSingle = TImpl<T>;
    };
}
template<template<class...> class TImpl, class TFirst, class... TRest>
using zip =
foldl
< __detail::zip_helpers<TImpl>::template zip_fold_expr
, map<__detail::zip_helpers<TImpl>::template TTupleSingle, TFirst>
, from_pack<TRest...>
>;
}