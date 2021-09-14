#ifndef SDATA_SOURCE_LOCATION_HPP
#define SDATA_SOURCE_LOCATION_HPP

#include <algorithm>
#include <string>
#include <string_view>

namespace sdata {

template <typename char_t>
class SourceLocation {
  using string_view_t = std::basic_string_view<char_t>;

  constexpr SourceLocation(string_view_t source,
                           typename string_view_t::const_iterator iterator)
      : m_source(source),
        m_line {std::count(m_source.cbegin(), iterator, char_t('\n'))},
        m_index {std::distance(m_source.begin(), iterator)} {}

  std::string snippet() const {
    size_t begin = m_source.rfind(char_t('\n'), m_index);
    size_t end = m_source.find(char_t('\n'), m_index);
    auto snippet = m_source.substr(begin, end);

    return {snippet.begin(), snippet.end()};
  }

  inline size_t index() const {
    return m_index;
  }
  inline size_t line() const {
    return m_line;
  }
  inline size_t source() const {
    return m_source;
  }

 private:
  const string_view_t m_source;
  const size_t m_index, m_line;
};

}  // namespace sdata

#endif
