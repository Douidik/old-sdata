#ifndef SDATA_SCANNER_TEST_HPP
#define SDATA_SCANNER_TEST_HPP

#include <catch2/catch.hpp>
#include <sdata/sdata.hpp>

using namespace sdata;

TEST_CASE("Scanner<char> patterns") {
  SECTION("EMPTY") {
    const auto &empty_pattern = s_token_patterns.at(TOKEN_EMPTY);
    CHECK(empty_pattern.match(" "));
    CHECK(empty_pattern.match("   "));
    CHECK(empty_pattern.match("\n "));
    CHECK(empty_pattern.match("\t\t\n   "));
    CHECK_FALSE(empty_pattern.match("@namespace"));
  }

  SECTION("STRING") {
    const auto &string_pattern = s_token_patterns.at(TOKEN_STRING);
    CHECK(string_pattern.match("\"hello world\""));
    CHECK(string_pattern.match("\"hello\nworld\""));
    CHECK_FALSE(string_pattern.match("hello"));
  }

  SECTION("CHARACTER") {
    const auto &character_pattern = s_token_patterns.at(TOKEN_CHAR);
    CHECK(character_pattern.match("'a'"));
    CHECK(character_pattern.match("'@'"));
    CHECK(character_pattern.match("'\''"));
    CHECK_FALSE(character_pattern.match("'b"));
    CHECK_FALSE(character_pattern.match("b'"));
    CHECK_FALSE(character_pattern.match("'hello world'"));
  }

  SECTION("BOOLEAN") {
    const auto &boolean_pattern = s_token_patterns.at(TOKEN_BOOL);
    CHECK(boolean_pattern.match("true"));
    CHECK(boolean_pattern.match("false"));
    CHECK_FALSE(boolean_pattern.match("'true'"));
  }

  SECTION("FLOAT") {
    const auto &float_pattern = s_token_patterns.at(TOKEN_FLOAT);
    CHECK(float_pattern.match("1.0f"));
    CHECK(float_pattern.match("13243.43934f"));
    CHECK(float_pattern.match("0.0003923423493423f"));
    CHECK(float_pattern.match("0.0"));
    CHECK_FALSE(float_pattern.match("95435"));
    CHECK_FALSE(float_pattern.match(".0f"));
    CHECK_FALSE(float_pattern.match("0.f"));
  }

  SECTION("INTEGER") {
    const auto &integer_pattern = s_token_patterns.at(TOKEN_INT);
    CHECK(integer_pattern.match("1"));
    CHECK(integer_pattern.match("134342"));
    CHECK(integer_pattern.match("00313134"));
  }

  SECTION("IDENTIFIER") {
    const auto &identifier_pattern = s_token_patterns.at(TOKEN_ID);
    CHECK(identifier_pattern.match("my_identifier"));
    CHECK(identifier_pattern.match("my_id_nb123"));
    CHECK_FALSE(identifier_pattern.match("@this_is_a_namespace_not_an_id"));
    CHECK_FALSE(identifier_pattern.match("12identifier"));
  }

  SECTION("OPERATORS") {
    CHECK(s_token_patterns.at(TOKEN_ASSIGN).match(":"));
    CHECK(s_token_patterns.at(TOKEN_BEG_SEQ).match("{"));
    CHECK(s_token_patterns.at(TOKEN_END_SEQ).match("}"));
    CHECK(s_token_patterns.at(TOKEN_SEPARATOR).match(","));
  }
}

template <typename CharT>
bool token_matches(Scanner<CharT> &scanner,
                   std::pair<std::basic_string_view<CharT>, TokenCategory> &&expected) {
  auto token = scanner.tokenize();
  return expected.first == token.expression && expected.second == token.category;
}

