#include "centralized/algorithms/checkers/cds_checker.h"
#include <algorithm>
#include <unordered_map>

centralized::CDSChecker::CDSChecker(CDSSimpleAlg *alg,
                                    const std::vector<int> &selected_nodes)
    : centralized::IChecker(alg, selected_nodes)
{
}

centralized::Graph *centralized::CDSChecker::construct_cds_subgraph() const
{
    std::set<Edge> cds_edges;
    for (auto it : selected_nodes_map) {
        int nodeid = it.first;
        Node *node = it.second;
        for (auto it1 : node->neighbors) {
            int neighbor_id = it1.first;
            if (neighbor_id < nodeid)
                continue;
            if (selected_nodes_set.count(neighbor_id)) {
                cds_edges.insert({ nodeid, neighbor_id });
            }
        }
    }
    return new Graph(selected_nodes_map, cds_edges, CDS_SIMPLE_NODE_TYPE);
}

bool centralized::CDSChecker::check_connected() const
{
    std::cout << "centralized::CDSChecker::check_connected()\n";
    Graph *cds_subgraph = construct_cds_subgraph();
    bool res = cds_subgraph->is_connected();
    if (res)
        std::cout << "\tPASS\n";
    else
        std::cout << "\tFAILED\n";
    delete cds_subgraph;
    return res;
}

bool centralized::CDSChecker::check() const
{
    std::cout << "centralized::CDSChecker::check()\n";
    if (!IChecker::check())
        return false;
    if (!check_connected()) {
        std::cout << "Failed CENTRALIZED CONNECTED check!\n";
        return false;
    }
    std::cout << "PASS CENTRALIZED CDS CHECK! There are "
              << selected_nodes.size() << " nodes selected.\n";
    return true;
}