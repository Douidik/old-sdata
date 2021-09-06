#ifndef SDATA_TOKEN_HPP
#define SDATA_TOKEN_HPP

#include <string_view>

#include "regex.hpp"

namespace sdata {

template <typename char_t>
struct Token {
  using string_view_t = std::basic_string_view<char_t>;

  enum Category : size_t {
    IDENTIFIER,
    ASSIGNMENT,
    SEPARATOR,
    NAMESPACE,
    COMMENT,

    FLOAT,
    INTEGER,
    BOOLEAN,
    STRING,
    CHARACTER,
    ARRAY,

    BEG_SEQ,
    END_SEQ,

    EMPTY,
    UNKNOWN,
  };

  inline static const std::unordered_map<Category, Regex<char>> patterns{
      {SEPARATOR, {"','"}},
      {END_SEQ, {"'}'"}},
      {BEG_SEQ, {"'{'"}},
      {ASSIGNMENT, {"'='"}},
      {IDENTIFIER, {"a {a|d}*"}},
      {NAMESPACE, {"'@' a {a|d}*"}},
      {INTEGER, {"{'-'|'+'}? d+"}},
      {FLOAT, {"{'-'|'+'}? d+ '.' d+ 'f'?"}},
      {BOOLEAN, {"'true'|'false'"}},
      {COMMENT, {"'//' {'\n'|'\\0'}$"}},
      {CHARACTER, {"q^q"}},
      {STRING, {"QQ$"}},
      {EMPTY, {"_+"}},
  };

  string_view_t data;
  Category category;
};

}  // namespace sdata

#endif
