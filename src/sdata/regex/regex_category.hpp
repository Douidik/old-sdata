#ifndef SDATA_REGEX_CATEGORY_HPP
#define SDATA_REGEX_CATEGORY_HPP

namespace sdata {

enum RegexCategory : char {
  REGEX_UNKNOWN = '\0',

  REGEX_BLANK = '_',
  REGEX_ALPHA = 'a',
  REGEX_OPERATOR = 'o',
  REGEX_DIGIT = 'd',
  REGEX_QUOTE = 'Q',
  REGEX_APOSTROPHE = 'q',
  REGEX_ANY = '^',

  REGEX_LITERAL = '\'',
  REGEX_BEG_SUBSEQ = '{',
  REGEX_END_SUBSEQ = '}',
  REGEX_ALTERNATIVE = '|',

  REGEX_QUEST = '?',
  REGEX_KLEENE = '*',
  REGEX_PLUS = '+',
  REGEX_UNTIL = '$'
};

}  // namespace sdata

#endif
