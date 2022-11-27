#include "centralized/algorithms/checkers/ichecker.h"
#include <algorithm>
#include <unordered_map>

centralized::IChecker::IChecker(CDSSimpleAlg *alg,
                                const std::vector<int> &selected_nodes)
    : alg(alg), selected_nodes(selected_nodes)
{
    for (auto it : alg->graph->nodes) {
        int nodeid = it.first;
        need_to_check_nodes.push_back(nodeid);
    }
    selected_nodes_set =
        std::set<int>(selected_nodes.begin(), selected_nodes.end());
    for (int selected_node_id : selected_nodes) {
        Node *selected_node = alg->graph->nodes[selected_node_id];
        selected_nodes_map[selected_node_id] = selected_node;
    }
}

bool centralized::IChecker::check_cover() const
{
    std::cout << "centralized::IChecker::check_cover()\n";
    std::unordered_map<int, bool> covered;
    for (int selected_node_id : selected_nodes) {
        covered[selected_node_id] = true;
        for (auto it : alg->graph->nodes[selected_node_id]->neighbors) {
            int neighbor_id = it.first;
            covered[neighbor_id] = true;
        }
    }

    std::vector<int> failed_cover_check_nodes;
    for (int nodeid : need_to_check_nodes) {
        if (!covered[nodeid]) {
            failed_cover_check_nodes.push_back(nodeid);
        }
    }

    bool res = failed_cover_check_nodes.empty();

    if (!res) {
        std::sort(failed_cover_check_nodes.begin(),
                  failed_cover_check_nodes.end());
        std::cout << "\tFailed CENTRALIZED cover check nodes: [";
        for (int nodeid : failed_cover_check_nodes)
            std::cout << nodeid << ",";
        std::cout << "]\n";
    } else {
        std::cout << "\tPASS CENTRALIZED cover check\n";
    }

    return res;
}

bool centralized::IChecker::check() const
{
    if (!check_cover()) {
        std::cout << "Failed CENTRALIZED COVER check!\n";
        return false;
    }
    return true;
}