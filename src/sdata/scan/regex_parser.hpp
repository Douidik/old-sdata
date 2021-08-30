#ifndef REGEX_PARSER_HPP
#define REGEX_PARSER_HPP

#include <algorithm>
#include <deque>
#include <set>
#include <string_view>

#include "misc/exception.hpp"
#include "misc/exception_desc.hpp"
#include "regex_automata.hpp"

namespace sdata {

template <typename char_t>
class RegexParserException : public Exception {
 public:
  RegexParserException(std::string_view description, std::basic_string_view<char_t> pattern,
                       typename std::basic_string_view<char_t>::iterator token)
      : Exception(description),
        m_pattern{pattern.begin(), pattern.end()},
        m_token{token, token + 1},
        m_index{std::to_string(std::distance(pattern.begin(), token))} {
  }

  std::string header() const override {
    return Exception::format_parameters(
        {{"pattern", m_pattern}, {"token", m_token}, {"i", m_index}});
  }

  std::string_view name() const override {
    return type_to_string<RegexParserException<char_t>>();
  }

  inline void set_pattern(std::basic_string_view<char_t> pattern) {
    m_pattern = {pattern.begin(), pattern.end()};
  }

 private:
  std::string m_pattern, m_token, m_index;
};

template <typename char_t>
class RegexParser {
 public:
  static RegexAutomata<char_t> parse(std::basic_string_view<char_t> pattern, size_t depth = 0) {
    using RegexParserException = RegexParserException<char_t>;
    using RegexAutomata = RegexAutomata<char_t>;
    using string_view_t = std::basic_string_view<char_t>;

    std::deque<RegexAutomata> sequence_stack{};

    for (auto token = pattern.begin(); token != pattern.end(); token++) {
      switch (*token) {
        case 'e':
        case 'a':
        case 'o':
        case 'n':
        case 'Q':
        case 'q': {
          static const std::unordered_map<char, std::string_view> s_character_map{
              {'e', "\n\t\v\b\f "},
              {'a', "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"},
              {'o', "!#$%&()*+,-./:;<=>?@[\\]^_`{|}~"},
              {'n', "0123456789"},
              {'Q', "\""},
              {'q', "'"},
          };

          auto& sequence = sequence_stack.emplace_back();
          RegexNode root = sequence.node_create(RegexAutomata::EPSILON, {}, {});

          std::string_view character_map = s_character_map.at(*token);

          for (const char& c : character_map) {
            RegexNode character_node = sequence.node_create(static_cast<char_t>(c), {root}, {});
          }
        } break;

        case '_': {
          auto& sequence = sequence_stack.emplace_back();
          sequence.node_create(RegexAutomata::ANY, {}, {});
        } break;

        case '\'': {
          auto begin = token + 1, end = std::find(begin, pattern.end(), '\'');

          if (end != pattern.end()) {
            auto& sequence = sequence_stack.emplace_back();
            string_view_t literal = {begin, end};

            for (const char_t& c : literal) {
              RegexNode node = sequence.node_create(c, {}, {});
              // Concatenate nodes
              if (node > 0) {
                sequence.node_connect(node - 1, {node});
              }
            }

            token = end;
          } else {
            throw RegexParserException(exception_desc::REGEX_UNTERMINATED_LITERAL, pattern, token);
          }
        } break;

        case '{': {
          auto begin = token + 1, end = pattern.end();

          // find the matching curly-brace
          {
            size_t sequence_depth = 1;

            end = std::find_if(begin, pattern.end(), [&sequence_depth](char_t c) {
              if (c == '}' && --sequence_depth <= 0) {
                return true;
              } else if (c == '{') {
                sequence_depth++;
              }
              return false;
            });
          }

          if (end != pattern.end()) {
            RegexAutomata& subsequence = sequence_stack.emplace_back();

            try {
              subsequence = parse({begin, end}, depth + 1);
            } catch (RegexParserException& exception) {
              // Forward the subsequence exception with the complete pattern
              exception.set_pattern(pattern);
              throw exception;
              return {};
            }

            token = end;
          } else {
            throw RegexParserException(exception_desc::REGEX_UNTERMINATED_SUBSEQUENCE, pattern,
                                       token);
          }
        } break;

        case '}': {
          throw RegexParserException(exception_desc::REGEX_UNEXPECTED_SUBSEQUENCE_END, pattern,
                                     token);
        } break;

        case '+': {
          // operand -> epsilon -> operand
          //                    -> next
          if (sequence_stack.empty() || sequence_stack.back().empty())
            throw RegexParserException(exception_desc::REGEX_MISSING_QUANTIFIABLE, pattern, token);

          auto& operand = sequence_stack.back();
          RegexNode epsilon = operand.node_create(RegexAutomata::EPSILON, operand.leafs(), {0});
        } break;

        case '?': {
          // root -> operand -> next
          //      -> epsilon -> next
          if (sequence_stack.empty() || sequence_stack.back().empty())
            throw RegexParserException(exception_desc::REGEX_MISSING_QUANTIFIABLE, pattern, token);

          auto operand = sequence_stack.back();

          sequence_stack.pop_back();
          auto& sequence = sequence_stack.emplace_back();
          RegexNode root = sequence.node_create(RegexAutomata::EPSILON, {}, {});
          sequence.merge(operand, {root});
          RegexNode epsilon = sequence.node_create(RegexAutomata::EPSILON, {root}, {});
        } break;

        case '*': {
          // root -> operand -> root
          //      -> epsilon -> next
          if (sequence_stack.empty() || sequence_stack.back().empty())
            throw RegexParserException(exception_desc::REGEX_MISSING_QUANTIFIABLE, pattern, token);

          auto operand = sequence_stack.back();

          sequence_stack.pop_back();
          auto& sequence = sequence_stack.emplace_back();
          RegexNode root = sequence.node_create(RegexAutomata::EPSILON, {}, {});
          sequence.merge(operand, {root});
          
          const auto operand_leafs = sequence.leafs();
          for (RegexNode operand_leaf : operand_leafs) {
            sequence.node_connect(operand_leaf, {root});
          }

          RegexNode epsilon = sequence.node_create(RegexAutomata::EPSILON, {root}, {});
        } break;

        case ' ': {
          // Spaces are ignored for readability
        } break;
          
        default: {
          throw RegexParserException(exception_desc::REGEX_UNRECOGNIZED_TOKEN, pattern, token);
        } break;
      }
    }

    if (!sequence_stack.empty()) {
      // concatenate sequences
      for (auto itr = sequence_stack.begin(); itr != sequence_stack.end() - 1; itr++) {
        itr->merge(*(itr + 1), itr->leafs());
      }

      auto& automata = sequence_stack.front();
      // Add an exit to the automata except for subsequences
      if (depth < 1) {
        automata.node_create(RegexAutomata::EPSILON, automata.leafs(), {});
      }

      return automata;
    } else {
      return {};
    }
  }
};

}  // namespace sdata

#endif
