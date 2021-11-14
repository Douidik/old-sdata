#ifndef SDATA_PARSER_TEST_HPP
#define SDATA_PARSER_TEST_HPP

#include <catch2/catch.hpp>
#include <sdata.hpp>
#include <sstream>

using namespace sdata;

TEST_CASE("Parser<char>") {
  auto game = std::make_shared<Node>("tetris", Sequence{});
  {
    game->emplace("window/width", 1920);
    game->emplace("window/height", 1080);
    game->emplace("window/title", "Tetris game");
    game->emplace("window/fullscreen", false);
  }
  {
    game->emplace("controls/left", 'a');
    game->emplace("controls/right", 'd');
    game->emplace("controls/confirm", 'e');
    game->emplace("controls/pause", 'p');
  }

  REQUIRE(*game == *from_file<char>("examples/game.sd"));
}

TEST_CASE("Parser<char16_t>") {
  auto dialog = std::make_shared<Node>("", Sequence{});

  auto english = dialog->emplace("en_US/game_over_dialog", Sequence{});
  {
    english->emplace("title", u"Game over");
    english->emplace("play_again_prompt", u"Play again ?");
    english->emplace("play_again_accept", u"Yes");
    english->emplace("play_again_refuse", u"No");
  }

  auto french = dialog->emplace("fr_FR/game_over_dialog", Sequence{});
  {
    french->emplace("title", u"Partie terminée");
    french->emplace("play_again_prompt", u"Souhaitez-vous rejouer ?");
    french->emplace("play_again_accept", u"Oui");
    french->emplace("play_again_refuse", u"Non");
  }

  auto spanish = dialog->emplace("es_ES/game_over_dialog", Sequence{});
  {
    spanish->emplace("title", u"Juego terminado");
    spanish->emplace("play_again_prompt", u"Juega de nuevo ?");
    spanish->emplace("play_again_accept", u"Sí");
    spanish->emplace("play_again_refuse", u"No");
  }

  auto chinese = dialog->emplace("zh_CN/game_over_dialog", Sequence{});
  {
    chinese->emplace("title", u"游戏结束");
    chinese->emplace("play_again_prompt", u"再玩一次 ？");
    chinese->emplace("play_again_accept", u"是的");
    chinese->emplace("play_again_refuse", u"不");
  }

  REQUIRE(*dialog == *from_file<char16_t>("examples/dialog.sd"));
}

#endif
