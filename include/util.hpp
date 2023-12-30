#pragma once

namespace rtl {
template<auto v>
struct value_tag {
    static constexpr auto value = v;
};
}