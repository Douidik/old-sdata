#ifndef SDATA_TYPE_TO_STRING
#define SDATA_TYPE_TO_STRING

#include <cxxabi.h>

#include <string>
#include <string_view>
#include <typeinfo>
#include <unordered_map>

namespace sdata {

template <typename T>
std::string_view type_to_string() {
  static std::string s_type_name = "";

  if (s_type_name.empty()) {  // Type not already demangled
    int status = -4;
    const char *mangled_type_name = typeid(T).name();
    char *buffer = abi::__cxa_demangle(mangled_type_name, nullptr, nullptr, &status);

    // Use the mangled name if the demangling process doesn't worked
    s_type_name = status < 0 ? mangled_type_name : buffer;
    delete buffer;
  }

  return s_type_name;
}

}  // namespace sdata

#endif
