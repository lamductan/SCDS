#include <unordered_map>

#include "checkers/cds_checker.h"
#include "algorithms/ialg_node.h"

using namespace omnetpp;

CDSChecker::CDSChecker(Network *network) : IChecker(network) {
    selected_nodes = network->get_selected_nodes();
}

bool CDSChecker::check_connected(bool is_final_check) const {
    return false;
}

bool CDSChecker::check(bool is_final_check) const {
    if (!check_cover(is_final_check)) {
        if (is_final_check) EV_ERROR << "Failed COVER check!\n";
        return false;
    }
    if (!check_connected(is_final_check)) {
        if (is_final_check) EV_ERROR << "Failed CONNECTED check!\n";
        return false;
    }
    EV << "PASS CDS CHECK!\n";
    return true;
}