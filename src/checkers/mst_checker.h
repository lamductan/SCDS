#ifndef SCDS_CHECKERS_MST_CHECKER_H_
#define SCDS_CHECKERS_MST_CHECKER_H_

#include "centralized/graph/graph.h"
#include "checkers/ichecker.h"
#include <vector>

class MSTChecker : public IChecker
{
  private:
    std::set<int> selected_nodes_set;
    std::map<int, centralized::Node *> selected_nodes_map;
    std::map<int, centralized::Node *> all_nodes_map;

    centralized::Graph *subgraph_from_original_graph_edges = nullptr;
    centralized::Graph *subgraph_from_alg_edges = nullptr;
    centralized::Graph *original_graph = nullptr;

    centralized::Graph *construct_subgraph_from_original_graph();
    centralized::Graph *construct_subgraph_from_alg();
    centralized::Graph *construct_original_graph();
    int minimal_weight_tree_of_subgraph_from_alg = -1;
    int minimal_weight_tree_of_subgraph_from_original_graph = -1;

  public:
    MSTChecker(Network *network);
    bool check_connected(bool is_final_check) const;
    virtual bool check(bool is_final_check = true) const;
    virtual ~MSTChecker();
};

#endif // SCDS_CHECKERS_MST_CHECKER_H_