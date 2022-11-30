#include "checkers/two_hop_mis_checker.h"
#include <set>
#include <unordered_map>

#include "algorithms/ialg_node.h"

TwoHopMISChecker::TwoHopMISChecker(Network *network) : MISChecker(network)
{
    two_hop_mis_checker = new centralized::TwoHopMISChecker(
        centralized_graph, network->get_selected_nodes());
}

bool TwoHopMISChecker::check(bool is_final_check) const
{
    std::cout << "TwoHopMISChecker::check()\n";
    if (selected_nodes.empty()) return false;
    if (!check_all_decided(is_final_check)) {
        if (is_final_check) EV_ERROR << "Failed ALL DECIDED check!\n";
        return false;
    }
    return two_hop_mis_checker->check();
}

TwoHopMISChecker::~TwoHopMISChecker() { delete two_hop_mis_checker; }