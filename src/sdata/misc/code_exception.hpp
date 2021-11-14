#ifndef SDATA_CODE_EXCEPTION_HPP
#define SDATA_CODE_EXCEPTION_HPP

#include <exception>
#include "misc/fmt.hpp"
#include "token.hpp"

namespace sdata {

template <typename CharT>
class CodeException : public std::exception {
  constexpr static std::string_view PATTERN =
      "[% raised]: %\n"
      "with {\n"
      "Token<%>: '%' in:\n"
      "  % | %\n"
      "}";

 public:
  CodeException(std::string_view name, std::string_view description, const Token<CharT> &token)
      : m_token(token),
        m_buffer(fmt(PATTERN,
                     name,
                     description,
                     token_category_name(token.category),
                     std::string{token.expression.begin(), token.expression.end()},
                     token.source_location.line,
                     std::string{
                         token.source_location.snippet().begin(),
                         token.source_location.snippet().end(),
                     })) {}

  inline virtual const char *what() const noexcept {
    return m_buffer.data();
  }

  inline const Token<CharT> &token() const noexcept {
    return m_token;
  }

 private:
  std::string m_buffer;
  const Token<CharT> &m_token;
};

}  // namespace sdata

#endif
