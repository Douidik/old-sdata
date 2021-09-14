#ifndef SDATA_SCANNER_TEST_HPP
#define SDATA_SCANNER_TEST_HPP

#include <catch2/catch_all.hpp>
#include <filesystem>
#include <fstream>
#include <sdata/scan/scanner.hpp>
#include <streambuf>

#include "catch2/interfaces/catch_interfaces_config.hpp"

TEST_CASE("sdata::Scanner<char> patterns") {
  using namespace std::string_view_literals;

  SECTION("EMPTY") {
    const auto &empty_pattern = sdata::Token<char>::patterns.at(sdata::Token<char>::EMPTY);
    CHECK(empty_pattern.match(" "sv));
    CHECK(empty_pattern.match("   "sv));
    CHECK(empty_pattern.match("\n "sv));
    CHECK(empty_pattern.match("\t\t\n   "sv));
    CHECK_FALSE(empty_pattern.match("@namespace"sv));
  }

  SECTION("STRING") {
    const auto &string_pattern = sdata::Token<char>::patterns.at(sdata::Token<char>::STRING);
    CHECK(string_pattern.match("\"hello world\""sv));
    CHECK(string_pattern.match("\"hello\nworld\""sv));
    CHECK_FALSE(string_pattern.match("hello"sv));
  }

  SECTION("CHARACTER") {
    const auto &character_pattern = sdata::Token<char>::patterns.at(sdata::Token<char>::CHAR);
    CHECK(character_pattern.match("'a'"sv));
    CHECK(character_pattern.match("'@'"sv));
    CHECK(character_pattern.match("'\''"sv));
    CHECK_FALSE(character_pattern.match("'b"sv));
    CHECK_FALSE(character_pattern.match("b'"sv));
    CHECK_FALSE(character_pattern.match("'hello world'"sv));
  }

  SECTION("COMMENT") {
    const auto &comment_pattern = sdata::Token<char>::patterns.at(sdata::Token<char>::COMMENT);
    CHECK(comment_pattern.match("//comment\n"sv));
    CHECK(comment_pattern.match("// comment\n"sv));
    CHECK(comment_pattern.match("// comment"sv));
    CHECK_FALSE(comment_pattern.match("\"hello"sv));
    CHECK_FALSE(comment_pattern.match("hello"sv));
  }

  SECTION("BOOLEAN") {
    const auto &boolean_pattern = sdata::Token<char>::patterns.at(sdata::Token<char>::BOOLEAN);
    CHECK(boolean_pattern.match("true"sv));
    CHECK(boolean_pattern.match("false"sv));
    CHECK_FALSE(boolean_pattern.match("'true'"sv));
  }

  SECTION("FLOAT") {
    const auto &float_pattern = sdata::Token<char>::patterns.at(sdata::Token<char>::FLOAT);
    CHECK(float_pattern.match("1.0f"sv));
    CHECK(float_pattern.match("13243.43934f"sv));
    CHECK(float_pattern.match("0.0003923423493423f"sv));
    CHECK(float_pattern.match("0.0"sv));
    CHECK_FALSE(float_pattern.match("95435"sv));
    CHECK_FALSE(float_pattern.match(".0f"sv));
    CHECK_FALSE(float_pattern.match("0.f"sv));
  }

  SECTION("INTEGER") {
    const auto &integer_pattern = sdata::Token<char>::patterns.at(sdata::Token<char>::INTEGER);
    CHECK(integer_pattern.match("1"sv));
    CHECK(integer_pattern.match("134342"sv));
    CHECK(integer_pattern.match("00313134"sv));
  }

  SECTION("NAMESPACE") {
    const auto &namespace_pattern = sdata::Token<char>::patterns.at(sdata::Token<char>::NAMESPACE);
    CHECK(namespace_pattern.match("@my_namespace"sv));
    CHECK(namespace_pattern.match("@Hello"sv));
    CHECK(namespace_pattern.match("@namespace_number0123"sv));
    CHECK_FALSE(namespace_pattern.match("not_namespace"sv));
    CHECK_FALSE(namespace_pattern.match("@12bad_namespace"sv));
  }

  SECTION("IDENTIFIER") {
    const auto &identifier_pattern =
        sdata::Token<char>::patterns.at(sdata::Token<char>::IDENTIFIER);
    CHECK(identifier_pattern.match("my_identifier"sv));
    CHECK(identifier_pattern.match("my_id_nb123"sv));
    CHECK_FALSE(identifier_pattern.match("@this_is_a_namespace_not_an_id"sv));
    CHECK_FALSE(identifier_pattern.match("12identifier"sv));
  }

  SECTION("OPERATORS") {
    CHECK(sdata::Token<char>::patterns.at(sdata::Token<char>::EQUALS).match("="sv));
    CHECK(sdata::Token<char>::patterns.at(sdata::Token<char>::BEG_SEQ).match("{"sv));
    CHECK(sdata::Token<char>::patterns.at(sdata::Token<char>::END_SEQ).match("}"sv));
    CHECK(sdata::Token<char>::patterns.at(sdata::Token<char>::SEPARATOR).match(","sv));
  }
}

