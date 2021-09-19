#ifndef SDATA_NODE_HPP
#define SDATA_NODE_HPP

#include <memory>
#include <unordered_set>
#include <variant>
#include <vector>
#include <string_view>

#include "misc/exception.hpp"

namespace sdata {

class Node;

class NodeException : public Exception {
 public:
  NodeException(std::string_view description, std::shared_ptr<Node> node);

  inline std::shared_ptr<Node> node() const {
    return m_node;
  }

  inline std::string_view name() const override {
    return "NodeException";
  }

 private:
  std::shared_ptr<Node> m_node;
};

class Node : public std::enable_shared_from_this<Node> {
 public:
  using variant_t = std::variant<  // clang-format off
                                 std::nullptr_t,
                                 float,
                                 int,
                                 bool,
                                 std::string, char,
                                 std::wstring, wchar_t,
                                 std::u8string, char8_t,
                                 std::u16string, char16_t,
                                 std::u32string, char32_t
                                >;  // clang-format on

  Node(std::string_view id, auto data, std::unordered_set<std::string_view> context = {})
      : m_identifier(id),
        m_variant(data),
        m_invisible {id.starts_with('@') && context.contains(id)} {}

  inline std::string_view id() const {
    return m_identifier;
  }

  inline const variant_t &variant() const {
    return m_variant;
  }

  auto &operator=(auto data) {
    using data_t = std::decay_t<decltype(data)>;

    m_variant = data;
    return std::get<data_t>(m_variant);
  }

  template <typename T>
  inline T &as(T &&default_data) {
    T *data = std::get_if<T>(m_variant);
    return data != nullptr ? *data : default_data;
  }

  template <typename T>
  inline T *as() const {
    return std::get_if<T>(m_variant);
  }

  template <typename T>
  inline bool is() {
    return std::holds_alternative<T>(m_variant);
  }

  std::shared_ptr<Node> add_member(std::shared_ptr<Node> member) {
    member->m_owner = shared_from_this();
    return m_members.emplace_back(member);
  }

  inline auto members() const {
    return m_members;
  }

  inline void merge(std::shared_ptr<Node> other) {
    m_members.insert(m_members.end(), other->m_members.begin(), other->m_members.end());
  }

  inline std::shared_ptr<Node> operator[](std::string_view path) {
    return find(shared_from_this(), path);
  }

  inline std::shared_ptr<const Node> operator[](std::string_view path) const {
    return find(shared_from_this(), path);
  }

  inline bool exists(std::string_view path) const {
    return find(shared_from_this(), path) != nullptr;
  }

  inline std::shared_ptr<Node> owner() const {
    return !m_owner.expired() ? m_owner.lock() : nullptr;
  }

  std::shared_ptr<Node> root() const {
    std::shared_ptr<Node> node;
    while (node) node = node->owner();

    return node;
  }

 private:
  template <typename node_t>
  static std::shared_ptr<node_t> find(std::shared_ptr<node_t> node, std::string_view path) {
    if (path.empty()) {  // Node found
      return node;
    }

    auto separator_itr = std::find(path.begin(), path.end(), '/');
    std::string_view step = {path.begin(), separator_itr};
    std::string_view rest = {separator_itr != path.end() ? separator_itr + 1 : path.end(),
                             path.end()};

    // Operators
    if (step == "\\")  // root operator
      return find<node_t>(node->root(), rest);
    else if (step == ".")  // this operator
      return find<node_t>(node, rest);
    else if (step == ".." && !node->m_owner.expired())  // owner operator
      return find<node_t>(node->m_owner.lock(), rest);
    else [[likely]] {
      for (auto member : node->members())
        if (member->id() == step) return find<node_t>(member, rest);

      // Force member creation for the non-const version
      if constexpr (!std::is_const_v<node_t>) {
        auto member = std::make_shared<Node>(std::string {step}, nullptr);
        node->add_member(member);
        return find<node_t>(member, rest);
      }
    }

    return nullptr;  // member not found
  }

  bool m_invisible;
  std::string m_identifier;
  variant_t m_variant;
  std::weak_ptr<Node> m_owner;
  std::vector<std::shared_ptr<Node>> m_members;
};

}  // namespace sdata

#endif
