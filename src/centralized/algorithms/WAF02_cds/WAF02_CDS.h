#ifndef CENTRALIZED_CDS_ALGORITHMS_CDS_WAF02_CDS_WAF02_H_
#define CENTRALIZED_CDS_ALGORITHMS_CDS_WAF02_CDS_WAF02_H_

#include "centralized/algorithms/WAF02_cds/WAF02_CDS_node.h"
#include "centralized/algorithms/cds_simple/CDS_simple.h"
#include "centralized/graph/graph.h"
#include <map>
#include <vector>

namespace centralized {

class WAF02CDSAlg : public CDSSimpleAlg
{
  public:
    std::map<int, WAF02CDSNode *> WAF02_cds_nodes;
    int root = 0;

    WAF02CDSAlg();
    WAF02CDSAlg(std::map<int, Node *> nodes, std::set<Edge> edges);

    std::map<int, BFSTreeStructure> bfs_tree_structures;
    std::map<int, BFSTreeStructure> find_bfs_tree_structure_from_root(int root);
    virtual std::vector<int> find_mis(bool print_undediced_node = false);
    int find_root_T_star(int root);
    int count_mis_neighbor(WAF02CDSNode *node);
    virtual std::set<int>
    find_cds_nodes_from_mis_nodes(const std::vector<int> &mis_nodes);
}; // class WAF02CDSAlg

}; // namespace centralized

#endif // CENTRALIZED_CDS_ALGORITHMS_CDS_WAF02_CDS_WAF02_H_