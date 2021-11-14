#ifndef SDATA_EMITTER_HPP
#define SDATA_EMITTER_HPP

#include <iomanip>
#include <sstream>
#include "emitter_config.hpp"
#include "misc/any_of.hpp"
#include "misc/string.hpp"
#include "node.hpp"

namespace sdata {

class EmitterException : std::exception {
  static constexpr std::string_view PATTERN =
      "[sdata::EmitterException raised]: %\n"
      "with { % }";

 public:
  EmitterException(std::string_view description, std::shared_ptr<Node> node)
      : m_buffer{fmt<char>(PATTERN, description, *node)}, m_node(node) {}

  inline const char *what() const noexcept override {
    return m_buffer.data();
  }

  inline std::shared_ptr<Node> node() const {
    return m_node;
  }

 private:
  std::string m_buffer;
  std::shared_ptr<Node> m_node;
};

template <typename CharT>
class Emitter {
  using StreamT = std::basic_ostream<CharT>;

 public:
  Emitter(std::shared_ptr<Node> node, EmitterConfig config = DEFAULT_EMITTER_CONFIG)
      : m_root(node), m_config(config) {}

  StreamT &stream(StreamT &stream) {
    return stream_node(stream, m_root, 0);
  }

 private:
  StreamT &stream_node(StreamT &stream, std::shared_ptr<Node> node, unsigned depth) {
    stream << string::convert<char, CharT>(node->id());

    return node->type() == Node::SEQUENCE ? stream_sequence(stream, node, depth)
                                          : stream_data(stream, node);
  }

  StreamT &stream_sequence(StreamT &stream, std::shared_ptr<Node> node, unsigned depth) {
    unsigned member_depth = node->is_anonymous() ? depth : depth + 1;

    if (!node->is_anonymous()) {
      if (m_config.style & STYLE_BREAK_BEFORE_BRACES) stream << CharT{'\n'};
      if (m_config.style & STYLE_SPACE_BEFORE_BRACES) stream << CharT{' '};
    }

    stream << CharT{'{'};

    if (node->is_anonymous() && m_config.style & STYLE_BREAK_ANONYMOUS_BRACES) {
      stream << CharT{'\n'};
    }

    if (m_config.style & STYLE_BREAK_AFTER_BRACES) stream << CharT{'\n'};
    if (m_config.style & STYLE_SPACE_AFTER_BRACES) stream << CharT{' '};

    for (const auto &sequence = node->as<Sequence>(); auto member : sequence) {
      if (!node->is_anonymous()) {
        stream_indent(stream, member_depth);
      }

      stream_node(stream, member, member_depth);

      if (member != sequence.back()) {
        stream << CharT{','};
      }

      if (m_config.style & STYLE_SPACE_AFTER_SEPARATOR) stream << CharT{' '};
      if (m_config.style & STYLE_BREAK_AFTER_SEPARATOR) stream << CharT{'\n'};
    }

    if (!node->is_anonymous()) {
      stream_indent(stream, depth);
    } else {
      if (m_config.style & STYLE_BREAK_ANONYMOUS_BRACES) stream << CharT{'\n'};
    }

    return stream << CharT{'}'};
  }

  StreamT &stream_data(StreamT &stream, std::shared_ptr<Node> node) {
    if (m_config.style & STYLE_SPACE_BEFORE_ASSIGN) stream << CharT{' '};
    stream << CharT{':'};
    if (m_config.style & STYLE_SPACE_AFTER_ASSIGN) stream << CharT{' '};

    std::visit(
        [&stream, &node](const auto &data) {
          using T = std::decay_t<decltype(data)>;

          if constexpr (any_of<T, int, float>) {
            stream << data;
          }
          if constexpr (any_of<T, bool>) {
            stream << std::boolalpha << data;
          }
          if constexpr (any_of<T, std::string, std::u16string, std::u32string>) {
            auto string = string::convert<typename T::value_type, CharT>(data);
            stream << CharT{'"'} << string << CharT{'"'};
          }
          if constexpr (any_of<T, char, char16_t, char32_t>) {
            stream << CharT{'\''} << static_cast<CharT>(data) << CharT{'\''};
          }
          if constexpr (any_of<T, std::nullptr_t>) {
            throw EmitterException{"Node of type 'nil' can't be emitted", node};
          }
        },
        node->variant());

    return stream;
  }

  StreamT &stream_indent(StreamT &stream, std::size_t width) {
    for (std::size_t i = 0; i < width; i++) {
      stream << string::convert<char, CharT>(m_config.indent);
    }
    return stream;
  }

  std::shared_ptr<Node> m_root;
  EmitterConfig m_config;
};

}  // namespace sdata

#endif