template <typename char_t>
bool token_matches(
    sdata::Scanner<char_t> &scanner,
    std::pair<std::basic_string_view<char_t>, typename sdata::Token<char_t>::Category> &&expected) {
  if (scanner.eof()) return false;

  auto token = scanner.tokenize();
  return expected == std::make_pair(token.expression, token.category);
}

template <typename char_t>
std::basic_string<char_t> read_source(std::filesystem::path path) {
  std::basic_ifstream<char_t> ifstream(path);
  return {std::istreambuf_iterator<char_t>(ifstream), {}};
}

TEST_CASE("sdata::Scanner<char>") {
  using Token = sdata::Token<char>;

  SECTION("game.sdt") {
    std::string source = read_source<char>("examples/game.sdat");
    sdata::Scanner<char> scanner {source};

    REQUIRE(token_matches(scanner, {"@tetris", Token::NAMESPACE}));
    REQUIRE(token_matches(scanner, {"{", Token::BEG_SEQ}));
    {
      REQUIRE(token_matches(scanner, {"window", Token::IDENTIFIER}));
      REQUIRE(token_matches(scanner, {"{", Token::BEG_SEQ}));
      {
        REQUIRE(token_matches(scanner, {"width", Token::IDENTIFIER}));
        REQUIRE(token_matches(scanner, {"=", Token::EQUALS}));
        REQUIRE(token_matches(scanner, {"1920", Token::INTEGER}));
        REQUIRE(token_matches(scanner, {",", Token::SEPARATOR}));

        REQUIRE(token_matches(scanner, {"height", Token::IDENTIFIER}));
        REQUIRE(token_matches(scanner, {"=", Token::EQUALS}));
        REQUIRE(token_matches(scanner, {"1080", Token::INTEGER}));
        REQUIRE(token_matches(scanner, {",", Token::SEPARATOR}));

        REQUIRE(token_matches(scanner, {"title", Token::IDENTIFIER}));
        REQUIRE(token_matches(scanner, {"=", Token::EQUALS}));
        REQUIRE(token_matches(scanner, {"\"Tetris game\"", Token::STRING}));
        REQUIRE(token_matches(scanner, {",", Token::SEPARATOR}));

        REQUIRE(token_matches(scanner, {"}", Token::END_SEQ}));
        REQUIRE(token_matches(scanner, {",", Token::SEPARATOR}));
      }

      REQUIRE(token_matches(scanner, {"controls", Token::IDENTIFIER}));
      REQUIRE(token_matches(scanner, {"{", Token::BEG_SEQ}));
      {
        REQUIRE(token_matches(scanner, {"left", Token::IDENTIFIER}));
        REQUIRE(token_matches(scanner, {"=", Token::EQUALS}));
        REQUIRE(token_matches(scanner, {"'a'", Token::CHAR}));
        REQUIRE(token_matches(scanner, {",", Token::SEPARATOR}));

        REQUIRE(token_matches(scanner, {"right", Token::IDENTIFIER}));
        REQUIRE(token_matches(scanner, {"=", Token::EQUALS}));
        REQUIRE(token_matches(scanner, {"'d'", Token::CHAR}));
        REQUIRE(token_matches(scanner, {",", Token::SEPARATOR}));

        REQUIRE(token_matches(scanner, {"confirm", Token::IDENTIFIER}));
        REQUIRE(token_matches(scanner, {"=", Token::EQUALS}));
        REQUIRE(token_matches(scanner, {"'e'", Token::CHAR}));
        REQUIRE(token_matches(scanner, {",", Token::SEPARATOR}));

        REQUIRE(token_matches(scanner, {"pause", Token::IDENTIFIER}));
        REQUIRE(token_matches(scanner, {"=", Token::EQUALS}));
        REQUIRE(token_matches(scanner, {"'p'", Token::CHAR}));
        REQUIRE(token_matches(scanner, {",", Token::SEPARATOR}));

        REQUIRE(token_matches(scanner, {"}", Token::END_SEQ}));
        REQUIRE(token_matches(scanner, {",", Token::SEPARATOR}));
      }

      REQUIRE(token_matches(scanner, {"}", Token::END_SEQ}));
      REQUIRE(token_matches(scanner, {",", Token::SEPARATOR}));
    }
  }
}

