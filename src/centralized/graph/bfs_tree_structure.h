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
    std::set<int> children;

  public:
    BFSTreeStructure();
    BFSTreeStructure(int nodeid, int level, int parent,
                     const std::set<int> &children = {});
    BFSTreeStructure(const BFSTreeStructure &other);
    BFSTreeStructure &operator=(const BFSTreeStructure &other);
    bool equals(const BFSTreeStructure &other) const;
    std::string to_string(int log_level = 0) const;
}; // class BFSTreeStructure

}; // namespace centralized

#endif // CENTRALIZED_CDS_GRAPH_BFS_TREE_STRUCTURE_H_