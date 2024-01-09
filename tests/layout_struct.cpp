#include <layout_struct.hpp>
using rtl::field;

using a = rtl::layout_struct
< rtl::struct_layouts::pack<1>
, field<int, "a">
, field<double, "b">
, field<float, "c">
>;

static_assert(std::same_as<void, a>);

int main(int, char**) {
}