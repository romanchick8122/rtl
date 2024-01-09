#include <layout_struct.hpp>
using rtl::field;

struct empty {};

using a = rtl::layout_struct
< rtl::struct_layouts::pack<1>
, field<int, "a">
, field<double, "b">
, field<empty, "c">
>;

static_assert(requires(a x, int& int_ref, double& double_ref, empty& empty_ref) {
    x.v<"a">() = 1;
    int_ref = x.v<"a">();
    x.v<"b">() = 1;
    double_ref = x.v<"b">();
    x.v<"c">() = {};
    empty_ref = x.v<"c">();
});
static_assert(std::alignment_of_v<a> == 1);
int main(int, char**) {
}