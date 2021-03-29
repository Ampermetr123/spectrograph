#include <string>
#include <type_traits>
#include <cstdlib>
#include "ocv.h"


template <typename T>
T load_or_default(cv::FileStorage& fs, std::string node_name, std::string param_name, T default_val) {

    cv::FileNode v;
    if (node_name.size()) {
        v = fs[node_name][param_name];     
    }
    else {
        v = fs[param_name];
    }
        
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

std::string get_user_dir(){
   const int sz = 2048;
   size_t len;
   char buf[sz];
   getenv_s(&len, buf, sz, "USERPROFILE");
   return std::string(buf);
}

