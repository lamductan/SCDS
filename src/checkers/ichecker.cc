#include "checkers/ichecker.h"

bool IChecker::check_all_decided(bool is_final_check) {
    EV << "IChecker::check_all_decided()\n";
    std::vector<int> failed_all_decided_check_nodes;
    for(auto it : network->nodes) {
        Node *node = it.second;
        if (!node->is_decided()) {
            failed_all_decided_check_nodes.push_back(node->id);
        }
    }

    bool res = failed_all_decided_check_nodes.empty();

    if (is_final_check) {
        if (!res) {
            EV << "\tFailed all decided check nodes: [";
            for(int node_id : failed_all_decided_check_nodes) EV << node_id << ",";
            EV << "]\n";
        } else {
            EV << "\tPASS\n";
        }
    }

    return res;
}


bool IChecker::check_cover(bool is_final_check, std::vector<int> need_to_check_nodes) {
    EV << "IChecker::check_cover()\n";
    std::unordered_map<int, bool> covered;
    for(int selected_node_id : selected_nodes) {
        covered[selected_node_id] = true;
        for(int neighbor_id : network->nodes[selected_node_id]->all_neighbors) {
            covered[neighbor_id] = true;
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
            EV << "Failed cover check nodes: [";
            for(int node_id : failed_cover_check_nodes) EV << node_id << ",";
            EV << "]\n";
        } else {
            EV << "\tPASS\n";
        }
    }

    return res;
}