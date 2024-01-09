#pragma once

#include "util.hpp"
#include <cstddef>
#include <cstring>
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
namespace __detail {
    template<class TLayout, is_field... TFields>
    struct layout_struct_builder {
        using impl = TLayout::template exec<TFields...>;

        template<is_field T>
        using key_lookup_func = rtl::make_key_value_pair<rtl::value_tag<T::name>, T>;
        using key_lookup =
        type_list::to_tuple
        < rtl::static_map
        , type_list::map<key_lookup_func, type_list::from_pack<TFields...>>
        >;

        struct alignas(impl::alignment) storage_raw {
            std::byte data[impl::size_of];
        };

        template<is_field T>
        constexpr static bool aligned_field = 
        ( impl::offsets::template at<T>::value
        % std::alignment_of_v<typename T::type>
        ) == 0;

        template<is_field T>
        struct v_impl_type {
            static_assert(false, "No suitable value implementation found");
        };
        template<is_field T>
        requires(aligned_field<T>)
        struct v_impl_type<T> {
            T::type& v_impl(rtl::value_tag<T::name>) {
                auto data_ptr = reinterpret_cast<std::byte*>(this);
                data_ptr += impl::offsets::template at<T>::value;
                return *reinterpret_cast<T::type*>(data_ptr);
            }
        };
        template<class T>
        struct access_proxy {
            std::byte* ptr;
            operator T() {
                T result;
                std::memcpy(&result, ptr, sizeof(T));
                return result;
            }
            access_proxy<T>& operator=(const T& value) {
                std::memcpy(ptr, &value, sizeof(T));
                return *this;
            }
        };
        template<is_field T>
        requires(!aligned_field<T> && std::is_trivially_copyable_v<typename T::type>)
        struct v_impl_type<T> {
            auto v_impl(rtl::value_tag<T::name>) {
                auto data_ptr = reinterpret_cast<std::byte*>(this);
                data_ptr += impl::offsets::template at<T>::value;
                return access_proxy<typename T::type>{ data_ptr };
            }
        };

        struct type
        : private storage_raw
        , private v_impl_type<TFields>...
        {
          private:
            using v_impl_type<TFields>::v_impl...;
          public:
            template<rtl::static_string Name>
            decltype(auto) v() {
                return v_impl(rtl::value_tag<Name>{});
            }
        };
        // static assert to prevent undefined behaviour inside the implimentation.
        // should never trigger, but just in case.
        static_assert(std::is_standard_layout_v<type>);
    };
}
template<class TLayout, is_field... TFields>
using layout_struct = __detail::layout_struct_builder<TLayout, TFields...>::type;

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
            template<class A, class B>
            using max_by_alignment = rtl::value_tag
            < std::max(A::value, std::alignment_of_v<typename B::type>)
            >;
        public:
            using offsets = type_list::to_tuple<rtl::static_map, raw_offsets>;
            constexpr static std::size_t alignment =
            std::min
            ( Alignment
            , type_list::foldl
              < max_by_alignment
              , rtl::value_tag<std::size_t{}>
              , type_list::from_pack<TFirst, TRest...>
              >::value
            );
            constexpr static std::size_t size_of = type_list::foldl
            < max_end
            , rtl::value_tag<std::size_t{0}>
            , raw_offsets
            >::value;
        };
    };
}
}