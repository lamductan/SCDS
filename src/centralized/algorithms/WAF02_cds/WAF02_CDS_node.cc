#include "centralized/algorithms/WAF02_cds/WAF02_CDS_node.h"

centralized::WAF02CDSNode::WAF02CDSNode(Node *node) {
    Node::copyInformation(node->x, node->y, node->id);
}

centralized::NodeType centralized::WAF02CDSNode::get_node_type() {return CDS_WAF02_NODE_TYPE;}