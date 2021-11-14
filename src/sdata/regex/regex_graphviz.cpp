#include "regex_graphviz.hpp"
#include <iomanip>
#include "regex_automata.hpp"

namespace sdata {

std::ostream &RegexGraphviz::stream(std::ostream &os) const {
  os << CREDITS << std::endl;
  os << "digraph regex_automata {" << std::endl;

  if (!m_automata.empty()) {
    stream_start(os);
    stream_shapes(os);
    stream_edges(os);
  } else {
    os << "  # Empty automata" << std::endl;
  }

  return os << "}";
}

std::string_view RegexGraphviz::parse_state(RegexNode *node) const {
  if (node->state == REGEX_EPSILON) {
    return "<$>";
  } else if (node->state == REGEX_ANY) {
    return "<^>";
  } else if (std::isspace(*node->state)) {
    return "<_>";
  } else if (!std::isprint(*node->state)) {
    return "<?>";
  } else if (*node->state == '"') {
    return "\\\"";
  } else {
    return {node->state, 1};
  }
}

std::ostream &RegexGraphviz::stream_start(std::ostream &os) const {
  os << "  rankdir = LR;" << std::endl;
  os << "  start [shape = box];" << std::endl;
  os << "  start -> 0 [label = \"" << parse_state(m_automata.root()) << "\"];" << std::endl;
  return os;
}

std::ostream &RegexGraphviz::stream_shapes(std::ostream &os) const {
  for (RegexNode *node : m_automata.nodes()) {
    std::string_view shape = node->is_leaf() ? "doublecircle" : "circle";
    os << "  " << node->id << " [shape = " << shape << "];" << std::endl;
  }

  return os;
}

std::ostream &RegexGraphviz::stream_edges(std::ostream &os) const {
  for (RegexNode *node : m_automata.nodes()) {
    for (RegexNode *edge : node->edges) {
      os << "  " << node->id << " -> " << edge->id;
      os << " [label = \"" << parse_state(edge) << "\"];" << std::endl;
    }
  }

  return os;
}

}  // namespace sdata
