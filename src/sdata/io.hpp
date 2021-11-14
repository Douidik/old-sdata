#ifndef SDATA_IO_HPP
#define SDATA_IO_HPP

#include <filesystem>
#include <fstream>
#include "emitter.hpp"
#include "parser.hpp"

namespace sdata {

template <typename CharT>
std::basic_string<CharT> read_source_file(std::filesystem::path path) {
  std::basic_ifstream<CharT> stream{path};

  if (!stream.is_open()) {
    throw std::runtime_error{
        fmt<char>("Can't read source from: '%'", path.string()),
    };
  }

  return {std::istreambuf_iterator<CharT>(stream), {}};
}

template <typename CharT>
std::shared_ptr<Node> from_source(std::basic_string_view<CharT> source) {
  return Parser<CharT>{source}.parse();
}

template <typename CharT>
std::shared_ptr<Node> from_file(std::filesystem::path path) {
  return from_source<CharT>(read_source_file<CharT>(path));
}

namespace literals {
inline std::shared_ptr<Node> operator""_sdata(const char *source, size_t) {
  return from_source<char>(source);
}

inline std::shared_ptr<Node> operator""_sdata(const char16_t *source, size_t) {
  return from_source<char16_t>(source);
}

inline std::shared_ptr<Node> operator""_sdata(const char32_t *source, size_t) {
  return from_source<char32_t>(source);
}
}  // namespace literals

template <typename CharT>
std::basic_string<CharT> to_source(std::shared_ptr<Node> node) {
  std::basic_ostringstream<CharT> stream;
  Emitter<CharT>{node}.stream(stream);
  return stream.str();
}

template <typename CharT>
void to_file(std::filesystem::path path, std::shared_ptr<Node> node) {
  std::basic_ifstream<CharT> stream;
  Emitter<CharT>{node}.stream(stream);
}

}  // namespace sdata

#endif
