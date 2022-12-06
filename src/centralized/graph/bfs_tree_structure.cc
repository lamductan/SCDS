#include "centralized/graph/bfs_tree_structure.h"
#include "utils/utils.h"

centralized::BFSTreeStructure::BFSTreeStructure() {}

centralized::BFSTreeStructure::BFSTreeStructure(int nodeid, int level,
                                                int parent, int root,
                                                const std::set<int> &children)
    : nodeid(nodeid), level(level), parent(parent), root(root),
      children(children)
{
}

centralized::BFSTreeStructure::BFSTreeStructure(
    const centralized::BFSTreeStructure &other)
{
    nodeid = other.nodeid;
    level = other.level;
    parent = other.parent;
    root = other.root;
    children = other.children;
}

centralized::BFSTreeStructure &centralized::BFSTreeStructure::operator=(
    const centralized::BFSTreeStructure &other)
{
    nodeid = other.nodeid;
    level = other.level;
    parent = other.parent;
    root = other.root;
    children = other.children;
    return *this;
}

bool centralized::BFSTreeStructure::equals(
    const centralized::BFSTreeStructure &other) const
{
    if (nodeid != other.nodeid) return false;
    if (level != other.level) return false;
    if (parent != other.parent) return false;
    if (root != other.root) return false;
    if (children != other.children) return false;
    return true;
}

std::string
centralized::BFSTreeStructure::to_string(int log_level,
                                         bool log_level_on_first_line) const
{
    std::string one_tab = std::string(4, ' ');
    std::string tab = std::string(log_level * 4, ' ');
    std::string s = (log_level_on_first_line) ? tab : "";
    s += "BFSTreeStructure = {\n";
    s += (tab + one_tab + "nodeid   = " + std::to_string(nodeid) + ",\n");
    s += (tab + one_tab + "level    = " + std::to_string(level) + ",\n");
    s += (tab + one_tab + "parent   = " + std::to_string(parent) + ",\n");
    s += (tab + one_tab + "root     = " + std::to_string(root) + ",\n");
    s += (tab + one_tab + "children = " + set_to_string<int>(children) + ",\n");
    return (s += tab + "}");
}
