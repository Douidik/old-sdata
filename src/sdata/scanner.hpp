#ifndef SDATA_SCANNER_HPP
#define SDATA_SCANNER_HPP

#include <deque>
#include <iterator>

#include "misc/source_location.hpp"
#include "token.hpp"

namespace sdata {

template <typename char_t>
class ScannerException : public Exception {
  using string_view_t = std::basic_string_view<char_t>;

 public:
  ScannerException(std::string_view description, const Token<char_t> &token)
      : Exception(description), m_token(token) {}

  inline std::string header() const override {
    std::string snippet = m_token.source_location.snippet();
    size_t line = m_token.source_location.line();

    return format("\n\t%i | %s\n", line, snippet.data());
  }

  inline std::string_view name() const override{
    return type_to_string<ScannerException<char_t>>();
  }

 private:
  Token<char_t> m_token;
};

template <typename char_t>
class Scanner {
  using string_view_t = std::basic_string_view<char_t>;

 public:
  explicit Scanner(string_view_t source) : m_source(source), m_iterator(m_source.begin()) {}

  Token<char_t> tokenize() {
    Token<char_t> token {{m_source, m_iterator}, {}, Token<char_t>::NONE};

    if (m_iterator == m_source.end()) {
      return token;
    }

    for (auto &[category, pattern] : Token<char_t>::patterns) {
      if (auto [matched, end] = pattern.automata().run(m_iterator, m_source.end()); matched) {
        token.category = category;
        token.expression = {m_iterator, (m_iterator = end)};
        break;
      }
    }

    if (token.category == Token<char_t>::NONE)  // token doesn't match any pattern
      throw ScannerException<char_t>(error::SCANNER_UNRECOGNIZED_TOKEN, token);

    return token.category != Token<char_t>::EMPTY ? token : tokenize();
  }

 private:
  string_view_t m_source;
  typename string_view_t::iterator m_iterator;
};

}  // namespace sdata

#endif
