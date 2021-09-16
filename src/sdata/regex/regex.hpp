#ifndef SDATA_REGEX_HPP
#define SDATA_REGEX_HPP

#include "regex_parser.hpp"

namespace sdata {

template <typename char_t>
class Regex {
  using string_view_t = std::basic_string_view<char_t>;

 public:
  Regex(string_view_t pattern) : m_pattern(pattern) {
    RegexParser<char_t> parser(m_pattern);
    m_automata = parser.parse();
  }

  template <typename T>
  bool match(T expression) const {
    auto [matched, _] = m_automata.run(std::begin(expression), std::end(expression));
    return matched;
  }

  template <typename iterator_t>
  bool match(iterator_t begin, const iterator_t end) const {
    auto [matched, _] = m_automata.run(begin, end);
    return matched;
  }

  template <typename iterator_t>
  bool fullmatch(iterator_t begin, const iterator_t end) const {
    auto [matched, output] = m_automata.run(begin, end);
    return matched && output == end;
  }

  inline string_view_t pattern() const {
    return m_pattern;
  }

  inline const RegexAutomata<char_t> &automata() const {
    return m_automata;
  }

 private:
  RegexAutomata<char_t> m_automata;
  string_view_t m_pattern;
};

}  // namespace sdata

#endif
