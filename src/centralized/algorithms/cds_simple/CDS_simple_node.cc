#include "centralized/algorithms/cds_simple/CDS_simple_node.h"

centralized::CDSSimpleNode::CDSSimpleNode(Node *node) {
    Node::copyInformation(node->x, node->y, node->id);
}

centralized::NodeType centralized::CDSSimpleNode::get_node_type() {return CDS_SIMPLE_NODE_TYPE;}