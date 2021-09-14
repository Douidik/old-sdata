#ifndef SDATA_TOKEN_HPP
#define SDATA_TOKEN_HPP

#include <cstdint>
#include <string_view>

#include "misc/bitwise.hpp"
#include "misc/source_location.hpp"
#include "regex.hpp"

namespace sdata {

template <typename char_t>
struct Token {
  using string_view_t = std::basic_string_view<char_t>;

  enum Category : unsigned int {
    NONE = 0,

    IDENTIFIER = BIT(0),
    EQUALS = BIT(1),
    SEPARATOR = BIT(2),
    NAMESPACE = BIT(3),
    COMMENT = BIT(4),

    FLOAT = BIT(5),
    INTEGER = BIT(6),
    BOOLEAN = BIT(7),
    STRING = BIT(8),
    CHAR = BIT(9),

    BEG_SEQ = BIT(10),
    END_SEQ = BIT(11),

    EMPTY = BIT(12),
  };

  template <Category category>
  constexpr std::string_view category_to_string() {
    switch (category) {
      case IDENTIFIER: return "identifier";
      case EQUALS: return "equals";
      case SEPARATOR: return "separator";
      case NAMESPACE: return "namespace";
      case COMMENT: return "comment";
      case FLOAT: return "float";
      case INTEGER: return "integer";
      case BOOLEAN: return "boolean";
      case STRING: return "string";
      case CHAR: return "character";
      case BEG_SEQ: return "sequence-begin";
      case END_SEQ: return "sequence-ending";
      case EMPTY: return "empty";
      case NONE: return "none";
    }
  }

  inline static const std::unordered_map<Category, Regex<char>> patterns {
      {SEPARATOR, {"','"}},
      {END_SEQ, {"'}'"}},
      {BEG_SEQ, {"'{'"}},
      {EQUALS, {"'='"}},
      {IDENTIFIER, {"a {a|d}*"}},
      {NAMESPACE, {"'@' a {a|d}*"}},
      {INTEGER, {"{'-'|'+'}? d+"}},
      {FLOAT, {"{'-'|'+'}? d+ '.' d+ 'f'?"}},
      {BOOLEAN, {"'true'|'false'"}},
      {COMMENT, {"'//' {'\n'|'\\0'}$"}},
      {CHAR, {"q^q"}},
      {STRING, {"Q Q$"}},
      {EMPTY, {"_+"}},
  };

  SourceLocation<char_t> source_location;
  string_view_t expression;
  Category category;
};

}  // namespace sdata

#endif
