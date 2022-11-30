#include "algorithms/alg_factory.h"
#include "common.h"

IAlgNode *AlgFactory::create_alg(Node *node, const char *alg_name,
                                 int starting_round, double threshold_ratio)
{
    if (strcmp(alg_name, "dummy") == 0) {
        EV << "Create DummyAlg instance\n";
        return new DummyAlg(node, starting_round);
    } else if (strcmp(alg_name, "MIS-Luby") == 0) {
        EV << "Create LubyMISAlg instance\n";
        return new LubyMISAlg(node, starting_round);
    } else if (strcmp(alg_name, "MIS-SW08") == 0) {
        EV << "Create SW08MISAlg instance\n";
        return new SW08MISAlg(node, starting_round);
    } else if (strcmp(alg_name, "MIS-GP22") == 0) {
        EV << "Create GP22MISAlg instance\n";
        return new GP22MISAlg(node, starting_round);
    } else if (strcmp(alg_name, "MIS-GP22-0.1") == 0) {
        EV << "Create GP22MISAlg instance\n";
        return new GP22MISAlg(node, starting_round, threshold_ratio);
    } else if (strcmp(alg_name, "MIS-GP22-0.5") == 0) {
        EV << "Create GP22MISAlg instance\n";
        return new GP22MISAlg(node, starting_round, threshold_ratio);
    } else if (strcmp(alg_name, "2RS-BGKO22") == 0) {
        EV << "Create BGKO22TwoRSAlg instance\n";
        return new BGKO22TwoRSAlg(node, starting_round);
    } else if (strcmp(alg_name, "2RS-Lam") == 0) {
        EV << "Create LamTwoRSAlg instance\n";
        return new LamTwoRSAlg(node, starting_round);
    } else if (strcmp(alg_name, "MIS-Lam") == 0) {
        EV << "Create LamMISAlg instance\n";
        return new LamMISAlg(node, starting_round);
    } else if (strcmp(alg_name, "CDS-Simple") == 0) {
        EV << "Create SimpleCDSAlg instance\n";
        return new SimpleCDSAlg(node, starting_round);
    } else if (strcmp(alg_name, "MST-GHS") == 0) {
        EV << "Create GHSMSTAlg instance\n";
        return new GHSMSTAlg(node, starting_round);
    } else if (strcmp(alg_name, "CDS-GK96") == 0) {
        EV << "Create GK96CDSAlg instance\n";
        return new GK96CDSAlg(node, starting_round);
    } else if (strcmp(alg_name, "BFS") == 0) {
        EV << "Create BFSAlg instance\n";
        return new BFSAlg(node, starting_round);
    } else if (strcmp(alg_name, "TwoHopMIS-WAF02") == 0) {
        EV << "Create WAF02TwoHopMISAlg instance\n";
        return new WAF02TwoHopMISAlg(node, starting_round);
    } else if (strcmp(alg_name, "CDS-WAF02") == 0) {
        EV << "Create WAF02CDSAlg instance\n";
        return new WAF02CDSAlg(node, starting_round);
    }
    return nullptr;
}