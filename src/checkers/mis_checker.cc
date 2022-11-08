#include <unordered_map>
#include <unordered_set>

#include "checkers/mis_checker.h"
#include "algorithms/ialg_node.h"

using namespace omnetpp;

MISChecker::MISChecker(Network *network) : IChecker(network) {
    selected_nodes = network->get_selected_nodes();
}

bool MISChecker::check_cover() {
    std::unordered_map<int, bool> covered;
    for(int selected_node_id : selected_nodes) {
        covered[selected_node_id] = true;
        for(int neighbor_id : network->nodes[selected_node_id]->alg->all_neighbors) {
            covered[neighbor_id] = true;
        }
    }
    for(auto it : network->nodes) {
        int node_id = it.first;
        if (!covered[node_id]) return false;
    }
    return true;
}

bool MISChecker::check_independent() {
    EV << "Checking INDEPENDENT...\n";
    std::unordered_set<int> selected_nodes_set(selected_nodes.begin(), selected_nodes.end());
    for(int selected_node_id : selected_nodes) {
        for(int neighbor_id : network->nodes[selected_node_id]->alg->all_neighbors) {
            if (selected_nodes_set.count(neighbor_id) > 0) {
                EV_ERROR << "\t" << "Both " << selected_node_id << " and " << neighbor_id
                         << " are selected but they are neighbors\n";
                return false;
            }
        }
    }
    return true;
}

bool MISChecker::check() {
    if (!check_cover()) {
        EV_ERROR << "Failed COVER check!\n";
        return false;
    }
    if (!check_independent()) {
        EV_ERROR << "Failed INDEPENDENT check!\n";
        return false;
    }
    EV << "PASS MIS CHECK!\n";
    return true;
}

MISChecker::~MISChecker() {}