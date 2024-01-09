#pragma once

#include "util.hpp"
#include <cstddef>
#include <static_map.hpp>
#include <static_string.hpp>
#include <type_list.hpp>
#include <type_traits>
#include <utility>
namespace rtl {
template<class T, rtl::static_string Name>
struct field {
    using type = T;
    constexpr static auto name = Name;
};
template<class T>
concept is_field = requires {
    []<class TArg, rtl::static_string Name>(field<TArg, Name>){}(std::declval<T>());
};
template<class TLayout, is_field... TFields>
struct layout_struct {};

namespace struct_layouts {
    namespace __detail {
        template<std::size_t Offset, std::size_t Alignment>
        constexpr static std::size_t layout_align_offset
        = Offset % Alignment == 0
        ? Offset
        : Offset + Alignment - (Offset % Alignment);
    }

    template<std::size_t Alignment = std::alignment_of_v<std::max_align_t>>
    struct pack {
        template<is_field... TFields>
        struct exec {
            using offsets = rtl::static_map<>;
            constexpr static std::size_t alignment = Alignment;
            constexpr static std::size_t size_of = 0;
        };
        template<is_field TFirst, is_field... TRest>
        struct exec<TFirst, TRest...> {
        private:
            template<key_value_pair A, class B>
            using offset_scan_op = rtl::make_key_value_pair
            < B
            , rtl::value_tag
              < __detail::layout_align_offset
                < A::value::value + sizeof(typename A::key::type)
                , std::min(Alignment, std::alignment_of_v<typename B::type>)
                >
              >
            >;
            using raw_offsets = type_list::scanl
            < offset_scan_op
            , rtl::make_key_value_pair<TFirst, rtl::value_tag<0>>
            , type_list::from_pack<TRest...>
            >;
            template<class A, class B>
            using max_end = rtl::value_tag
            < std::max(A::value, sizeof(typename B::key::type) + B::value::value)
            >;
        public:
            using offsets = type_list::to_tuple<rtl::static_map, raw_offsets>;
            constexpr static std::size_t alignment = Alignment;
            constexpr static std::size_t size_of = type_list::foldl
            < max_end
            , rtl::value_tag<std::size_t{0}>
            , raw_offsets
            >::value;
        };
    };
}
}