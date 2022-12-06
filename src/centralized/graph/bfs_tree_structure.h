#ifndef CENTRALIZED_CDS_GRAPH_BFS_TREE_STRUCTURE_H_
#define CENTRALIZED_CDS_GRAPH_BFS_TREE_STRUCTURE_H_

#include <set>
#include <string>

namespace centralized {

class BFSTreeStructure
{
  public:
    int nodeid = -1;
    int level = -1;
    int parent = -1;
    int root = -1;
    std::set<int> children;

  public:
    BFSTreeStructure();
    BFSTreeStructure(int nodeid, int level, int parent, int root,
                     const std::set<int> &children = {});
    BFSTreeStructure(const BFSTreeStructure &other);
    BFSTreeStructure &operator=(const BFSTreeStructure &other);
    bool equals(const BFSTreeStructure &other) const;
    std::string to_string(int log_level = 0,
                          bool log_level_on_first_line = true) const;
}; // class BFSTreeStructure

}; // namespace centralized

#endif // CENTRALIZED_CDS_GRAPH_BFS_TREE_STRUCTURE_H_