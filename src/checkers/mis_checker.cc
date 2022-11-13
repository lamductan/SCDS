#include <unordered_map>
#include <unordered_set>

#include "checkers/mis_checker.h"
#include "algorithms/ialg_node.h"

using namespace omnetpp;

MISChecker::MISChecker(Network *network) : IChecker(network) {
    selected_nodes = network->get_selected_nodes();
}

bool MISChecker::check_independent(bool is_final_check) {
    EV << "MISChecker::check_independent()\n";
    std::unordered_set<int> selected_nodes_set(selected_nodes.begin(), selected_nodes.end());
    for(int selected_node_id : selected_nodes) {
        for(int neighbor_id : network->nodes[selected_node_id]->all_neighbors) {
            if (selected_nodes_set.count(neighbor_id) > 0) {
                if (is_final_check) 
                    EV << "\t" << "Failed: Both " << selected_node_id << " and " << neighbor_id
                       << " are selected but they are neighbors\n";
                return false;
            }
        }
    }
    EV << "\tPASS\n";
    return true;
}

bool MISChecker::check(bool is_final_check) {
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
    EV << "PASS MIS CHECK!\n";
    return true;
}

MISChecker::~MISChecker() {}