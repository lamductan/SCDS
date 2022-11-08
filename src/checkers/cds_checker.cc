#include <unordered_map>

#include "checkers/cds_checker.h"
#include "algorithms/ialg_node.h"

using namespace omnetpp;

CDSChecker::CDSChecker(Network *network) : IChecker(network) {
    selected_nodes = network->get_selected_nodes();
}

bool CDSChecker::check_cover() {
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

bool CDSChecker::check_connected() {
    return false;
}

bool CDSChecker::check() {
    if (!check_cover()) {
        EV_ERROR << "Failed COVER check!\n";
        return false;
    }
    if (!check_connected()) {
        EV_ERROR << "Failed CONNECTED check!\n";
        return false;
    }
    EV << "PASS CDS CHECK!\n";
    return true;
}

CDSChecker::~CDSChecker() {}