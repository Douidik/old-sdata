#ifndef SDATA_REGEX_TEST_HPP
#define SDATA_REGEX_TEST_HPP

#include <catch2/catch_all.hpp>
#include <iomanip>
#include <iostream>
#include <sdata/scan/regex.hpp>
#include <string_view>

template <typename char_t>
bool regex_match(std::basic_string_view<char_t> pattern,
                 std::basic_string_view<char_t> expression) {
  sdata::Regex<char_t> regex(pattern);
  return regex.match(expression.begin(), expression.end());
}

template <typename char_t>
bool regex_fullmatch(std::basic_string_view<char_t> pattern,
                     std::basic_string_view<char_t> expression) {
  sdata::Regex<char_t> regex(pattern);
  return regex.fullmatch(expression.begin(), expression.end());
}

// one byte wide characters
constexpr std::string_view LOREM_IPSUM = R"(
Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.
Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.
Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur.
Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.
)";

// two byte wide characters
constexpr std::wstring_view LOREM_IPSUM_FRENCH =
    LR"(
Pour vous faire mieux connaitre d’où vient l’erreur de ceux qui blâment la volupté, et qui louent en
quelque sorte la douleur, je vais entrer dans une explication plus étendue, et vous faire voir tout
ce qui a été dit là-dessus par l’inventeur de la vérité, et, pour ainsi dire, par l’architecte de
la vie heureuse. Personne [dit Épicure] ne craint ni ne fuit la volupté en tant que volupté, mais
en tant qu’elle attire de grandes douleurs à ceux qui ne savent pas en faire un usage modéré et
raisonnable ; et personne n’aime ni ne recherche la douleur comme douleur, mais parce qu’il arrive
quelquefois que, par le travail et par la peine, on parvienne à jouir d’une grande volupté.
)";

constexpr std::u16string_view LOREM_IPSUM_CHINESE =
    uR"(
設図記隣安筆鉄療明減戦因基空。
検芸津試清芝護対返着専図座。
会詳応含重英室注続藤議近永整端応講付検画。
良生支手側断生東芸期熊用帰兼特植。
姫石作末期校知後葉町定与方交型読変軽金。
村囲会列瑙容免危年府集際訴航弁景詳衛妙。
育競流引得活者括績沖勢法提無裁。
映受消知護上用優引真本医。
長日断武住砲東誕検生話独間髪号域表。
)";

template <typename char_t>
static std::basic_string<char_t> quoted(std::basic_string_view<char_t> expression) {
  std::basic_stringstream<char_t> ss;
  ss << std::quoted<char_t>(expression, 0x27);
  return ss.str();
}

TEST_CASE("sdata::Regex<char> LITERAL") {
  SECTION("MINIMAL") {
    CHECK(regex_match<char>("'abc'", "abc"));
    CHECK(regex_match<char>("'abc'", "abcccccccccc"));
    CHECK(regex_match<char>("'hello ' 'world'", "hello world"));
    CHECK(regex_match<char>("'hello\nworld'", "hello\nworld"));
    CHECK(regex_match<char>(quoted(LOREM_IPSUM), LOREM_IPSUM));
  }

  SECTION("INVALID") {
    REQUIRE_THROWS_AS(regex_match<char>("'hello world", "hello world"),
                      sdata::RegexParserException<char>);
    REQUIRE_THROWS_AS(regex_match<char>("'", ""), sdata::RegexParserException<char>);
  }

  SECTION("FALSE") {
    CHECK_FALSE(regex_match<char>("'cba'", "abc"));
    CHECK_FALSE(regex_match<char>("'cbaa'", "aabc"));
    CHECK_FALSE(regex_match<char>(quoted(LOREM_IPSUM), LOREM_IPSUM.substr(1)));
  }

  SECTION("FULLMATCH") {
    CHECK_FALSE(regex_fullmatch<char>("'abccccccccc'", "abc"));
    CHECK_FALSE(regex_fullmatch<char>("'abc'", "abcdef"));
  }
}

