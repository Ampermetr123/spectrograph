#include <string>
#include <type_traits>
#include "ocv.h"


template <typename T>
T load_or_default(cv::FileStorage& fs, std::string node_name, std::string param_name, T default_val) {
    auto node = fs[node_name];
    auto v = node[param_name];
    if ( v.empty() ||
        (std::is_integral_v<T> && !v.isInt()) ||
        (std::is_floating_point_v<T> && !v.isReal()) ||
        (!std::is_integral_v<T> && !std::is_floating_point_v<T> && !v.isString() )  )
    {
        return default_val;
    }
    else {
        return static_cast<T>(v);
    }
}