#include <algorithm>
#include <unordered_map>
#include "centralized/algorithms/checkers/two_hop_mis_checker.h"

centralized::TwoHopMISChecker::TwoHopMISChecker(CDSSimpleAlg *alg, const std::vector<int> &selected_nodes)
    : centralized::MISChecker(alg, selected_nodes) {}

bool centralized::TwoHopMISChecker::check_two_hop_mis() const {
    std::cout << "centralized::TwoHopMISChecker::check_two_hop_mis()\n";
    Graph *two_hop_mis_graph = construct_two_hop_mis_graph();
    bool res = two_hop_mis_graph->is_connected();
    if (res) std::cout << "\tPASS\n";
    else std::cout << "\tFAILED\n";
    delete two_hop_mis_graph;
    return res;
}

centralized::Graph *centralized::TwoHopMISChecker::construct_two_hop_mis_graph() const {
    std::set<Edge> two_hop_edges;
    for(auto it : selected_nodes_map) {
        int nodeid = it.first;
        Node *node = it.second;
        std::map<Node *, std::array<Node *, 2>> two_hop_neighbors = node->two_hop_neighbors;
        for(auto it1 : two_hop_neighbors) {
            int two_hop_neighbor_id = it1.first->id;
            if (two_hop_neighbor_id < nodeid) continue;
            if (selected_nodes_set.count(two_hop_neighbor_id)) {
                two_hop_edges.insert({nodeid, two_hop_neighbor_id});
            }
        }
    }
    return new Graph(selected_nodes_map, two_hop_edges, CDS_SIMPLE_NODE_TYPE);
}

bool centralized::TwoHopMISChecker::check() const {
    std::cout << "centralized::TwoHopMISChecker::check()\n";
    if (!check_two_hop_mis()) {
        std::cout << "Failed CENTRALIZED TWO-HOP-MIS check!\n";
        return false;
    }
    if (!MISChecker::check()) return false;
    std::cout << "PASS CENTRALIZED TWO-HOP-MIS CHECK!\n";
    return true;
}