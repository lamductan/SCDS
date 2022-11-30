#include "checkers/ichecker.h"
#include <algorithm>

bool IChecker::check_all_decided(bool is_final_check) const
{
    EV << "IChecker::check_all_decided()\n";
    std::vector<int> failed_all_decided_check_nodes;
    for (auto it : network->nodes) {
        Node *node = it.second;
        if (!node->is_decided()) {
            failed_all_decided_check_nodes.push_back(node->id);
        }
    }

    bool res = failed_all_decided_check_nodes.empty();

    if (is_final_check) {
        if (!res) {
            std::sort(failed_all_decided_check_nodes.begin(),
                      failed_all_decided_check_nodes.end());
            EV << "\tFailed all decided check nodes: [";
            for (int node_id : failed_all_decided_check_nodes)
                EV << node_id << ",";
            EV << "]\n";
        } else {
            EV << "\tPASS\n";
        }
    }

    return res;
}

bool IChecker::check_cover(bool is_final_check,
                           std::vector<int> need_to_check_nodes) const
{
    EV << "IChecker::check_cover()\n";
    std::unordered_map<int, bool> covered;
    for (int selected_node_id : selected_nodes) {
        covered[selected_node_id] = true;
        for (int neighbor_id :
             network->nodes[selected_node_id]->all_neighbors) {
            covered[neighbor_id] = true;
        }
    }

    if (need_to_check_nodes.size() == 1 && need_to_check_nodes[0] == -1) {
        need_to_check_nodes.clear();
        for (auto it : network->nodes) {
            int node_id = it.first;
            need_to_check_nodes.push_back(node_id);
        }
    }

    std::vector<int> failed_cover_check_nodes;
    for (int node_id : need_to_check_nodes) {
        if (!covered[node_id]) {
            failed_cover_check_nodes.push_back(node_id);
        }
    }

    bool res = failed_cover_check_nodes.empty();

    if (is_final_check) {
        if (!res) {
            std::sort(failed_cover_check_nodes.begin(),
                      failed_cover_check_nodes.end());
            EV << "Failed cover check nodes: [";
            for (int node_id : failed_cover_check_nodes) EV << node_id << ",";
            EV << "]\n";
        } else {
            EV << "\tPASS\n";
        }
    }

    return res;
}

bool IChecker::check_independent(bool is_final_check) const
{
    EV << "IChecker::check_independent()\n";
    std::set<int> selected_nodes_set(selected_nodes.begin(),
                                     selected_nodes.end());
    for (int selected_node_id : selected_nodes) {
        for (int neighbor_id :
             network->nodes[selected_node_id]->all_neighbors) {
            if (selected_nodes_set.count(neighbor_id) > 0) {
                if (is_final_check)
                    EV << "\t"
                       << "Failed: Both " << selected_node_id << " and "
                       << neighbor_id
                       << " are selected but they are neighbors\n";
                return false;
            }
        }
    }
    EV << "\tPASS\n";
    return true;
}