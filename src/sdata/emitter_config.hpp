#ifndef SDATA_EMITTER_CONFIG_HPP
#define SDATA_EMITTER_CONFIG_HPP

#include <string_view>

namespace sdata {

enum Style : unsigned int {
  STYLE_BREAK_BEFORE_BRACES = 1 << 0,
  STYLE_SPACE_BEFORE_BRACES = 1 << 1,
  STYLE_BREAK_AFTER_BRACES = 1 << 2,
  STYLE_SPACE_AFTER_BRACES = 1 << 3,

  STYLE_SPACE_BEFORE_ASSIGN = 1 << 4,
  STYLE_SPACE_AFTER_ASSIGN = 1 << 5,

  STYLE_BREAK_AFTER_SEPARATOR = 1 << 6,
  STYLE_SPACE_AFTER_SEPARATOR = 1 << 7,

  STYLE_BREAK_ANONYMOUS_BRACES = 1 << 8,
};

struct EmitterConfig {
  std::string_view indent;
  unsigned int style = 0x0;
};

constexpr EmitterConfig DEFAULT_EMITTER_CONFIG{
    .indent = "  ",
    .style = STYLE_BREAK_AFTER_BRACES | STYLE_SPACE_BEFORE_BRACES | STYLE_SPACE_AFTER_ASSIGN |
             STYLE_BREAK_AFTER_SEPARATOR | STYLE_BREAK_ANONYMOUS_BRACES,
};

constexpr EmitterConfig INLINE_EMITTER_CONFIG{
    .indent = "",
    .style = STYLE_SPACE_BEFORE_BRACES | STYLE_SPACE_AFTER_BRACES | STYLE_SPACE_AFTER_ASSIGN |
             STYLE_SPACE_AFTER_SEPARATOR,
};

}  // namespace sdata

#endif
