#include "centralized/graph/node_factory.h"
#include "centralized/algorithms/WAF02_cds/WAF02_CDS_node.h"
#include "centralized/algorithms/cds_simple/CDS_simple_node.h"

centralized::Node *
centralized::NodeFactory::create_node(centralized::Node *node,
                                      centralized::NodeType node_type)
{
    if (node_type == CDS_DEFAULT_NODE_TYPE) {
        return node->dup();
    } else if (node_type == CDS_SIMPLE_NODE_TYPE) {
        return new CDSSimpleNode(node);
    } else if (node_type == CDS_GK96_NODE_TYPE) {
        return node->dup();
    } else if (node_type == CDS_WAF02_NODE_TYPE) {
        return new WAF02CDSNode(node);
    }
    return nullptr;
}