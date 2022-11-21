#include <set>
#include <algorithm>
#include <unordered_map>

#include "checkers/two_rs_checker.h"
#include "algorithms/ialg_node.h"

using namespace omnetpp;

TwoRSChecker::TwoRSChecker(Network *network) : IChecker(network) {
    selected_nodes = network->get_selected_nodes();
}

bool TwoRSChecker::check_cover(bool is_final_check, std::vector<int> need_to_check_nodes) const {
    EV << "TwoRSChecker::check_cover()\n";
    std::unordered_map<int, bool> covered;
    for(int selected_node_id : selected_nodes) {
        covered[selected_node_id] = true;
        for(int neighbor_id : network->nodes[selected_node_id]->all_neighbors) {
            covered[neighbor_id] = true;
            for(int two_hop_neighbor_id : network->nodes[neighbor_id]->all_neighbors) {
                covered[two_hop_neighbor_id] = true;
            }
        }
    }

    if (need_to_check_nodes.size() == 1 && need_to_check_nodes[0] == -1) {
        need_to_check_nodes.clear();
        for(auto it : network->nodes) {
            int node_id = it.first;
            need_to_check_nodes.push_back(node_id);
        }
    }
    
    std::vector<int> failed_cover_check_nodes;
    for(int node_id : need_to_check_nodes) {
        if (!covered[node_id]) {
            failed_cover_check_nodes.push_back(node_id);
        }
    }

    bool res = failed_cover_check_nodes.empty();

    if (is_final_check) {
        if (!res) {
            std::sort(failed_cover_check_nodes.begin(), failed_cover_check_nodes.end());
            EV << "Failed two ruling set cover check nodes: [";
            for(int node_id : failed_cover_check_nodes) EV << node_id << ",";
            EV << "]\n";
        } else {
            EV << "\tPASS\n";
        }
    }

    return res;
}

bool TwoRSChecker::check(bool is_final_check) const {
    if (!check_all_decided(is_final_check)) {
        if (is_final_check) EV_ERROR << "Failed ALL DECIDIED check!\n";
        return false;
    }

    if (!check_cover(is_final_check)) {
        if (is_final_check) EV_ERROR << "Failed COVER check!\n";
        return false;
    }
    if (!check_independent(is_final_check)) {
        if (is_final_check) EV_ERROR << "Failed INDEPENDENT check!\n";
        return false;
    }
    EV << "PASS TwoRS CHECK!\n";
    return true;
}