TEST_CASE("sdata::Regex<char> CHAR_CLASSES") {
  SECTION("MINIMAL") {
    CHECK(regex_match<char>("_", "\n"));
    CHECK(regex_match<char>("a", "a"));
    CHECK(regex_match<char>("o", "+"));
    CHECK(regex_match<char>("d", "0"));
    CHECK(regex_match<char>("Q", "\""));
    CHECK(regex_match<char>("q", "'"));
  }

  SECTION("FALSE") {
    CHECK_FALSE(regex_match<char>("_", "h"));
    CHECK_FALSE(regex_match<char>("a", "1"));
    CHECK_FALSE(regex_match<char>("o", "A"));
    CHECK_FALSE(regex_match<char>("d", "a"));
    CHECK_FALSE(regex_match<char>("Q", "v"));
    CHECK_FALSE(regex_match<char>("q", "%"));
  }

  SECTION("FULLMATCH") {
    CHECK(regex_fullmatch<char>("dd", "01"));
    CHECK_FALSE(regex_fullmatch<char>("_", "\t\t"));
    CHECK_FALSE(regex_fullmatch<char>("ooo", "+"));
  }
}

TEST_CASE("sdata::Regex<char> SEQUENCES") {
  SECTION("MINIMAL") {
    CHECK(regex_match<char>("{'ABC'}", "ABC"));
    CHECK(regex_match<char>("{'AB'} {'CD'}", "ABCD"));
    CHECK(regex_match<char>("{{{{{'a'}}}}}", "a"));
  }

  SECTION("INVALID") {
    REQUIRE_THROWS_AS(regex_match<char>("{aaa", "zyx"), sdata::RegexParserException<char>);
    REQUIRE_THROWS_AS(regex_match<char>("{", ""), sdata::RegexParserException<char>);
    REQUIRE_THROWS_AS(regex_match<char>("}", ""), sdata::RegexParserException<char>);
    REQUIRE_THROWS_AS(regex_match<char>("'hello'}}}", "hello"), sdata::RegexParserException<char>);
    REQUIRE_THROWS_AS(regex_match<char>("{{{'hello'", "hello"), sdata::RegexParserException<char>);
  }
}

TEST_CASE("sdata::Regex<char> QUANTIFIER +") {
  SECTION("MINIMAL") {
    CHECK(regex_match<char>("{'abc'}+", "abcabc"));
    CHECK(regex_match<char>("{'ab'd}+", "ab1ab2ab3"));

    CHECK(regex_match<char>("d+d+", "12"));

    CHECK(regex_match<char>("d+ o d+ '=' d+", "12+90=102"));
    CHECK(regex_match<char>("{{{'hello'}}}+", "hellohello"));
  }

  SECTION("FALSE") {
    CHECK_FALSE(regex_match<char>("{'abc'}+", ""));
    CHECK_FALSE(regex_match<char>("{'ab'd}+", "ab+"));
  }

  SECTION("INVALID") {
    REQUIRE_THROWS_AS(regex_match<char>("+", "a"), sdata::RegexParserException<char>);
    REQUIRE_THROWS_AS(regex_match<char>("+++", "a"), sdata::RegexParserException<char>);
  }
}

TEST_CASE("sdata::Regex<char> QUANTIFIER *") {
  SECTION("MINIMAL") {
    CHECK(regex_match<char>("{'abc'}*", "abc"));
    CHECK(regex_match<char>("{'abc'}*", ""));
    CHECK(regex_match<char>("{'ab'd}*", "ab1ab2ab3"));
    CHECK(regex_match<char>("{{{'hello'}}}*", ""));
    CHECK(regex_match<char>("{{{'hello'}}}*", "hellohello"));
  }

  SECTION("INVALID") {
    REQUIRE_THROWS_AS(regex_match<char>("*", "a"), sdata::RegexParserException<char>);
    REQUIRE_THROWS_AS(regex_match<char>("***", "a"), sdata::RegexParserException<char>);
  }

  SECTION("FULLMATCH") {
    CHECK_FALSE(regex_fullmatch<char>("{'abc'}*", "aaa"));
    CHECK_FALSE(regex_fullmatch<char>("{'ab'd}*", "ab*"));
  }
}

