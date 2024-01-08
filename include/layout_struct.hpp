#pragma once

#include <static_string.hpp>
#include <utility>
namespace rtl::layout_struct {
template<class T, rtl::static_string Name>
struct field {
    using type = T;
    constexpr static auto name = Name;
};
template<class T>
concept is_field = requires {
    []<class TArg, rtl::static_string Name>(field<TArg, Name>){}(std::declval<T>());
};
template<class T>
concept layout = true; // placeholder
template<layout TLayout, is_field... TFields>
struct layout_struct {};
};