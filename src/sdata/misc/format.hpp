#ifndef SDATA_FORMAT_HPP
#define SDATA_FORMAT_HPP

#include <cstdio>
#include <sstream>
#include <string>
#include <string_view>

namespace sdata {

template <typename... Ts>
std::string format(std::string_view pattern, Ts... args) {
  size_t length = std::snprintf(nullptr, 0, pattern.data(), args...) + 1;
  char* buffer = new char[length];
  std::string formatted;
  
  if (std::snprintf(buffer, length, pattern.data(), args...)) {
    // snprintf outputs null terminated strings so we don't include the last character
    formatted = {buffer, length - 1};
  }

  delete[] buffer;
  return formatted;
}

}  // namespace sdata

#endif
