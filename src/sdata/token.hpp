#ifndef SDATA_TOKEN_HPP
#define SDATA_TOKEN_HPP

#include <cstdint>
#include <string_view>

#include "misc/bitwise.hpp"
#include "misc/source_location.hpp"
#include "sdata/regex/regex.hpp"

namespace sdata {

template <typename char_t>
struct Token {
  using string_view_t = std::basic_string_view<char_t>;

  enum Category : unsigned int {
    NONE = 0,

    IDENTIFIER = bit(0),
    EQUALS = bit(1),
    SEPARATOR = bit(2),
    NAMESPACE = bit(3),

    FLOAT = bit(5),
    INTEGER = bit(6),
    BOOLEAN = bit(7),
    STRING = bit(8),
    CHAR = bit(9),

    BEG_SEQ = bit(10),
    END_SEQ = bit(11),

    EMPTY = bit(12),

    CATEGORY_COUNT = 12,
  };

  constexpr static std::string_view category_to_string(Category category) {
    switch (category) {
      case IDENTIFIER: return "identifier";
      case EQUALS: return "equals";
      case SEPARATOR: return "separator";
      case NAMESPACE: return "namespace";
      case FLOAT: return "float";
      case INTEGER: return "integer";
      case BOOLEAN: return "boolean";
      case STRING: return "string";
      case CHAR: return "character";
      case BEG_SEQ: return "sequence-begin";
      case END_SEQ: return "sequence-ending";
      case EMPTY: return "empty";
      case NONE: return "none";
      case CATEGORY_COUNT: return "category-count";
    }
  }

  inline static const std::unordered_map<Category, Regex<char>> patterns {
      {SEPARATOR, {"','"}},
      {END_SEQ, {"'}'"}},
      {BEG_SEQ, {"'{'"}},
      {EQUALS, {"':'"}},
      {IDENTIFIER, {"a {a|d}*"}},
      {NAMESPACE, {"'@' a {a|d}*"}},
      {INTEGER, {"{'-'|'+'}? d+"}},
      {FLOAT, {"{'-'|'+'}? d+ '.' d+ 'f'?"}},
      {BOOLEAN, {"'true'|'false'"}},
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
