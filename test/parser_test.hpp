#ifndef SDATA_PARSER_TEST_HPP
#define SDATA_PARSER_TEST_HPP

#include <catch2/catch_all.hpp>
#include <fstream>
#include <memory>
#include <sdata/parser.hpp>

template <typename T>
concept streamable = requires(std::ostream os, T x) {
  {os << x};
};

template <typename T>
concept is_string = std::is_same_v<T, std::basic_string<typename T::value_type>>;

template <typename T>
concept is_char_convertible = requires(T x) {
  { x } -> std::convertible_to<char>;
};

void stream_hierarchy(std::ostream &stream, std::shared_ptr<sdata::Node> node, size_t depth = 0) {
  stream << "'" << node->id() << "'";

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

          stream << " <" << sdata::type_to_string<T>() << ">: ";

          if constexpr (streamable<T>) {
            stream << data;
          } else if constexpr (is_string<T>) {
            stream << std::string {data.begin(), data.end()};
          } else if constexpr (is_char_convertible<T>) {
            stream << static_cast<char>(data);
          } else {
            stream << "undefined";
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
