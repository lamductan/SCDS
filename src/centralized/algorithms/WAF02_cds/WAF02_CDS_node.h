#ifndef CENTRALIZED_CDS_ALGORITHMS_CDS_WAF02_CDS_WAF02_NODE_H_
#define CENTRALIZED_CDS_ALGORITHMS_CDS_WAF02_CDS_WAF02_NODE_H_

#include "centralized/graph/node.h"
#include <map>
#include <set>

namespace centralized {

struct BFSTreeStructure
{
    int nodeid = -1;
    int level = -1;
    int parent = -1;
    std::set<int> children;

    BFSTreeStructure() {}

    BFSTreeStructure(int nodeid, int level, int parent)
        : nodeid(nodeid), level(level), parent(parent)
    {
    }

    BFSTreeStructure(const BFSTreeStructure &other)
    {
        nodeid = other.nodeid;
        level = other.level;
        parent = other.parent;
        children = other.children;
    }

    BFSTreeStructure &operator=(const BFSTreeStructure &other)
    {
        nodeid = other.nodeid;
        level = other.level;
        parent = other.parent;
        children = other.children;
        return *this;
    }
};

class WAF02CDSNode : public Node
{
  public:
    BFSTreeStructure bfs_tree_structure;
    int x1 = 0;
    int x2 = 0;
    int y = 0;
    int z = 0;

    WAF02CDSNode(Node *node);
    virtual NodeType get_node_type() override;
}; // class WAF02CDSNode

}; // namespace centralized

#endif // CENTRALIZED_CDS_ALGORITHMS_CDS_WAF02_CDS_WAF02_NODE_H_