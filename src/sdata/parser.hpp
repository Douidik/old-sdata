#ifndef SDATA_PARSER_HPP
#define SDATA_PARSER_HPP

#include <charconv>
#include <memory>
#include <unordered_set>

#include "node.hpp"
#include "scanner.hpp"

namespace sdata {

template <typename char_t>
class ParserException : public Exception {
 public:
  ParserException(std::string_view description, const Token<char_t> &token)
      : Exception(description), m_token(token) {}

  inline std::string header() const override {
    std::string snippet = m_token.source_location.snippet();
    size_t line = m_token.source_location.line();

    return format("\n\t%i | %s\n", line, snippet.data());
  }

  inline std::string_view name() const override {
    return type_to_string<ParserException<char_t>>();
  }

 private:
  Token<char_t> m_token;
};

template <typename char_t>
class Parser {
  using string_view_t = std::basic_string_view<char_t>;

 public:
  explicit Parser(string_view_t source, std::unordered_set<std::string_view> context = {})
      : m_scanner(source), m_context(context) {}

  std::shared_ptr<Node> parse(std::shared_ptr<Node> owner = nullptr) {
    using Token = Token<char_t>;

    Token id = parse_token(Token::IDENTIFIER | Token::NAMESPACE);
    auto node = std::make_shared<Node>(std::string {id.expression.begin(), id.expression.end()},
                                       nullptr, m_context);

    unsigned int expected_assignment =  // Namespaces can't directly contain data
        id.category != Token::NAMESPACE ? (Token::EQUALS | Token::BEG_SEQ) : (Token::BEG_SEQ);

    Token assignment = parse_token(expected_assignment);

    if (assignment.category == Token::BEG_SEQ) {
      do {
        auto member = parse(node);
        node->add_member(member);
      } while (parse_token(Token::SEPARATOR | Token::END_SEQ).category != Token::END_SEQ);
    }

    if (assignment.category == Token::EQUALS) {
      parse_data(node);
    }

    return node;
  }

 private:
  void parse_data(std::shared_ptr<Node> node) {
    using Token = Token<char_t>;

    unsigned int expectations =
        Token::FLOAT | Token::INTEGER | Token::BOOLEAN | Token::STRING | Token::CHAR;

    auto token = parse_token(expectations);

    switch (token.category) {
      case Token::FLOAT: {
        *node = std::stof(std::string {token.expression.begin(), token.expression.end()});
      } break;
      case Token::INTEGER: {
        *node = std::stoi(std::string {token.expression.begin(), token.expression.end()});
      } break;
      case Token::BOOLEAN: {
        if (token.expression == "true") *node = true;
        if (token.expression == "false") *node = false;
      } break;
      case Token::STRING: {
        *node = std::basic_string<char_t> {token.expression.begin(), token.expression.end()};
      } break;
      case Token::CHAR: {
        *node = token.expression.at(1);
      } break;

      default: break;
    }
  }

  Token<char_t> parse_token(unsigned int expectations) {
    using Token = Token<char_t>;
    Token token = m_scanner.tokenize();

    if (bool expected = token.category & expectations; !expected) {
      std::ostringstream expectations_stream {};

      for (unsigned int i = 0; i < Token::CATEGORY_COUNT; i++) {
        auto category = (typename Token::Category)bit(i);
        if (expectations & category)
          expectations_stream << "'" << Token::category_to_string(category) << "',";
      }

      auto expectations_str = expectations_stream.str();
      auto category_str = Token::category_to_string(token.category);

      throw ParserException<char_t> {
          format("Expected expression of one of the following type(s) [%s] got '%s'",
                 expectations_str.data(), category_str.data()),
          token,
      };
    }

    return token;
  }

  Scanner<char_t> m_scanner;
  std::unordered_set<string_view_t> m_context;
};

}  // namespace sdata

#endif
