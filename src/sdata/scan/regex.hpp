#ifndef SDATA_REGEX_HPP
#define SDATA_REGEX_HPP

#include <iostream>

#include "regex_parser.hpp"

namespace sdata {

template <typename char_t>
class Regex {
  using string_view_t = std::basic_string_view<char_t>;

 public:
  Regex(string_view_t pattern)
      : m_pattern(pattern), m_automata{RegexParser<char_t>::parse(pattern)} {
  }

  template <typename Iterator>
  bool match(Iterator begin, const Iterator end) const {
    auto [accepted, _] = m_automata.run(begin, end);
    return accepted;
  }

  template <typename Iterator>
  bool fullmatch(Iterator begin, const Iterator end) const {
    auto [accepted, output] = m_automata.run(begin, end);
    return accepted && output == end;
  }

  inline string_view_t pattern() const {
    return m_pattern;
  }

  inline const RegexAutomata<char_t> &automata() const {
    return m_automata;
  }

 private:
  RegexAutomata<char_t> m_automata;
  const string_view_t m_pattern;
};

}  // namespace sdata

#endif
