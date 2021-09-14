#ifndef SDATA_CONTEXT_HPP
#define SDATA_CONTEXT_HPP

#include <set>
#include <string>

#include "node.hpp"

namespace sdata {

class Context {
 public:
  Context(std::shared_ptr<Node> root) : m_root(root) {}

  inline std::shared_ptr<Node> root() const {
    return m_root;
  }

 private:
  std::shared_ptr<Node> m_root;
  std::set<std::string> m_namespaces;
};

}  // namespace sdata

#endif
