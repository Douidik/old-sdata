#ifndef SDATA_PARSER_HPP
#define SDATA_PARSER_HPP

#include <charconv>
#include <memory>

#include "node/context.hpp"
#include "node/node.hpp"
#include "scan/scanner.hpp"

namespace sdata {

template <typename char_t>
class ParserException : public Exception {
  using string_view_t = std::basic_string_view<char_t>;

 public:
  ParserException(std::string_view description, const Token<char_t> &token)
      : Exception(description), m_token(token) {}

  inline std::string header() const override {
    std::string snippet = m_token.source_location.snippet();
    size_t line = m_token.source_location.line();

    return format("\n\t%i | %s\n", line, snippet);
  }

  inline virtual std::string_view name() const {
    return type_to_string<ParserException<char_t>>();
  }

 private:
  const Token<char_t> &m_token;
};

template <typename char_t>
class Parser {
  using string_view_t = std::basic_string_view<char_t>;

 public:
  Parser(string_view_t source, std::initializer_list<string_view_t> context = {})
      : m_scanner(source) {}

  void parse(std::shared_ptr<Node> owner) {
    using Token = Token<char_t>;

    Token token = read_token(Token::IDENTIFIER | Token::NAMESPACE);
    auto node = std::make_shared<>(token, nullptr);

    switch (auto assignment = read_token(Token::EQUALS | Token::BEG_SEQ)) {
      case Token::EQUALS: {
        parse_data(node);
      } break;

      case Token::BEG_SEQ: {
        parse(node);
      } break;

      default: {
      } break;
    }

    if (owner) {
      owner->add_member(node);
    }

    if (read_token(Token::SEPARATOR | Token::END_SEQ).category == Token::SEPARATOR) {
      parse(owner);
    }
  }

 private:
  void parse_data(std::shared_ptr<Node> node) {
    using Token = Token<char_t>;

    auto token =
        read_token(Token::FLOAT | Token::INTEGER | Token::BOOLEAN | Token::STRING | Token::CHAR);

    switch (token.category) {
      case Token::FLOAT: {
        node = parse_number<float>(token);
      } break;
      case Token::INTEGER: {
        node = parse_number<int>(token);
      } break;
      case Token::BOOLEAN: {
        node = token.view == "true" ? true : false;
      } break;
      case Token::STRING: {
        node = {token.view.begin(), token.view.end()};
      } break;
      case Token::CHAR: {
        node = token.view.at(0);
      } break;

      default: {
      } break;
    }
  }

  template <typename T>
  T parse_number(Token<char_t> token) {
    std::string buffer = {token.expression.begin(), token.expression.end()};
    T number {};

    auto [_, status] = std::from_chars(buffer.begin(), buffer.begin() + buffer.size(), number);

    if (status == std::errc::result_out_of_range) {
      if constexpr (std::is_same_v<T, float>)
        throw ParserException<char_t>(error::PARSER_FLOAT_OVERFLOW, token);
      if constexpr (std::is_same_v<T, int>)
        throw ParserException<char_t>(error::PARSER_INTEGER_OVERFLOW, token);
    }

    return number;
  }

  Token<char_t> read_token(unsigned int expected) {
    auto token = m_scanner.tokenize();

    if (!(token.category & expected)) {
      if (token.category != Token<char_t>::NONE)
        throw ParserException<char_t>(error::PARSER_UNEXPECTED_TOKEN, token);
      else
        throw ParserException<char_t>(error::PARSER_EXPECTED_TOKEN, token);
    }

    return token;
  }

  Scanner<char_t> m_scanner;
  std::unordered_set<string_view_t> m_context;
};

}  // namespace sdata

#endif
