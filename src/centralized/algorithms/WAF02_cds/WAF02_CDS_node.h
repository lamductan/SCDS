#ifndef CENTRALIZED_CDS_ALGORITHMS_CDS_WAF02_CDS_WAF02_NODE_H_
#define CENTRALIZED_CDS_ALGORITHMS_CDS_WAF02_CDS_WAF02_NODE_H_

#include "centralized/graph/bfs_tree_structure.h"
#include "centralized/graph/node.h"
#include <map>
#include <set>

namespace centralized {

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