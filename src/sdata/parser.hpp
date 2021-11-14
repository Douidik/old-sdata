#ifndef SDATA_PARSER_HPP
#define SDATA_PARSER_HPP

#include <sstream>
#include "misc/fmt.hpp"
#include "misc/trim.hpp"
#include "node.hpp"
#include "scanner.hpp"

namespace sdata {

template <typename CharT>
class ParserException : public CodeException<CharT> {
 public:
  ParserException(std::string_view description, const Token<CharT> &token)
      : CodeException<CharT>("sdata::ParserException", description, token) {}
};

template <typename CharT>
class Parser {
  using StringViewT = std::basic_string_view<CharT>;

 public:
  explicit Parser(StringViewT source) : m_scanner(source) {}

  std::shared_ptr<Node> parse() {
    std::shared_ptr<Node> node{};
    Token<CharT> token = parse_token(TOKEN_ID | TOKEN_BEG_SEQ | TOKEN_EOF), assignment{};

    switch (token.category) {
      case TOKEN_ID: {
        node = std::make_shared<Node>(token.expression, nullptr);
        assignment = parse_token(TOKEN_BEG_SEQ | TOKEN_ASSIGN);
      } break;
      case TOKEN_BEG_SEQ: {  // Anonymous node case
        node = std::make_shared<Node>("", nullptr);
        assignment = token;
      } break;
      default: {
        return {};
      };
    }

    if (assignment.category == TOKEN_BEG_SEQ) {
      parse_sequence(node);
    }
    if (assignment.category == TOKEN_ASSIGN) {
      parse_data(node);
    }

    return node;
  }

 private:
  void parse_sequence(std::shared_ptr<Node> node) {
    node->assign(Sequence{});

    do {
      if (auto member = parse()) {
        node->emplace(member);
      }
    } while (parse_token(TOKEN_SEPARATOR | TOKEN_END_SEQ).category != TOKEN_END_SEQ);
  }

  void parse_data(std::shared_ptr<Node> node) {
    auto data = parse_token(TOKEN_DATA);
    switch (data.category) {
      case TOKEN_FLOAT: {
        *node = std::stof(std::string{data.expression.begin(), data.expression.end()});
      } break;
      case TOKEN_INT: {
        *node = std::stoi(std::string{data.expression.begin(), data.expression.end()});
      } break;
      case TOKEN_BOOL: {
        if (data.expression == string::convert<char, CharT>("true")) *node = true;
        if (data.expression == string::convert<char, CharT>("false")) *node = false;
      } break;
      case TOKEN_STRING: {
        *node = std::basic_string<CharT>{trim<CharT>(data.expression, '"')};
      } break;
      case TOKEN_CHAR: {
        *node = CharT{data.expression.at(1)};
      } break;

      default: break;
    }
  }

  Token<CharT> parse_token(unsigned int expected) {
    auto token = m_scanner.tokenize();

    if (!(expected & token.category)) {
      parse_unexpected_token(token, expected);
    }

    return token;
  }

  void parse_unexpected_token(Token<CharT> token, unsigned int expected) {
    std::ostringstream expected_stream;

    for (unsigned int i = 0; i != TOKEN_CATEGORY_MAX; i <<= 1) {
      if (i & expected) expected_stream << "<" << token_category_name((TokenCategory)i) << ">,";
    }

    throw ParserException<CharT>{
        fmt<char>("Expected token of type(s) [%]", expected_stream.str()),
        token,
    };
  }

  Scanner<CharT> m_scanner;
};

}  // namespace sdata

#endif
