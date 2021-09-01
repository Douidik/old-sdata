#ifndef SDATA_EXCEPTION_DESC_HPP
#define SDATA_EXCEPTION_DESC_HPP

#include <string_view>

namespace sdata {
namespace error {

constexpr std::string_view REGEX_UNTERMINATED_LITERAL =
    "Unterminated string literal, missing \' closing character";

constexpr std::string_view REGEX_UNTERMINATED_SUBSEQ =
    "Unterminated subsequence, missing '}' closing character";

constexpr std::string_view REGEX_EMPTY_SUBSEQ =
    "Empty subsequence";

constexpr std::string_view REGEX_UNEXPECTED_SUBSEQ_END =
    "Unexpected sequence end, missing '{' opening character";

constexpr std::string_view REGEX_MISSING_QUANTIFIABLE =
    "The preceding sequence is unquantifiable or missing";

constexpr std::string_view REGEX_MISSING_ALTERNATIVE =
    "Missing alternative, the | operator must be surrounded by operands";

constexpr std::string_view REGEX_UNRECOGNIZED_TOKEN =
    "Unrecognized token in pattern (literals must be surrounded by ')";

}  // namespace exception_desc
}  // namespace sdata

#endif