TEST_CASE("sdata::Regex<char> QUANTIFIER ?") {
  SECTION("MINIMAL") {
    CHECK(regex_match<char>("{'abc'}?", "abc"));
    CHECK(regex_match<char>("{'abc'}?", ""));
    CHECK(regex_match<char>("{'ab'd}?", "ab1"));
    CHECK(regex_match<char>("{{{'hello'}}}?", ""));
    CHECK(regex_match<char>("{{{'hello'}}}?", "hello"));
    CHECK(regex_match<char>("d+'.'d+ 'f'?", "12.95f"));
  }

  SECTION("INVALID") {
    REQUIRE_THROWS_AS(regex_match<char>("?", "a"), sdata::RegexParserException<char>);
    REQUIRE_THROWS_AS(regex_match<char>("???", "a"), sdata::RegexParserException<char>);
  }

  SECTION("FULLMATCH") {
    CHECK(regex_fullmatch<char>("{'abc'}?", ""));
    CHECK_FALSE(regex_fullmatch<char>("{'ab'd}?", "ab?"));
  }
}

TEST_CASE("sdata::Regex<char> UNKNOWN TOKENS") {
  REQUIRE_THROWS_AS(regex_match<char>("N", "12"), sdata::RegexParserException<char>);
  REQUIRE_THROWS_AS(regex_match<char>(")", ")"), sdata::RegexParserException<char>);
  REQUIRE_THROWS_AS(regex_match<char>("ù", ""), sdata::RegexParserException<char>);
  REQUIRE_THROWS_AS(regex_match<char>("hello", "hello"), sdata::RegexParserException<char>);
}

TEST_CASE("sdata::Regex<char> ALTERNATIVE") {
  SECTION("MINIMAL") {
    CHECK(regex_match<char>("'a'|'b'", "a"));
    CHECK(regex_match<char>("'a'|'b'", "b"));

    CHECK(regex_match<char>("'a' | 'b'", "a"));
    CHECK(regex_match<char>("'a' | 'b'", "b"));

    CHECK(regex_match<char>("a{a|d}*", "camelCase123"));
    CHECK(regex_match<char>("a{a|'_'|d}*", "snake_case_var123"));
  }

  SECTION("INVALID") {
    REQUIRE_THROWS_AS(regex_match<char>("|", ""), sdata::RegexParserException<char>);
    REQUIRE_THROWS_AS(regex_match<char>("||", ""), sdata::RegexParserException<char>);
    REQUIRE_THROWS_AS(regex_match<char>("|||", ""), sdata::RegexParserException<char>);
    REQUIRE_THROWS_AS(regex_match<char>("'a'|", ""), sdata::RegexParserException<char>);
    REQUIRE_THROWS_AS(regex_match<char>("|'b'", ""), sdata::RegexParserException<char>);
    REQUIRE_THROWS_AS(regex_match<char>("|'b'", ""), sdata::RegexParserException<char>);
  }
}

TEST_CASE("sdata::Regex<char> UNTIL") {
  SECTION("MINIMAL") {
    CHECK(regex_match<char>("'z'$", "abcdefghijklmnopqrstuvwxyz"));
  }
}

TEST_CASE("sdata::Regex<wchar_t>") {
  CHECK(regex_match<wchar_t>(L"'café'", L"café"));
  CHECK(regex_match<wchar_t>(quoted<wchar_t>(LOREM_IPSUM_FRENCH), LOREM_IPSUM_FRENCH));
}

TEST_CASE("sdata::Regex<char16_t>") {
  CHECK(regex_match<char16_t>(u"'你好，世界'", u"你好，世界"));
  CHECK(regex_match<char16_t>(quoted<char16_t>(LOREM_IPSUM_CHINESE), LOREM_IPSUM_CHINESE));
}

#endif
