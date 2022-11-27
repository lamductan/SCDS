#ifndef CENTRALIZED_CDS_ALGORITHMS_CDS_SIMPLE_CDS_SIMPLE_H_
#define CENTRALIZED_CDS_ALGORITHMS_CDS_SIMPLE_CDS_SIMPLE_H_

#include "centralized/graph/graph.h"
#include <vector>

namespace centralized {

class CDSSimpleAlg
{
  private:
    bool rule1_condition1(int u, int v);
    bool rule1_condition2(int u, int v);

    bool rule2_condition1(int u, int v, int w);
    bool rule2_condition2(int u, int v, int w);

    bool rule3_condition1(int u, int x, int y, int z);
    bool rule3_condition2(int u, int x, int y, int z);

  public:
    Graph *graph;

    CDSSimpleAlg();
    CDSSimpleAlg(std::map<int, Node *> nodes, std::set<Edge> edges);
    virtual std::vector<int> find_mis(bool print_undediced_node = false);
    bool check_all_nodes_decided_mis_status(bool print_undediced_node = false);

    void find_three_hop_neighbors();

    virtual std::set<int>
    find_cds_nodes_from_mis_nodes(const std::vector<int> &mis_nodes);
    virtual std::vector<int> find_cds(bool apply_rule = false);

    std::set<int> apply_rule_1(const std::set<int> &cds_nodes);
    std::set<int> apply_rule_2(const std::set<int> &cds_nodes);

    virtual ~CDSSimpleAlg();
}; // class CDSSimple

}; // namespace centralized

#endif // CENTRALIZED_CDS_ALGORITHMS_CDS_SIMPLE_CDS_SIMPLE_H_