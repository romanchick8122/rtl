#include <static_string.hpp>

template<rtl::static_string value>
constexpr static char first_letter = value.data[0];

static_assert(first_letter<"James"> == 'J');
int main(int, char**) {
}
