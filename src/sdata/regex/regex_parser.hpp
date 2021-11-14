#ifndef SDATA_REGEX_PARSER
#define SDATA_REGEX_PARSER

#include <deque>
#include <exception>
#include "regex_automata.hpp"

namespace sdata {

class RegexParserException : public std::exception {
  constexpr static std::string_view PATTERN =
      "[sdata::RegexParserException raised]: %\n"
      "with {pattern: \"%\" at [%]:'%'}";

 public:
  RegexParserException(std::string_view description,
                       std::string_view pattern,
                       std::string_view::iterator token);

  inline const char *what() const noexcept override {
    return m_buffer.data();
  }

 private:
  const std::string m_buffer;
};

class RegexParser {
 public:
  explicit RegexParser(std::string_view pattern);
  RegexAutomata parse();

 private:
  RegexAutomata merge_sequences();
  RegexAutomata parse_operand(std::string_view::iterator &token);
  auto parse_sequence_pattern(std::string_view::iterator &token);

  void parse_token(std::string_view::iterator &token);
  void parse_character_class(std::string_view::iterator &token);
  void parse_any(std::string_view::iterator &token);
  void parse_literal(std::string_view::iterator &token);
  void parse_sequence(std::string_view::iterator &token);
  void parse_alternative(std::string_view::iterator &token);
  void parse_quest(std::string_view::iterator &token);
  void parse_kleene(std::string_view::iterator &token);
  void parse_plus(std::string_view::iterator &token);
  void parse_wave(std::string_view::iterator &token);

  std::deque<RegexAutomata> m_stack;
  std::string_view m_pattern;
};

}  // namespace sdata

#endif
