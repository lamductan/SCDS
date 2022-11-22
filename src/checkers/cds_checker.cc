#include <unordered_map>

#include "checkers/cds_checker.h"
#include "algorithms/ialg_node.h"

using namespace omnetpp;

CDSChecker::CDSChecker(Network *network) : IChecker(network) {
    selected_nodes = network->get_selected_nodes();
    selected_nodes_set = std::set<int>(selected_nodes.begin(), selected_nodes.end());
    for(int selected_node_id : selected_nodes) {
        Node *node = network->nodes[selected_node_id];
        centralized::Node *centralized_node = new centralized::Node(0, 0, node->id);
        selected_nodes_map[selected_node_id] = centralized_node;
    }
}

centralized::Graph *CDSChecker::construct_cds_subgraph() const {
    std::set<centralized::Edge> cds_edges;
    for(auto it : selected_nodes_map) {
        int nodeid = it.first;
        centralized::Node *centralized_node = it.second;
        Node *node = network->nodes[nodeid];
        for(int neighbor_id : node->all_neighbors) {
            if (neighbor_id < nodeid) continue;
            if (selected_nodes_set.count(neighbor_id)) {
                cds_edges.insert({nodeid, neighbor_id});
            }
        }
    }
    return new centralized::Graph(selected_nodes_map, cds_edges, centralized::CDS_SIMPLE_NODE_TYPE);
}

bool CDSChecker::check_connected(bool is_final_check) const {
    std::cout << "CDSChecker::check_connected()\n";
    centralized::Graph *cds_subgraph = construct_cds_subgraph();
    bool res = cds_subgraph->is_connected();
    if (res) std::cout << "\tPASS\n";
    else std::cout << "\tFAILED\n";
    delete cds_subgraph;
    return res;
}

bool CDSChecker::check(bool is_final_check) const {
    if (!check_cover(is_final_check)) {
        if (is_final_check) EV_ERROR << "Failed COVER check!\n";
        return false;
    }
    if (!check_connected(is_final_check)) {
        if (is_final_check) EV_ERROR << "Failed CONNECTED check!\n";
        return false;
    }
    EV << "PASS CDS CHECK!\n";
    return true;
}

CDSChecker::~CDSChecker() {
    for(auto it : selected_nodes_map) delete it.second;
}