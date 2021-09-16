#ifndef SDATA_REGEX_PARSER_HPP
#define SDATA_REGEX_PARSER_HPP

#include <deque>

#include "misc/errors.hpp"
#include "misc/exception.hpp"
#include "misc/type_to_string.hpp"
#include "regex_automata.hpp"
#include "regex_category.hpp"

namespace sdata {

template <typename char_t>
class RegexParserException : public Exception {
  using string_view_t = std::basic_string_view<char_t>;

 public:
  RegexParserException(std::string_view description,
                       string_view_t pattern,
                       typename string_view_t::iterator token)
      : Exception(description),
        m_pattern(pattern.begin(), pattern.end()),
        m_token(static_cast<char>(*token)),
        m_index(std::distance(pattern.begin(), token)) {}

  std::string header() const override {
    return format("index: %i, pattern: '%s', token: '%c'", m_index, m_pattern.data(), m_token);
  }

  std::string_view name() const override {
    return type_to_string<RegexParserException>();
  }

 private:
  std::string m_pattern;
  char m_token;
  size_t m_index;
};

template <typename char_t>
class RegexParser {
  using string_view_t = std::basic_string_view<char_t>;

 public:
  explicit RegexParser(string_view_t pattern) : m_pattern(pattern) {}

  RegexAutomata<char_t> parse() {
    for (m_token = m_pattern.begin(); m_token != m_pattern.end(); m_token++) {
      parse_token();
    }

    return m_stack.size() > 0 ? compile_sequences() : RegexAutomata<char_t> {};
  }

 private:
  RegexAutomata<char_t> &compile_sequences() {
    size_t ancestor_root_id = 0;
    auto &automata = m_stack.front();

    for (auto itr = m_stack.begin() + 1; itr != m_stack.end(); itr++) {
      // merge sequences inside the automata
      ancestor_root_id = automata.merge(*itr, automata.node_leaves(ancestor_root_id));
    }

    return automata;
  }

  void parse_token() {
    if (std::isblank(*m_token)) {  // skip blank tokens
      m_token++;
      return parse_token();
    }

    switch (*m_token) {
      case REGEX_BLANK:
      case REGEX_ALPHA:
      case REGEX_OPERATOR:
      case REGEX_DIGIT:
      case REGEX_QUOTE:
      case REGEX_APOSTROPHE: return parse_character_class();
      case REGEX_ANY: return parse_any();
      case REGEX_LITERAL: return parse_literal();
      case REGEX_BEG_SUBSEQ: return parse_subsequence();
      case REGEX_PLUS: return parse_plus();
      case REGEX_QUEST: return parse_quest();
      case REGEX_KLEENE: return parse_kleene();
      case REGEX_UNTIL: return parse_until();
      case REGEX_ALTERNATIVE: return parse_alternative();

      case REGEX_END_SUBSEQ:
        throw RegexParserException<char_t>(error::REGEX_UNEXPECTED_SUBSEQ_END, m_pattern, m_token);

      default:
        throw RegexParserException<char_t>(error::REGEX_UNRECOGNIZED_TOKEN, m_pattern, m_token);
    }
  }

