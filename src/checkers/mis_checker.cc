#include <set>
#include <unordered_map>

#include "algorithms/ialg_node.h"
#include "checkers/mis_checker.h"

MISChecker::MISChecker(Network *network) : IChecker(network)
{
    selected_nodes = network->get_selected_nodes();
}

bool MISChecker::check(bool is_final_check) const
{
    if (!check_all_decided(is_final_check)) {
        if (is_final_check)
            EV_ERROR << "Failed ALL DECIDED check!\n";
        return false;
    }

    if (!check_cover(is_final_check)) {
        if (is_final_check)
            EV_ERROR << "Failed COVER check!\n";
        return false;
    }
    if (!check_independent(is_final_check)) {
        if (is_final_check)
            EV_ERROR << "Failed INDEPENDENT check!\n";
        return false;
    }
    EV << "PASS MIS CHECK!\n";
    return true;
}