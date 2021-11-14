#ifndef SDATA_FMT_HPP
#define SDATA_FMT_HPP

#include <algorithm>
#include <deque>
#include <initializer_list>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include "assert.hpp"

namespace sdata {

// NOTE: this implementation is temporary, std::format is now available on C++20
// unfortunately std::format is not yet implemented on GCC or clang

template <typename T, typename CharT>
concept streamable = requires(std::basic_ostream<CharT> os, T e) {
  {os << e};
};

namespace format {

template <typename CharT>
inline std::deque<std::basic_ostringstream<CharT>> stream_args() {
  return {};  // base case
}

template <typename CharT, typename... Ts>
std::deque<std::basic_ostringstream<CharT>> stream_args(auto &argument, Ts &...rest) {
  static_assert(streamable<decltype(argument), CharT>, "Format argument is not streamable");

  std::deque<std::basic_ostringstream<CharT>> streams = stream_args<CharT>(rest...);
  {
    auto &stream = streams.emplace_front();
    stream << argument;
  }
  return streams;
}

}  // namespace format

template <typename CharT, typename... Ts>
std::basic_string<CharT> fmt(std::basic_string_view<CharT> pattern, Ts... args) {
  SDATA_ASSERT(std::ranges::count(pattern, '%') == sizeof...(Ts),
               "Wrong '%' token count in format pattern");

  std::basic_ostringstream<CharT> stream;
  auto args_streams = format::stream_args<CharT>(args...);

  for (const char &token : pattern) {
    if (token == CharT('%')) {
      stream << args_streams.front().str();
      args_streams.pop_front();
    } else {
      stream.put(token);
    }
  }

  return stream.str();
}

}  // namespace sdata

#endif
