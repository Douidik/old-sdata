#ifndef SDATA_EXCEPTION_DESC_HPP
#define SDATA_EXCEPTION_DESC_HPP

#include <string_view>

namespace sdata {
namespace error {

constexpr std::string_view REGEX_UNTERMINATED_LITERAL =
    "Unterminated string literal, missing \' closing character";

constexpr std::string_view REGEX_UNTERMINATED_SUBSEQ =
    "Unterminated subsequence, missing '}' closing character";

constexpr std::string_view REGEX_UNEXPECTED_SUBSEQ_END =
    "Unexpected sequence end, missing '{' opening character";

constexpr std::string_view REGEX_MISSING_QUANTIFIABLE =
    "The preceding sequence is unquantifiable or missing";

constexpr std::string_view REGEX_MISSING_ALTERNATIVE =
    "Missing alternative, the | operator must be surrounded by operands";

constexpr std::string_view REGEX_UNRECOGNIZED_TOKEN =
    "Unrecognized token in pattern (literals must be surrounded by ')";

constexpr std::string_view SCANNER_UNRECOGNIZED_TOKEN = "Unrecognized token in source";

constexpr std::string_view PARSER_INTEGER_OVERFLOW =
    "Integer overflow, number must be contained between -2147483648 and +2147483647";

constexpr std::string_view PARSER_FLOAT_OVERFLOW =
    "Float overflow, number must be contained between -(10^37) and +(10^37)";

constexpr std::string_view PARSER_EXPECTED_TOKEN = "Expected token";

constexpr std::string_view PARSER_UNEXPECTED_TOKEN = "Unexpected token";

}  // namespace error
}  // namespace sdata

#endif
