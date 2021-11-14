#ifndef SDATA_TOKEN_HPP
#define SDATA_TOKEN_HPP

#include <unordered_map>
#include "misc/source_location.hpp"
#include "regex/regex.hpp"
#include "misc/string.hpp"

namespace sdata {

enum TokenCategory : unsigned int {
  TOKEN_NONE = 0,

  TOKEN_ID = 1 << 0,
  TOKEN_ASSIGN = 1 << 1,
  TOKEN_SEPARATOR = 1 << 2,

  TOKEN_FLOAT = 1 << 3,
  TOKEN_INT = 1 << 4,
  TOKEN_BOOL = 1 << 5,
  TOKEN_STRING = 1 << 6,
  TOKEN_CHAR = 1 << 7,
  TOKEN_DATA = TOKEN_FLOAT | TOKEN_INT | TOKEN_BOOL | TOKEN_STRING | TOKEN_CHAR,

  TOKEN_BEG_SEQ = 1 << 8,
  TOKEN_END_SEQ = 1 << 9,

  TOKEN_EMPTY = 1 << 10,
  TOKEN_EOF = 1 << 11,

  TOKEN_CATEGORY_MAX = 1 << 12,
};

constexpr static std::string_view token_category_name(TokenCategory category) {
  switch (category) {
    case TOKEN_ID: return "id";
    case TOKEN_ASSIGN: return "assign";
    case TOKEN_SEPARATOR: return "separator";
    case TOKEN_FLOAT: return "float";
    case TOKEN_INT: return "int";
    case TOKEN_BOOL: return "bool";
    case TOKEN_STRING: return "string";
    case TOKEN_CHAR: return "character";
    case TOKEN_BEG_SEQ: return "sequence-begin";
    case TOKEN_END_SEQ: return "sequence-ending";
    case TOKEN_EMPTY: return "empty";
    default: return "";
  }
}

inline std::ostream &operator<<(std::ostream &os, TokenCategory category) {
  return os << token_category_name(category);
}

inline static const std::unordered_map<TokenCategory, Regex> s_token_patterns = {
    {TOKEN_SEPARATOR, {" ',' "}},
    {TOKEN_END_SEQ, {" '}' "}},
    {TOKEN_BEG_SEQ, {" '{' "}},
    {TOKEN_ASSIGN, {" ':' "}},
    {TOKEN_ID, {"{a|'_'} {a|n|'_'}*"}},
    {TOKEN_INT, {"{'-'|'+'}? n+"}},
    {TOKEN_FLOAT, {"{'-'|'+'}? n+ '.' n+ 'f'?"}},
    {TOKEN_BOOL, {"'true'|'false'"}},
    {TOKEN_CHAR, {"q^q"}},
    {TOKEN_STRING, {"Q~Q"}},
    {TOKEN_EMPTY, {"_+"}},
};

template <typename CharT>
struct Token {
  std::basic_string_view<CharT> expression;
  TokenCategory category;
  SourceLocation<CharT> source_location;
};

template <typename T, typename U>
bool operator==(Token<T> a, Token<U> b) {
  return a.expression == string::convert<U, T>(b.expression) && a.category == b.category;
}

}  // namespace sdata

#endif
