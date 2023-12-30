#pragma once

#include <utility>
namespace rtl {
template<typename T>
concept key_value_pair = requires {
    typename T::key;
    typename T::value;
};
namespace __detail {
    template<key_value_pair T>
    struct map_entry {
        static typename T::value at_impl(typename T::key);
    };
}
template<key_value_pair... TData>
struct static_map : private __detail::map_entry<TData>... {
    using __detail::map_entry<TData>::at_impl...;
    template<typename T>
    using at = decltype(at_impl(std::declval<T>()));
};
template<typename TKey, typename TValue>
struct make_key_value_pair {
    using key = TKey;
    using value = TValue;
};
}