TEST_CASE("Scanner<char>") {
  std::string source = read_source_file<char>("examples/game.sd");
  Scanner<char> scanner{source};

  REQUIRE(token_matches(scanner, {"tetris", TOKEN_ID}));
  REQUIRE(token_matches(scanner, {"{", TOKEN_BEG_SEQ}));
  {
    REQUIRE(token_matches(scanner, {"window", TOKEN_ID}));
    REQUIRE(token_matches(scanner, {"{", TOKEN_BEG_SEQ}));
    {
      REQUIRE(token_matches(scanner, {"width", TOKEN_ID}));
      REQUIRE(token_matches(scanner, {":", TOKEN_ASSIGN}));
      REQUIRE(token_matches(scanner, {"1920", TOKEN_INT}));
      REQUIRE(token_matches(scanner, {",", TOKEN_SEPARATOR}));

      REQUIRE(token_matches(scanner, {"height", TOKEN_ID}));
      REQUIRE(token_matches(scanner, {":", TOKEN_ASSIGN}));
      REQUIRE(token_matches(scanner, {"1080", TOKEN_INT}));
      REQUIRE(token_matches(scanner, {",", TOKEN_SEPARATOR}));

      REQUIRE(token_matches(scanner, {"title", TOKEN_ID}));
      REQUIRE(token_matches(scanner, {":", TOKEN_ASSIGN}));
      REQUIRE(token_matches(scanner, {"\"Tetris game\"", TOKEN_STRING}));
      REQUIRE(token_matches(scanner, {",", TOKEN_SEPARATOR}));

      REQUIRE(token_matches(scanner, {"fullscreen", TOKEN_ID}));
      REQUIRE(token_matches(scanner, {":", TOKEN_ASSIGN}));
      REQUIRE(token_matches(scanner, {"false", TOKEN_BOOL}));

      REQUIRE(token_matches(scanner, {"}", TOKEN_END_SEQ}));
      REQUIRE(token_matches(scanner, {",", TOKEN_SEPARATOR}));
    }

    REQUIRE(token_matches(scanner, {"controls", TOKEN_ID}));
    REQUIRE(token_matches(scanner, {"{", TOKEN_BEG_SEQ}));
    {
      REQUIRE(token_matches(scanner, {"left", TOKEN_ID}));
      REQUIRE(token_matches(scanner, {":", TOKEN_ASSIGN}));
      REQUIRE(token_matches(scanner, {"'a'", TOKEN_CHAR}));
      REQUIRE(token_matches(scanner, {",", TOKEN_SEPARATOR}));

      REQUIRE(token_matches(scanner, {"right", TOKEN_ID}));
      REQUIRE(token_matches(scanner, {":", TOKEN_ASSIGN}));
      REQUIRE(token_matches(scanner, {"'d'", TOKEN_CHAR}));
      REQUIRE(token_matches(scanner, {",", TOKEN_SEPARATOR}));

      REQUIRE(token_matches(scanner, {"confirm", TOKEN_ID}));
      REQUIRE(token_matches(scanner, {":", TOKEN_ASSIGN}));
      REQUIRE(token_matches(scanner, {"'e'", TOKEN_CHAR}));
      REQUIRE(token_matches(scanner, {",", TOKEN_SEPARATOR}));

      REQUIRE(token_matches(scanner, {"pause", TOKEN_ID}));
      REQUIRE(token_matches(scanner, {":", TOKEN_ASSIGN}));
      REQUIRE(token_matches(scanner, {"'p'", TOKEN_CHAR}));

      REQUIRE(token_matches(scanner, {"}", TOKEN_END_SEQ}));
    }

    REQUIRE(token_matches(scanner, {"}", TOKEN_END_SEQ}));
  }
}

