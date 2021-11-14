#ifndef SDATA_TRIM_HPP
#define SDATA_TRIM_HPP

#include <string_view>

namespace sdata {

template <typename CharT>
constexpr auto trim(std::basic_string_view<CharT> s, CharT c) {
  s.remove_prefix(std::min(s.find_first_not_of(c), s.size()));
  s.remove_suffix(std::min(s.size() - s.find_last_not_of(c) - 1, s.size()));

  return s;
}

}  // namespace sdata

#endif
