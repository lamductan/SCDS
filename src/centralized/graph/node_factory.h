#ifndef CENTRALIZED_CDS_GRAPH_NODE_FACTORY_H_
#define CENTRALIZED_CDS_GRAPH_NODE_FACTORY_H_

#include "centralized/graph/node.h"

namespace centralized {

class NodeFactory {
public:
    static Node *create_node(Node *node, NodeType node_type);
};

}; //namespace centralized

#endif //CENTRALIZED_CDS_GRAPH_NODE_FACTORY_H_