TEST_CASE("Scanner<char16_t>") {
  std::u16string source = read_source_file<char16_t>("examples/dialog.sd");
  sdata::Scanner<char16_t> scanner{source};

  REQUIRE(token_matches(scanner, {u"{", TOKEN_BEG_SEQ}));
  {
    REQUIRE(token_matches(scanner, {u"en_US", TOKEN_ID}));
    REQUIRE(token_matches(scanner, {u"{", TOKEN_BEG_SEQ}));
    {
      REQUIRE(token_matches(scanner, {u"game_over_dialog", TOKEN_ID}));
      REQUIRE(token_matches(scanner, {u"{", TOKEN_BEG_SEQ}));
      {
        REQUIRE(token_matches(scanner, {u"title", TOKEN_ID}));
        REQUIRE(token_matches(scanner, {u":", TOKEN_ASSIGN}));
        REQUIRE(token_matches(scanner, {u"\"Game over\"", TOKEN_STRING}));
        REQUIRE(token_matches(scanner, {u",", TOKEN_SEPARATOR}));

        REQUIRE(token_matches(scanner, {u"play_again_prompt", TOKEN_ID}));
        REQUIRE(token_matches(scanner, {u":", TOKEN_ASSIGN}));
        REQUIRE(token_matches(scanner, {u"\"Play again ?\"", TOKEN_STRING}));
        REQUIRE(token_matches(scanner, {u",", TOKEN_SEPARATOR}));

        REQUIRE(token_matches(scanner, {u"play_again_accept", TOKEN_ID}));
        REQUIRE(token_matches(scanner, {u":", TOKEN_ASSIGN}));
        REQUIRE(token_matches(scanner, {u"\"Yes\"", TOKEN_STRING}));
        REQUIRE(token_matches(scanner, {u",", TOKEN_SEPARATOR}));

        REQUIRE(token_matches(scanner, {u"play_again_refuse", TOKEN_ID}));
        REQUIRE(token_matches(scanner, {u":", TOKEN_ASSIGN}));
        REQUIRE(token_matches(scanner, {u"\"No\"", TOKEN_STRING}));

        REQUIRE(token_matches(scanner, {u"}", TOKEN_END_SEQ}));
      }

      REQUIRE(token_matches(scanner, {u"}", TOKEN_END_SEQ}));
      REQUIRE(token_matches(scanner, {u",", TOKEN_SEPARATOR}));
    }

    REQUIRE(token_matches(scanner, {u"fr_FR", TOKEN_ID}));
    REQUIRE(token_matches(scanner, {u"{", TOKEN_BEG_SEQ}));
    {
      REQUIRE(token_matches(scanner, {u"game_over_dialog", TOKEN_ID}));
      REQUIRE(token_matches(scanner, {u"{", TOKEN_BEG_SEQ}));
      {
        REQUIRE(token_matches(scanner, {u"title", TOKEN_ID}));
        REQUIRE(token_matches(scanner, {u":", TOKEN_ASSIGN}));
        REQUIRE(token_matches(scanner, {u"\"Partie terminée\"", TOKEN_STRING}));
        REQUIRE(token_matches(scanner, {u",", TOKEN_SEPARATOR}));

        REQUIRE(token_matches(scanner, {u"play_again_prompt", TOKEN_ID}));
        REQUIRE(token_matches(scanner, {u":", TOKEN_ASSIGN}));
        REQUIRE(token_matches(scanner, {u"\"Souhaitez-vous rejouer ?\"", TOKEN_STRING}));
        REQUIRE(token_matches(scanner, {u",", TOKEN_SEPARATOR}));

        REQUIRE(token_matches(scanner, {u"play_again_accept", TOKEN_ID}));
        REQUIRE(token_matches(scanner, {u":", TOKEN_ASSIGN}));
        REQUIRE(token_matches(scanner, {u"\"Oui\"", TOKEN_STRING}));
        REQUIRE(token_matches(scanner, {u",", TOKEN_SEPARATOR}));

        REQUIRE(token_matches(scanner, {u"play_again_refuse", TOKEN_ID}));
        REQUIRE(token_matches(scanner, {u":", TOKEN_ASSIGN}));
        REQUIRE(token_matches(scanner, {u"\"Non\"", TOKEN_STRING}));
        REQUIRE(token_matches(scanner, {u"}", TOKEN_END_SEQ}));
      }

      REQUIRE(token_matches(scanner, {u"}", TOKEN_END_SEQ}));
      REQUIRE(token_matches(scanner, {u",", TOKEN_SEPARATOR}));
    }

    REQUIRE(token_matches(scanner, {u"es_ES", TOKEN_ID}));
    REQUIRE(token_matches(scanner, {u"{", TOKEN_BEG_SEQ}));
    {
      REQUIRE(token_matches(scanner, {u"game_over_dialog", TOKEN_ID}));
      REQUIRE(token_matches(scanner, {u"{", TOKEN_BEG_SEQ}));
      {
        REQUIRE(token_matches(scanner, {u"title", TOKEN_ID}));
        REQUIRE(token_matches(scanner, {u":", TOKEN_ASSIGN}));
        REQUIRE(token_matches(scanner, {u"\"Juego terminado\"", TOKEN_STRING}));
        REQUIRE(token_matches(scanner, {u",", TOKEN_SEPARATOR}));

        REQUIRE(token_matches(scanner, {u"play_again_prompt", TOKEN_ID}));
        REQUIRE(token_matches(scanner, {u":", TOKEN_ASSIGN}));
        REQUIRE(token_matches(scanner, {u"\"Juega de nuevo ?\"", TOKEN_STRING}));
        REQUIRE(token_matches(scanner, {u",", TOKEN_SEPARATOR}));

        REQUIRE(token_matches(scanner, {u"play_again_accept", TOKEN_ID}));
        REQUIRE(token_matches(scanner, {u":", TOKEN_ASSIGN}));
        REQUIRE(token_matches(scanner, {u"\"Sí\"", TOKEN_STRING}));
        REQUIRE(token_matches(scanner, {u",", TOKEN_SEPARATOR}));

        REQUIRE(token_matches(scanner, {u"play_again_refuse", TOKEN_ID}));
        REQUIRE(token_matches(scanner, {u":", TOKEN_ASSIGN}));
        REQUIRE(token_matches(scanner, {u"\"No\"", TOKEN_STRING}));

        REQUIRE(token_matches(scanner, {u"}", TOKEN_END_SEQ}));
      }

      REQUIRE(token_matches(scanner, {u"}", TOKEN_END_SEQ}));
      REQUIRE(token_matches(scanner, {u",", TOKEN_SEPARATOR}));
    }

    REQUIRE(token_matches(scanner, {u"zh_CN", TOKEN_ID}));
    REQUIRE(token_matches(scanner, {u"{", TOKEN_BEG_SEQ}));
    {
      REQUIRE(token_matches(scanner, {u"game_over_dialog", TOKEN_ID}));
      REQUIRE(token_matches(scanner, {u"{", TOKEN_BEG_SEQ}));
      {
        REQUIRE(token_matches(scanner, {u"title", TOKEN_ID}));
        REQUIRE(token_matches(scanner, {u":", TOKEN_ASSIGN}));
        REQUIRE(token_matches(scanner, {u"\"游戏结束\"", TOKEN_STRING}));
        REQUIRE(token_matches(scanner, {u",", TOKEN_SEPARATOR}));

        REQUIRE(token_matches(scanner, {u"play_again_prompt", TOKEN_ID}));
        REQUIRE(token_matches(scanner, {u":", TOKEN_ASSIGN}));
        REQUIRE(token_matches(scanner, {u"\"再玩一次 ？\"", TOKEN_STRING}));
        REQUIRE(token_matches(scanner, {u",", TOKEN_SEPARATOR}));

        REQUIRE(token_matches(scanner, {u"play_again_accept", TOKEN_ID}));
        REQUIRE(token_matches(scanner, {u":", TOKEN_ASSIGN}));
        REQUIRE(token_matches(scanner, {u"\"是的\"", TOKEN_STRING}));
        REQUIRE(token_matches(scanner, {u",", TOKEN_SEPARATOR}));

        REQUIRE(token_matches(scanner, {u"play_again_refuse", TOKEN_ID}));
        REQUIRE(token_matches(scanner, {u":", TOKEN_ASSIGN}));
        REQUIRE(token_matches(scanner, {u"\"不\"", TOKEN_STRING}));

        REQUIRE(token_matches(scanner, {u"}", TOKEN_END_SEQ}));
      }

      REQUIRE(token_matches(scanner, {u"}", TOKEN_END_SEQ}));
    }
  }
  REQUIRE(token_matches(scanner, {u"}", TOKEN_END_SEQ}));
}

#endif
