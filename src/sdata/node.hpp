#ifndef SDATA_NODE_HPP
#define SDATA_NODE_HPP

#include <exception>
#include <memory>
#include <string_view>
#include <unordered_set>
#include <variant>
#include "misc/fmt.hpp"

namespace sdata {

class Node;

class NodeException : std::exception {
  static constexpr std::string_view PATTERN =
      "[sdata::NodeException raised]: %\n"
      "with { % }";

 public:
  NodeException(std::string_view description, std::shared_ptr<const Node> node);

  inline const char *what() const noexcept override {
    return m_buffer.data();
  }

  inline std::shared_ptr<const Node> node() const {
    return m_node;
  }

 private:
  std::string m_buffer;
  std::shared_ptr<const Node> m_node;
};

using Sequence = std::vector<std::shared_ptr<Node>>;

using Variant = std::variant<std::nullptr_t,
                             Sequence,
                             float,
                             int,
                             bool,
                             char,
                             char16_t,
                             char32_t,
                             std::string,
                             std::u16string,
                             std::u32string>;

class Node : public std::enable_shared_from_this<Node> {
 public:
  enum Type : size_t {
    NIL,
    SEQUENCE,
    FLOAT,
    INT,
    BOOL,
    CHAR,
    CHAR_UTF16,
    CHAR_UTF32,
    STRING,
    STRING_UTF16,
    STRING_UTF32,
  };

  template <typename CharT>
  Node(std::basic_string_view<CharT> id, Variant data)
      : m_identifier{id.begin(), id.end()}, m_variant(data) {}

  Node(std::string_view id, Variant data) : m_identifier(id), m_variant(data) {}

  inline Type type() const {
    return static_cast<Type>(m_variant.index());
  }

  inline std::string_view id() const {
    return m_identifier;
  }

  inline const Variant &variant() const {
    return m_variant;
  }

  inline bool is_anonymous() const {
    return m_identifier.empty();
  }

  inline auto &assign(Variant data) {
    return (m_variant = data);
  }

  inline auto &operator=(Variant data) {
    return assign(data);
  }

  template <typename T>
  T &as() {
    assert_type<T>();
    return std::get<T>(m_variant);
  }

  template <typename T>
  const T &as() const {
    assert_type<T>();
    return std::get<T>(m_variant);
  }

  template <typename T>
  inline bool is() const {
    return std::holds_alternative<T>(m_variant);
  }

  inline std::shared_ptr<Node> emplace(std::shared_ptr<Node> member) {
    return as<Sequence>().emplace_back(member);
  }

  std::shared_ptr<Node> emplace(std::string_view path, Variant data);

  static constexpr std::string_view type_name(Type type) {
    switch (type) {
      case SEQUENCE: return "sequence";
      case FLOAT: return "float";
      case INT: return "int";
      case BOOL: return "bool";
      case CHAR: return "char";
      case CHAR_UTF16: return "char-utf16";
      case CHAR_UTF32: return "char-utf32";
      case STRING: return "string";
      case STRING_UTF16: return "string-utf16";
      case STRING_UTF32: return "string-utf32";
      case NIL: return "nil";
      default: return "";
    }
  }

  std::shared_ptr<const Node> at(std::string_view path) const;

 private:
  std::string_view parse_path_token(std::string_view &path) const;

  template <typename T>
  void assert_type() const {
    if (!is<T>()) {
      auto error = fmt<char>("Node does not contain data of type <%>", type_name(type()));
      throw NodeException(error, shared_from_this());
    }
  }

  std::string m_identifier;
  Variant m_variant;
};

// Debug stream
std::ostream &operator<<(std::ostream &os, const Node &node);

// Recursive node content comparison
bool operator==(const Node &a, const Node &b);

}  // namespace sdata

#endif
