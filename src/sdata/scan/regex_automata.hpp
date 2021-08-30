#ifndef REGEX_AUTOMATA_HPP
#define REGEX_AUTOMATA_HPP

#include <algorithm>
#include <iostream>
#include <limits>
#include <map>
#include <string_view>
#include <unordered_set>
#include <vector>

namespace sdata {

using RegexNode = size_t;

template <typename char_t>
class RegexAutomata {
  using string_view_t = std::basic_string_view<char_t>;

 public:
  static constexpr char_t EPSILON = 0x0;
  static constexpr char_t ANY = 0x1;

  RegexAutomata() {
  }

  RegexAutomata<char_t> merge(RegexAutomata<char_t>& automata,
                              std::unordered_set<RegexNode> ancestors) {
    RegexAutomata<char_t> rebased{};
    const RegexNode base = node_count();

    for (const auto& [node, state] : automata.m_states) {
      rebased.m_states[base + node] = state;
    }

    for (const auto& [node, edges] : automata.m_transitions) {
      auto& node_edges = rebased.m_transitions[base + node];

      for (RegexNode edge : edges) {
        node_edges.insert(base + edge);
      }
    }

    for (RegexNode ancestor : ancestors) {
      m_transitions[ancestor].insert(base);
    }

    m_states.merge(rebased.m_states);
    m_transitions.merge(rebased.m_transitions);

    return rebased;
  }

  inline RegexNode node_count() const {
    return m_states.empty() ? 0 : m_states.end()->first;
  }

  inline bool empty() const {
    return node_count() <= 0;
  }

  inline const char_t& node_state(RegexNode node) const {
    return m_states.at(node);
  }

  inline const std::unordered_set<RegexNode>& node_edges(RegexNode node) const {
    return m_transitions.at(node);
  }

  RegexNode node_create(const char_t& state, std::unordered_set<RegexNode>&& ancestors,
                        std::unordered_set<RegexNode>&& edges) {
    const RegexNode node = node_count();

    m_states[node] = state;
    m_transitions[node] = edges;

    for (RegexNode ancestor : ancestors) {
      node_connect(ancestor, {node});
    }

    return node;
  }

  inline void node_connect(RegexNode node, std::unordered_set<RegexNode>&& edges) {
    m_transitions[node].merge(edges);
  }

  bool node_is_leaf(RegexNode node) const {
    const auto& edges = m_transitions.at(node);
    return edges.empty() || *std::max_element(edges.begin(), edges.end()) < node;
  }

  std::unordered_set<RegexNode> leafs() const {
    std::unordered_set<RegexNode> leafs{};

    for (const auto& [node, _] : m_transitions) {
      if (node_is_leaf(node)) leafs.emplace(node);
    }

    return leafs;
  }

  // Runs the automata until the input reached the end or until all paths have been explored
  // Returns [accepted, output]
  template <typename Iterator>
  std::pair<bool, const Iterator> run(Iterator input, const Iterator end,
                                      RegexNode node = 0) const {
    if (node_is_leaf(node)) {
      return {true, input};
    }

    if (node_state(node) == EPSILON || node_state(node) == ANY || node_state(node) == *input) {
      // Epsilon state doesn't consume input
      Iterator edge_input = node_state(node) != EPSILON ? input + 1 : input;

      for (RegexNode edge : m_transitions.at(node)) {
        auto [edge_accepted, edge_output] = run(edge_input, end, edge);

        if (edge_accepted) {
          return {true, edge_output};  // Path available, we return the success
        }
      }
    }

    // No path available
    return {false, input};
  }

  // Translate the regex graph into graphviz code
  std::ostream& graphviz(std::ostream& os, RegexNode node = 0) const {
    const auto graphviz_state = [this](RegexNode node) -> char {
      char_t state = m_states.at(node);
      return state != EPSILON ? state : '$';
    };

    if (node == 0) {
      os << "rankdir=\"LR\";" << std::endl;
      os << "start [shape = box];" << std::endl;
      os << "start -> 0 [label = \"" << graphviz_state(node) << "\"];" << std::endl;
    }

    std::string shape = node_is_leaf(node) ? "doublecircle" : "circle";
    os << node << " [shape = " << shape << "];" << std::endl;

    for (RegexNode edge : node_edges(node)) {
      char state = graphviz_state(edge);

      os << node << " -> " << edge << " [label = \"" << state << "\"];" << std::endl;

      // Avoid infinite recursion by reparsing nodes (loop case)
      if (edge > node) graphviz(os, edge);
    }

    return os;
  }

 private:
  std::map<RegexNode, char_t> m_states;
  std::map<RegexNode, std::unordered_set<RegexNode>> m_transitions;
};

template <typename char_t>
std::ostream& operator<<(std::ostream& os, const RegexAutomata<char_t>& automata) {
  return automata.graphviz(os);
}

}  // namespace sdata

#endif
