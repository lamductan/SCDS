#include "common.h"
#include "algorithms/alg_factory.h"

IAlgNode *AlgFactory::create_alg(Node *node, const char* alg_name, int starting_round, double threshold_ratio) {
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
    } else if (strcmp(alg_name, "MIS-Lam-M1") == 0) {
        EV << "Create LamMISM1Alg instance\n";
        return new LamMISM1Alg(node, starting_round);
    }
    return nullptr;
}