  void parse_character_class() {
    //      -> first_character_alternative
    // root -> ...
    //      -> last_character_alternative

    static const std::unordered_map<RegexCategory, std::string_view> s_character_map {
        {REGEX_BLANK, "\n\t\v\b\f "},
        {REGEX_ALPHA, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_"},
        {REGEX_OPERATOR, "!#$%&()*+,-./:;<=>?@[\\]^`{|}~"},
        {REGEX_DIGIT, "0123456789"},
        {REGEX_QUOTE, "\""},
        {REGEX_APOSTROPHE, "'"},
    };

    auto &sequence = m_stack.emplace_back();
    size_t root_id = sequence.node_create(RegexAutomata<char_t>::EPSILON, {}, {});
    std::string_view character_map = s_character_map.at((RegexCategory)*m_token);

    for (const char &character : character_map)
      sequence.node_create(static_cast<char_t>(character), {root_id}, {});
  }

  void parse_any() {
    auto &sequence = m_stack.emplace_back();
    sequence.node_create(RegexAutomata<char_t>::ANY, {}, {});
  }

  void parse_literal() {
    // first_character -> ... -> last_character

    auto begin = m_token + 1, end = std::find(begin, m_pattern.end(), REGEX_LITERAL);

    if (end == m_pattern.end())
      throw RegexParserException<char_t>(error::REGEX_UNTERMINATED_LITERAL, m_pattern, m_token);

    auto &sequence = m_stack.emplace_back();
    string_view_t literal = {begin, end};

    for (const char_t &c : literal) {
      size_t id = sequence.node_create(c, {}, {});
      // Concatenate literal nodes
      if (id > 0) sequence.node_connect(id - 1, id);
    }

    m_token = end;
  }

  void parse_subsequence() {
    auto [begin, end] = [pattern = m_pattern, begin = m_token + 1] {
      size_t depth = 1;

      // find the matching brace
      auto end = std::find_if(begin, pattern.end(), [&depth](const char_t &c) {
        if (c == REGEX_BEG_SUBSEQ) depth++;
        if (c == REGEX_END_SUBSEQ && --depth <= 0) return true;
        return false;
      });

      return std::make_pair(begin, end);
    }();

    if (end == m_pattern.end())
      throw RegexParserException<char_t>(error::REGEX_UNTERMINATED_SUBSEQ, m_pattern, m_token);

    auto &subsequence = m_stack.emplace_back();

    try {
      RegexParser<char_t> parser({begin, end});
      subsequence = parser.parse();
    } catch (const RegexParserException<char_t> &exception) {
      // Throws the subsequence exception with the completed pattern
      throw RegexParserException<char_t>(exception.description(), m_pattern, m_token);
    }

    m_token = end;
  }

  void parse_quest() {
    // root -> operand -> next
    //      -> epsilon -> next
    if (m_stack.empty() || m_stack.back().empty())
      throw RegexParserException<char_t>(error::REGEX_MISSING_QUANTIFIABLE, m_pattern, m_token);

    auto operand = m_stack.back();
    m_stack.pop_back();

    auto &sequence = m_stack.emplace_back();
    size_t root_id = sequence.node_create(RegexAutomata<char_t>::EPSILON, {}, {});
    sequence.merge(operand, {root_id});
    size_t epsilon_id = sequence.node_create(RegexAutomata<char_t>::EPSILON, {root_id}, {});
  }

  void parse_kleene() {
    // root -> operand -> root
    //      -> epsilon -> next
    if (m_stack.empty() || m_stack.back().empty())
      throw RegexParserException<char_t>(error::REGEX_MISSING_QUANTIFIABLE, m_pattern, m_token);

    auto operand = m_stack.back();
    m_stack.pop_back();

    auto &sequence = m_stack.emplace_back();
    size_t root_id = sequence.node_create(RegexAutomata<char_t>::EPSILON, {}, {});
    size_t op_root_id = sequence.merge(operand, {root_id});
    size_t epsilon_id = sequence.node_create(RegexAutomata<char_t>::EPSILON, {root_id}, {});

    const auto op_leaves = sequence.node_leaves(op_root_id);
    for (size_t op_leaf : op_leaves) {
      sequence.node_connect(op_leaf, root_id);
    }
  }

  void parse_plus() {
    // operand -> epsilon -> operand
    //                    -> next
    if (m_stack.empty() || m_stack.back().empty())
      throw RegexParserException<char_t>(error::REGEX_MISSING_QUANTIFIABLE, m_pattern, m_token);

    auto &operand = m_stack.back();
    operand.node_create(RegexAutomata<char_t>::EPSILON, operand.node_leaves(), {0});
  }

  void parse_until() {
    // root -> operand -> next
    //      -> any     -> root
    if (m_stack.empty() || m_stack.back().empty())
      throw RegexParserException<char_t>(error::REGEX_MISSING_QUANTIFIABLE, m_pattern, m_token);

    auto operand = m_stack.back();
    m_stack.pop_back();

    auto &sequence = m_stack.emplace_back();
    size_t root_id = sequence.node_create(RegexAutomata<char_t>::EPSILON, {}, {});
    size_t op_root_id = sequence.merge(operand, {root_id});
    size_t any_id = sequence.node_create(RegexAutomata<char_t>::ANY, {root_id}, {root_id});
  }

  void parse_alternative() {
    // root -> first_alternative
    //      -> second_alternative
    if (++m_token != m_pattern.end()) {
      parse_token();
    } else {
      throw RegexParserException<char_t>(error::REGEX_MISSING_ALTERNATIVE, m_pattern, m_token - 1);
    }

    RegexAutomata<char_t> sequence;
    size_t root_id = sequence.node_create(RegexAutomata<char_t>::EPSILON, {}, {});

    for (size_t i = 0; i < 2; i++) {
      if (m_stack.empty())
        throw RegexParserException<char_t>(error::REGEX_MISSING_ALTERNATIVE, m_pattern, m_token);

      const auto &alternative = m_stack.back();
      sequence.merge(alternative, {root_id});
      m_stack.pop_back();
    }

    m_stack.push_back(sequence);
  }

  string_view_t m_pattern;
  typename string_view_t::iterator m_token;
  std::deque<RegexAutomata<char_t>> m_stack;
};

}  // namespace sdata

#endif
