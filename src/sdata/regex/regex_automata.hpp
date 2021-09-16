#ifndef SDATA_REGEX_AUTOMATA_HPP
#define SDATA_REGEX_AUTOMATA_HPP

#include <iostream>
#include <map>
#include <set>
#include <string>
#include <string_view>

namespace sdata {
template <typename char_t>
struct RegexNode {
  char_t state;
  std::set<size_t> edges;
};

template <typename char_t>
class RegexAutomata {
 public:
  static constexpr char_t EPSILON = 0x0;
  static constexpr char_t ANY = 0x1;

  inline bool empty() const {
    return m_nodes.empty();
  }

  size_t node_create(const char_t &state,
                     const std::set<size_t> &&ancestors,
                     const std::set<size_t> &&edges) {
    size_t id = node_new_id();
    m_nodes[id] = {state, edges};

    for (size_t ancestor_id : ancestors) {
      node_connect(ancestor_id, id);
    }

    return id;
  }

  inline void node_connect(size_t id, size_t edge) {
    m_nodes[id].edges.insert(edge);
  }

  bool node_is_leaf(size_t id) const {
    const auto &edges = m_nodes.at(id).edges;
    return edges.empty() || *std::max_element(edges.begin(), edges.end()) < id;
  }

  std::set<size_t> node_leaves(size_t id = 0) const {
    if (node_is_leaf(id)) {
      return {id};
    }

    const RegexNode<char_t> &node = m_nodes.at(id);
    std::set<size_t> leaves;

    for (size_t edge_id : node.edges) {
      if (edge_id > id) leaves.merge(node_leaves(edge_id));
    }

    return leaves;
  }

  /**
   * Runs the automata until the input reached the end or until all paths are explored
   * Returns are pair containing [matched, output]
   */
  template <typename iterator_t>
  std::pair<bool, iterator_t> run(iterator_t input, const iterator_t end, size_t id = 0) const {
    const RegexNode<char_t> &node = m_nodes.at(id);

    if (node.state == EPSILON || node.state == ANY || (input != end && node.state == *input)) {
      // Epsilon state does not consume the input
      iterator_t edge_input = node.state != EPSILON ? input + 1 : input;

      for (size_t edge_id : node.edges) {
        if (auto [edge_matched, edge_output] = run(edge_input, end, edge_id); edge_matched) {
          // Path available in edges, we accept the input
          return {true, edge_output};
        }
      }

      // ANY node can't be an accepting state
      if (node.state != ANY && node_is_leaf(id)) {
        // Automata reached the end, we accept the input
        return {true, edge_input};
      }
    }

    // No path available, we reject the input
    return {false, input};
  }

  /**
   * Merges an automata inside this one
   * Connects the ancestors to the automata's root
   * Returns the id of the merged automata's root
   */
  size_t merge(const RegexAutomata<char_t> &automata, const std::set<size_t> &&ancestors) {
    std::map<size_t, RegexNode<char_t>> merged;
    const size_t root_id = node_new_id();

    for (const auto &[id, node] : automata.m_nodes) {
      RegexNode<char_t> merged_node {node.state};

      for (size_t edge_id : node.edges) {
        merged_node.edges.insert(root_id + edge_id);
      }

      merged[root_id + id] = merged_node;
    }

    for (size_t ancestor_id : ancestors) {
      node_connect(ancestor_id, {root_id});
    }

    m_nodes.merge(merged);
    return root_id;
  }

  std::ostream &stream_graphviz(std::ostream &os) const {
    os << "digraph regex_automata {\n";
    if (!empty()) {
      stream_graphviz_start(os);
      stream_graphviz_shapes(os);
      stream_graphviz_edges(os);
    }
    os << "}";

    return os;
  }

 private:
  std::string graphviz_state(const RegexNode<char_t> &node) const {
    if (node.state == EPSILON) return "EPS";
    if (node.state == ANY) return "ANY";
    if (node.state == '"') return "\\\"";
    if (node.state >= 0x20 && node.state <= 0x7E) return {(char)node.state};

    return "???";
  }

  std::ostream &stream_graphviz_start(std::ostream &os) const {
    const RegexNode<char_t> &root_node = m_nodes.at(0);

    os << "\trankdir=\"LR\";\n";
    os << "\tstart [shape = box];\n";
    os << "\tstart -> 0 [label = \"" << graphviz_state(root_node) << "\"];\n";

    return os;
  }

  std::ostream &stream_graphviz_shapes(std::ostream &os) const {
    for (const auto &[id, node] : m_nodes) {
      std::string shape = node_is_leaf(id) ? "doublecircle" : "circle";
      os << '\t' << id << " [shape = " << shape << "];\n";
    }

    return os;
  }

  std::ostream &stream_graphviz_edges(std::ostream &os, size_t id = 0) const {
    for (const auto &[id, node] : m_nodes) {
      for (size_t edge_id : node.edges) {
        const auto &edge = m_nodes.at(edge_id);
        os << '\t' << id << " -> " << edge_id << " [label = \"" << graphviz_state(edge) << "\"];\n";
      }
    }

    return os;
  }

  inline size_t node_new_id() const {
    return !m_nodes.empty() ? m_nodes.cend()->first : 0;
  }

  std::map<size_t, RegexNode<char_t>> m_nodes;
};

template <typename char_t>
inline std::ostream &operator<<(std::ostream &os, const RegexAutomata<char_t> &automata) {
  return automata.stream_graphviz(os);
}

}  // namespace sdata

#endif
