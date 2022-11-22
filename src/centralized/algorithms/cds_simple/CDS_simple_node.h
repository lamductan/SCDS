#ifndef CENTRALIZED_ALGORITHMS_CDS_SIMPLE_CDS_SIMPLE_NODE_H_
#define CENTRALIZED_ALGORITHMS_CDS_SIMPLE_CDS_SIMPLE_NODE_H_

#include <map>
#include "centralized/graph/node.h"

namespace centralized {

class CDSSimpleNode : public Node {
public:
    std::map<Node *, std::array<Node *, 2>> three_hop_neighbors;
    std::map<Node *, std::array<Node *, 2>> two_three_hop_neighbors;

    CDSSimpleNode(Node *node);
    virtual NodeType get_node_type() override;
}; //class CDSSimpleNode

}; //namespace centralized

#endif //ALGORITHMS_CDS_SIMPLE_CDS_SIMPLE_NODE_H_