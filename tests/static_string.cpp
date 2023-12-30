#include <static_string.hpp>

template<rtl::static_string> struct literal_template{};
static_assert(requires {
    literal_template<"string">{};
});
int main(int, char**) {
}
