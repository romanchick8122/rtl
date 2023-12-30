#include <static_map.hpp>

#include <type_traits>
using sample_map = rtl::static_map<rtl::make_key_value_pair<int, float>, rtl::make_key_value_pair<float, int>>;
static_assert(std::is_same_v<sample_map::at<int>, float>);
static_assert(std::is_same_v<sample_map::at<float>, int>);
int main(int, char**) {
}
