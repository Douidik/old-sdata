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

    return format("\n\t%i | %s\n", line, snippet);
  }

  inline virtual std::string_view name() const {
    return type_to_string<ScannerException<char_t>>();
  }

 private:
  const Token<char_t> &m_token;
};

template <typename char_t>
class Scanner {
  using string_view_t = std::basic_string_view<char_t>;

 public:
  explicit Scanner(string_view_t source) : m_source(source), m_iterator(m_source.begin()) {}

  inline bool eof() const {
    return m_iterator == m_source.end();
  }

  Token<char_t> tokenize() {
    using Token = Token<char_t>;

    Token token {{m_source, m_iterator}, "", Token::NONE};

    if (eof()) {
      return token;
    }

    for (auto &[category, pattern] : Token::patterns) {
      if (auto [matched, end] = pattern.automata().run(m_iterator, m_source.end()); matched) {
        token.category = category;
        token.expression = {m_iterator, (m_iterator = end)};
      }
    }

    if (token.category == Token::NONE) // token doesn't match any pattern
      throw ScannerException<char_t>(error::SCANNER_UNRECOGNIZED_TOKEN, token);

    return token.category != Token::EMPTY ? token : tokenize();
  }

 private:
  string_view_t m_source;
  typename string_view_t::iterator m_iterator;
};

}  // namespace sdata

#endif
