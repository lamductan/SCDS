#include "checkers/bfs_checker.h"

#include <set>
#include <unordered_map>

#include "algorithms/ialg_node.h"

using namespace omnetpp;

BFSChecker::BFSChecker(Network *network) : IChecker(network) {}

bool BFSChecker::check_bfs_structure(bool is_final_check) const
{
    std::cout << "BFSChecker::check_bfs_structure()\n";
    centralized::WAF02CDSAlg *WAF02_cds_alg = new centralized::WAF02CDSAlg(
        centralized_graph->nodes, centralized_graph->edges);
    std::map<int, centralized::BFSTreeStructure> bfs_structures_distributed =
        get_bfs_structure_from_network();
    int distributed_bfs_root = bfs_structures_distributed[0].root;
    std::map<int, centralized::BFSTreeStructure> bfs_structures_centralized =
        WAF02_cds_alg->find_bfs_tree_structure_from_root(distributed_bfs_root);

    bool res = is_equal_bfs_structure(bfs_structures_centralized,
                                      bfs_structures_distributed);
    if (res)
        std::cout << "\tPASS\n";
    else
        std::cout << "\tFAILED\n";
    delete WAF02_cds_alg;
    return res;
}

bool BFSChecker::check(bool is_final_check) const
{
    std::cout << "BFSChecker::check()\n";
    if (!check_all_decided(is_final_check)) {
        if (is_final_check) EV_ERROR << "Failed ALL DECIDED check!\n";
        return false;
    }
    if (!check_bfs_structure(is_final_check)) {
        if (is_final_check) EV_ERROR << "Failed BFS STRUCTURE check!\n";
        return false;
    }
    EV << "PASS BFS CHECK!\n";
    return true;
}

std::map<int, centralized::BFSTreeStructure>
BFSChecker::get_bfs_structure_from_network() const
{
    std::map<int, centralized::BFSTreeStructure> bfs_tree_structures;
    for (auto it : network->nodes) {
        Node *node = it.second;
        bfs_tree_structures[node->id] = node->alg->get_bfs_tree_structure();
    }
    return bfs_tree_structures;
}

bool BFSChecker::is_equal_bfs_structure(
    const std::map<int, centralized::BFSTreeStructure> &lhs,
    std::map<int, centralized::BFSTreeStructure> &rhs)
{
    if (lhs.size() != rhs.size()) return false;
    bool res = true;
    for (auto it : lhs) {
        int nodeid = it.first;
        centralized::BFSTreeStructure lhs_bfs_tree_structure = it.second;
        centralized::BFSTreeStructure rhs_bfs_tree_structure = rhs[nodeid];
        EV << "centralized = " << lhs_bfs_tree_structure.to_string(0) << '\n';
        EV << "distributed = " << rhs_bfs_tree_structure.to_string(0) << '\n';
        if (!lhs_bfs_tree_structure.equals(rhs_bfs_tree_structure)) {
            EV << "\tdiff\n";
            res = false;
        }
        EV << '\n';
    }
    return res;
}