TEST_CASE("sdata::Scanner<char16_t>") {
  using Token = sdata::Token<char16_t>;

  SECTION("dialog.sdt") {
    std::u16string source = read_source<char16_t>("examples/dialog.sdat");
    sdata::Scanner<char16_t> scanner {source};

    REQUIRE(token_matches(scanner, {u"@en_US", Token::NAMESPACE}));
    REQUIRE(token_matches(scanner, {u"{", Token::BEG_SEQ}));
    {
      REQUIRE(token_matches(scanner, {u"game_over_dialog", Token::IDENTIFIER}));
      REQUIRE(token_matches(scanner, {u"{", Token::BEG_SEQ}));
      {
        REQUIRE(token_matches(scanner, {u"title", Token::IDENTIFIER}));
        REQUIRE(token_matches(scanner, {u"=", Token::EQUALS}));
        REQUIRE(token_matches(scanner, {u"\"GAME OVER\"", Token::STRING}));
        REQUIRE(token_matches(scanner, {u",", Token::SEPARATOR}));

        REQUIRE(token_matches(scanner, {u"play_again_prompt", Token::IDENTIFIER}));
        REQUIRE(token_matches(scanner, {u"=", Token::EQUALS}));
        REQUIRE(token_matches(scanner, {u"\"Play again ?\"", Token::STRING}));
        REQUIRE(token_matches(scanner, {u",", Token::SEPARATOR}));

        REQUIRE(token_matches(scanner, {u"play_again_accept", Token::IDENTIFIER}));
        REQUIRE(token_matches(scanner, {u"=", Token::EQUALS}));
        REQUIRE(token_matches(scanner, {u"\"Yes\"", Token::STRING}));
        REQUIRE(token_matches(scanner, {u",", Token::SEPARATOR}));

        REQUIRE(token_matches(scanner, {u"play_again_refuse", Token::IDENTIFIER}));
        REQUIRE(token_matches(scanner, {u"=", Token::EQUALS}));
        REQUIRE(token_matches(scanner, {u"\"No\"", Token::STRING}));
        REQUIRE(token_matches(scanner, {u",", Token::SEPARATOR}));

        REQUIRE(token_matches(scanner, {u"}", Token::END_SEQ}));
        REQUIRE(token_matches(scanner, {u",", Token::SEPARATOR}));
      }

      REQUIRE(token_matches(scanner, {u"}", Token::END_SEQ}));
      REQUIRE(token_matches(scanner, {u",", Token::SEPARATOR}));
    }

    REQUIRE(token_matches(scanner, {u"@fr_FR", Token::NAMESPACE}));
    REQUIRE(token_matches(scanner, {u"{", Token::BEG_SEQ}));
    {
      REQUIRE(token_matches(scanner, {u"game_over_dialog", Token::IDENTIFIER}));
      REQUIRE(token_matches(scanner, {u"{", Token::BEG_SEQ}));
      {
        REQUIRE(token_matches(scanner, {u"title", Token::IDENTIFIER}));
        REQUIRE(token_matches(scanner, {u"=", Token::EQUALS}));
        REQUIRE(token_matches(scanner, {u"\"Partie terminée\"", Token::STRING}));
        REQUIRE(token_matches(scanner, {u",", Token::SEPARATOR}));

        REQUIRE(token_matches(scanner, {u"play_again_prompt", Token::IDENTIFIER}));
        REQUIRE(token_matches(scanner, {u"=", Token::EQUALS}));
        REQUIRE(token_matches(scanner, {u"\"Souhaitez-vous rejouer ?\"", Token::STRING}));
        REQUIRE(token_matches(scanner, {u",", Token::SEPARATOR}));

        REQUIRE(token_matches(scanner, {u"play_again_accept", Token::IDENTIFIER}));
        REQUIRE(token_matches(scanner, {u"=", Token::EQUALS}));
        REQUIRE(token_matches(scanner, {u"\"Oui\"", Token::STRING}));
        REQUIRE(token_matches(scanner, {u",", Token::SEPARATOR}));

        REQUIRE(token_matches(scanner, {u"play_again_refuse", Token::IDENTIFIER}));
        REQUIRE(token_matches(scanner, {u"=", Token::EQUALS}));
        REQUIRE(token_matches(scanner, {u"\"Non\"", Token::STRING}));
        REQUIRE(token_matches(scanner, {u",", Token::SEPARATOR}));

        REQUIRE(token_matches(scanner, {u"}", Token::END_SEQ}));
        REQUIRE(token_matches(scanner, {u",", Token::SEPARATOR}));
      }

      REQUIRE(token_matches(scanner, {u"}", Token::END_SEQ}));
      REQUIRE(token_matches(scanner, {u",", Token::SEPARATOR}));
    }

    REQUIRE(token_matches(scanner, {u"@es_ES", Token::NAMESPACE}));
    REQUIRE(token_matches(scanner, {u"{", Token::BEG_SEQ}));
    {
      REQUIRE(token_matches(scanner, {u"game_over_dialog", Token::IDENTIFIER}));
      REQUIRE(token_matches(scanner, {u"{", Token::BEG_SEQ}));
      {
        REQUIRE(token_matches(scanner, {u"title", Token::IDENTIFIER}));
        REQUIRE(token_matches(scanner, {u"=", Token::EQUALS}));
        REQUIRE(token_matches(scanner, {u"\"Juego terminado\"", Token::STRING}));
        REQUIRE(token_matches(scanner, {u",", Token::SEPARATOR}));

        REQUIRE(token_matches(scanner, {u"play_again_prompt", Token::IDENTIFIER}));
        REQUIRE(token_matches(scanner, {u"=", Token::EQUALS}));
        REQUIRE(token_matches(scanner, {u"\"Juega de nuevo ?\"", Token::STRING}));
        REQUIRE(token_matches(scanner, {u",", Token::SEPARATOR}));

        REQUIRE(token_matches(scanner, {u"play_again_accept", Token::IDENTIFIER}));
        REQUIRE(token_matches(scanner, {u"=", Token::EQUALS}));
        REQUIRE(token_matches(scanner, {u"\"Sí\"", Token::STRING}));
        REQUIRE(token_matches(scanner, {u",", Token::SEPARATOR}));

        REQUIRE(token_matches(scanner, {u"play_again_refuse", Token::IDENTIFIER}));
        REQUIRE(token_matches(scanner, {u"=", Token::EQUALS}));
        REQUIRE(token_matches(scanner, {u"\"No\"", Token::STRING}));
        REQUIRE(token_matches(scanner, {u",", Token::SEPARATOR}));

        REQUIRE(token_matches(scanner, {u"}", Token::END_SEQ}));
        REQUIRE(token_matches(scanner, {u",", Token::SEPARATOR}));
      }

      REQUIRE(token_matches(scanner, {u"}", Token::END_SEQ}));
      REQUIRE(token_matches(scanner, {u",", Token::SEPARATOR}));
    }

    REQUIRE(token_matches(scanner, {u"@zh_CN", Token::NAMESPACE}));
    REQUIRE(token_matches(scanner, {u"{", Token::BEG_SEQ}));
    {
      REQUIRE(token_matches(scanner, {u"game_over_dialog", Token::IDENTIFIER}));
      REQUIRE(token_matches(scanner, {u"{", Token::BEG_SEQ}));
      {
        REQUIRE(token_matches(scanner, {u"title", Token::IDENTIFIER}));
        REQUIRE(token_matches(scanner, {u"=", Token::EQUALS}));
        REQUIRE(token_matches(scanner, {u"\"游戏结束\"", Token::STRING}));
        REQUIRE(token_matches(scanner, {u",", Token::SEPARATOR}));

        REQUIRE(token_matches(scanner, {u"play_again_prompt", Token::IDENTIFIER}));
        REQUIRE(token_matches(scanner, {u"=", Token::EQUALS}));
        REQUIRE(token_matches(scanner, {u"\"再玩一次 ？\"", Token::STRING}));
        REQUIRE(token_matches(scanner, {u",", Token::SEPARATOR}));

        REQUIRE(token_matches(scanner, {u"play_again_accept", Token::IDENTIFIER}));
        REQUIRE(token_matches(scanner, {u"=", Token::EQUALS}));
        REQUIRE(token_matches(scanner, {u"\"是的\"", Token::STRING}));
        REQUIRE(token_matches(scanner, {u",", Token::SEPARATOR}));

        REQUIRE(token_matches(scanner, {u"play_again_refuse", Token::IDENTIFIER}));
        REQUIRE(token_matches(scanner, {u"=", Token::EQUALS}));
        REQUIRE(token_matches(scanner, {u"\"不\"", Token::STRING}));
        REQUIRE(token_matches(scanner, {u",", Token::SEPARATOR}));

        REQUIRE(token_matches(scanner, {u"}", Token::END_SEQ}));
        REQUIRE(token_matches(scanner, {u",", Token::SEPARATOR}));
      }

      REQUIRE(token_matches(scanner, {u"}", Token::END_SEQ}));
      REQUIRE(token_matches(scanner, {u",", Token::SEPARATOR}));
    }
  }
}

#endif
