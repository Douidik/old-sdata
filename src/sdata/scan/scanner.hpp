#ifndef SDATA_SCANNER_HPP
#define SDATA_SCANNER_HPP

#include <deque>
#include <iterator>

#include "token.hpp"

namespace sdata {

template <typename char_t>
class ScannerException : public Exception {
  using string_view_t = std::basic_string_view<char_t>;

 public:
  ScannerException(std::string_view description, string_view_t source,
                   typename string_view_t::iterator iterator)
      : Exception(description),
        m_source(source),
        m_line(std::count(source.begin(), iterator, '\n')),
        m_index(std::distance(source.begin(), iterator)),
        m_snippet(create_snippet(source, m_index)) {
  }

  inline std::string header() const override {
    return format("\n\t%i | %s\n", m_line, m_snippet.data());
  }

  inline string_view_t source() const {
    return m_source;
  }

  inline size_t line() const {
    return m_line;
  }

  inline size_t index() const {
    return m_index;
  }

 private:
  inline static std::string create_snippet(string_view_t source, size_t index) {
    size_t begin = source.rfind(char_t('\n'), index) + 1;
    size_t end = source.find(char_t('\n'), index);
    string_view_t snippet = source.substr(begin, end);
    return {snippet.begin(), snippet.end()};
  }

  string_view_t m_source;
  std::string m_snippet;
  size_t m_line, m_index;
};

template <typename char_t>
class Scanner {
  using string_view_t = std::basic_string_view<char_t>;

 public:
  explicit Scanner(string_view_t source) : m_source(source), m_iterator(m_source.begin()) {
  }

  inline operator bool() const {
    return m_iterator != m_source.end();
  }

  Token<char_t> tokenize() {
    for (const auto &[category, pattern] : Token<char_t>::patterns) {
      auto [matched, end] = pattern.automata().run(m_iterator, m_source.end());
      if (matched) {
        return {
            {m_iterator, m_iterator = end},
            category,
        };
      }
    }

    throw ScannerException<char_t>(error::SCANNER_UNRECOGNIZED_TOKEN, m_source, m_iterator);
    return {{}, Token<char_t>::UNKNOWN};
  }

 private:
  string_view_t m_source;
  typename string_view_t::iterator m_iterator;
};

}  // namespace sdata

#endif
