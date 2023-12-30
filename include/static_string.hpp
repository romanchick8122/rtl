#pragma once

#include<cstddef>
#include<algorithm>
namespace rtl {
template<std::size_t extent>
struct static_string {
    consteval static_string(const char(&data_)[extent]) {
        std::copy_n(data_, extent, data);
    }
    char data[extent];
};
}
