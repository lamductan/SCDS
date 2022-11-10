#include <unordered_map>
#include <unordered_set>

#include "checkers/mis_checker.h"
#include "algorithms/ialg_node.h"

using namespace omnetpp;

MISChecker::MISChecker(Network *network) : IChecker(network) {
    selected_nodes = network->get_selected_nodes();
}

bool MISChecker::check_cover(bool is_final_check) {
    std::unordered_map<int, bool> covered;
    for(int selected_node_id : selected_nodes) {
        covered[selected_node_id] = true;
        for(int neighbor_id : network->nodes[selected_node_id]->all_neighbors) {
            covered[neighbor_id] = true;
        }
    }

    bool res = true;
    std::vector<int> failed_cover_check_nodes;
    for(auto it : network->nodes) {
        int node_id = it.first;
        if (!covered[node_id]) {
            failed_cover_check_nodes.push_back(node_id);
            res = false;
        }
    }

    return res;
}

bool MISChecker::check_independent(bool is_final_check) {
    EV << "Checking INDEPENDENT...\n";
    std::unordered_set<int> selected_nodes_set(selected_nodes.begin(), selected_nodes.end());
    for(int selected_node_id : selected_nodes) {
        for(int neighbor_id : network->nodes[selected_node_id]->all_neighbors) {
            if (selected_nodes_set.count(neighbor_id) > 0) {
                if (is_final_check) 
                    EV_ERROR << "\t" << "Both " << selected_node_id << " and " << neighbor_id
                             << " are selected but they are neighbors\n";
                return false;
            }
        }
    }
    return true;
}

bool MISChecker::check(bool is_final_check) {
    if (!check_cover(is_final_check)) {
        if (is_final_check) EV_ERROR << "Failed COVER check!\n";
        return false;
    }
    if (!check_independent(is_final_check)) {
        if (is_final_check) EV_ERROR << "Failed INDEPENDENT check!\n";
        return false;
    }
    EV << "PASS MIS CHECK!\n";
    return true;
}

MISChecker::~MISChecker() {}