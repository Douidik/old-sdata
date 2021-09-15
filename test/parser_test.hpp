#ifndef SDATA_PARSER_TEST_HPP
#define SDATA_PARSER_TEST_HPP

#include <catch2/catch_all.hpp>
#include <fstream>
#include <memory>
#include <sdata/parse/parser.hpp>

template <typename stream_t, typename T, typename = void>
struct is_streamable : std::false_type {};

template <typename stream_t, typename T>
struct is_streamable<stream_t,
                     T,
                     std::void_t<decltype(std::declval<stream_t &>() << std::declval<T>())>>
    : std::true_type {};

template <typename stream_t, typename T>
constexpr bool is_streamable_v = is_streamable<stream_t, T>();

void stream_hierarchy(std::ostream &stream,
                               std::shared_ptr<sdata::Node> node,
                               size_t depth = 0) {
  stream << "'" << node->id() << "': ";

  if (!node->members().empty()) {
    stream << '{' << '\n';
    for (const auto &member : node->members()) {
      stream << std::string(depth * 4, ' ');
      stream_hierarchy(stream, member, depth + 1);
      stream << ',' << '\n';
    }
    stream << "}," << '\n';
  } else {
    std::visit(
        [&stream](const auto &data) {
          using T = std::decay_t<decltype(data)>;
          if constexpr (is_streamable_v<std::ostream, T>) {
            stream << data;
          } else {
            stream << "???";
          }
        },
        node->variant());
  }
}

TEST_CASE("yeet case") {
  std::ifstream ifstream("examples/game.sdat");

  std::string source(std::istreambuf_iterator<char>(ifstream), {});
  sdata::Parser<char> parser(source);

  try {
    std::shared_ptr<sdata::Node> root = parser.parse();
    stream_hierarchy(std::cout, root);
  } catch (std::bad_weak_ptr exception) {
    std::cout << exception.what() << std::endl;
  }
}

#endif
