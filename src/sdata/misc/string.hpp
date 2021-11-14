#ifndef SDATA_STRING_HPP
#define SDATA_STRING_HPP

#include <locale>
#include <stdexcept>
#include <string>
#include <string_view>
#include "assert.hpp"

namespace sdata::string {

template <typename T, typename U, typename ConverterT = std::codecvt<T, U, std::mbstate_t>>
std::basic_string<U> convert(const std::basic_string_view<T> &src) {
  if constexpr (sizeof(T) <= sizeof(U)) {  // No conversion needed
    return {src.begin(), src.end()};
  }

  static auto &facet = std::use_facet<ConverterT>(std::locale{});

  std::basic_string<U> dst(src.size() * facet.max_length(), U{0x0});

  std::mbstate_t state;
  const T *src_next;
  U *dst_next;

  auto status =
      facet.out(state, &src[0], &src[src.size()], src_next, &dst[0], &dst[dst.size()], dst_next);

  switch (status) {
    case ConverterT::ok:
    case ConverterT::noconv: {
      return {&dst[0], dst_next};
    }
    default: {
      SDATA_ASSERT(0, "String conversion failed");
    }
  }
}

}  // namespace sdata::string

#endif
