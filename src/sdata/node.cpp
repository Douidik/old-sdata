#include "node.hpp"
#include <iomanip>
#include "misc/fmt.hpp"
#include "token.hpp"

namespace sdata {

NodeException::NodeException(std::string_view description, std::shared_ptr<const Node> node)
    : m_buffer{fmt<char>(PATTERN, description, *node)}, m_node(node) {}

std::shared_ptr<Node> Node::emplace(std::string_view path, Variant data) {
  auto token = parse_path_token(path);

  if (path.empty()) {
    return emplace(std::make_shared<Node>(token, data));
  }

  for (auto sequence = as<Sequence>(); auto member : sequence) {
    if (member->id() == token) return member->emplace(path, data);
  }

  return emplace(std::make_shared<Node>(token, Sequence{}))->emplace(path, data);
}

std::shared_ptr<const Node> Node::at(std::string_view path) const {
  if (path.empty()) return shared_from_this();

  auto token = parse_path_token(path);

  for (auto sequence = as<Sequence>(); auto member : sequence) {
    if (member->id() == token) return at(path);
  }

  return {};
}

std::string_view Node::parse_path_token(std::string_view &path) const {
  std::string_view token{};

  if (size_t length = path.find('/'); length != path.npos) {
    token = path.substr(0, length);
    path.remove_prefix(length + 1);
  } else {
    token = path;
    path = "";
  }

  return token;
}

std::ostream &operator<<(std::ostream &os, const Node &node) {
  constexpr std::string_view PATTERN = "node<%> '%' : %";
  std::stringstream data_stream{};

  std::visit(
      [&data_stream](const auto &data) {
        using T = std::decay_t<decltype(data)>;

        if constexpr (std::is_same_v<T, Sequence>) {
          data_stream << '[';
          for (auto m : data) data_stream << std::quoted(m->id(), '\'') << ',';
          data_stream << ']';
        } else if constexpr (streamable<T, char>) {
          data_stream << data;
        } else {
          data_stream << "<?>";  // unstreamable
        }
      },
      node.variant());

  return os << fmt<char>(PATTERN, Node::type_name(node.type()), node.id(), data_stream.str());
}

bool operator==(const Node &a, const Node &b) {
  if (a.id() == b.id() && a.type() == b.type()) {
    Node::Type type = a.type();

    if (type == Node::SEQUENCE) {
      const auto &a_members = a.as<Sequence>(), &b_members = b.as<Sequence>();
      return std::ranges::equal(a_members, b_members, [](auto a, auto b) { return *a == *b; });
    } else {
      return a.variant() == b.variant();
    }
  } else {
    return false;
  }
}

}  // namespace sdata
