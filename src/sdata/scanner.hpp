#ifndef SDATA_SCANNER_HPP
#define SDATA_SCANNER_HPP

#include "misc/code_exception.hpp"

namespace sdata {

template <typename CharT>
class ScannerException : public CodeException<CharT> {
 public:
  ScannerException(std::string_view description, const Token<CharT> &token)
      : CodeException<CharT>("sdata::ScannerException", description, token) {}
};

template <typename CharT>
class Scanner {
  using StringViewT = std::basic_string_view<CharT>;

 public:
  explicit Scanner(StringViewT source) : m_source(source), m_iterator(source.begin()) {}

  inline bool eof() const {
    return m_iterator == m_source.end();
  }

  Token<CharT> tokenize() {
    Token<CharT> token{{}, TOKEN_NONE, {m_source, m_iterator}};

    if (eof()) {
      token.category = TOKEN_EOF;
      return token;
    }

    for (auto &[category, regex] : s_token_patterns) {
      if (RegexMatch match = regex.match(m_iterator, m_source.end())) {
        token.expression = {m_iterator, m_iterator += match.length};
        token.category = category;
        break;
      }
    }

    if (token.category == TOKEN_NONE) {
      throw ScannerException<CharT>("Unrecognized token", token);
    }

    // Empty tokens are skipped
    return token.category != TOKEN_EMPTY ? token : tokenize();
  }

 private:
  StringViewT m_source;
  typename StringViewT::iterator m_iterator;
};

}  // namespace sdata

#endif
