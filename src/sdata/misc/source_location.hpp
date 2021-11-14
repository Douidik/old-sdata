#ifndef SDATA_SOURCE_LOCATION_HPP
#define SDATA_SOURCE_LOCATION_HPP

#include <algorithm>
#include <string_view>

namespace sdata {

template <typename CharT>
struct SourceLocation {
  using StringViewT = std::basic_string_view<CharT>;

 public:
  constexpr SourceLocation(StringViewT source, typename StringViewT::const_iterator iterator)
      : source(source),
        index(std::distance(source.cbegin(), iterator)),
        line(std::count(source.cbegin(), iterator, CharT{'\n'})) {}

  constexpr SourceLocation() : source{}, index((size_t)-1), line((size_t)-1) {}

  constexpr inline StringViewT snippet() const {
    return {
        source.begin() + source.rfind(CharT{'\n'}, index) + 1,
        source.begin() + source.find(CharT{'\n'}, index),
    };
  }

  StringViewT source;
  size_t index, line;
};

};  // namespace sdata

#endif
