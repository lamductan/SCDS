#include "centralized/algorithms/checkers/mis_checker.h"
#include <algorithm>
#include <unordered_map>

centralized::MISChecker::MISChecker(CDSSimpleAlg *alg,
                                    const std::vector<int> &selected_nodes)
    : centralized::IChecker(alg, selected_nodes)
{
}

bool centralized::MISChecker::check_independent() const
{
    std::cout << "centralized::MISChecker::check_independent()\n";
    for (int selected_node_id : selected_nodes) {
        for (auto it : alg->graph->nodes[selected_node_id]->neighbors) {
            int neighbor_id = it.first;
            if (selected_nodes_set.count(neighbor_id) > 0) {
                std::cout << "\t"
                          << "Failed: Both " << selected_node_id << " and "
                          << neighbor_id
                          << " are selected but they are neighbors\n";
                return false;
            }
        }
    }
    std::cout << "\tPASS\n";
    return true;
}

bool centralized::MISChecker::check() const
{
    std::cout << "centralized::MISChecker::check()\n";
    if (!IChecker::check())
        return false;
    if (!check_independent()) {
        std::cout << "Failed CENTRALIZED INDEPENDENT check!\n";
        return false;
    }
    std::cout << "PASS CENTRALIZED MIS CHECK!\n";
    return true;
}