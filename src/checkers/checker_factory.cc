#include "checkers/checker_factory.h"

IChecker *CheckerFactory::create_checker(Network *network)
{
    const char *alg_name = network->alg_name;
    if (strcmp(alg_name, "dummy") == 0) {
        return new DummyChecker(network);
    } else if (strncmp(alg_name, "MIS", 3) == 0) {
        EV << "Algorithm: " << alg_name << '\n';
        return new MISChecker(network);
    } else if (strncmp(alg_name, "CDS", 3) == 0) {
        EV << "Algorithm: " << alg_name << '\n';
        return new CDSChecker(network);
    } else if (strncmp(alg_name, "2RS", 3) == 0) {
        EV << "Algorithm: " << alg_name << '\n';
        return new TwoRSChecker(network);
    } else if (strncmp(alg_name, "MST", 3) == 0) {
        EV << "Algorithm: " << alg_name << '\n';
        return new MSTChecker(network);
    } else if (strncmp(alg_name, "BFS", 3) == 0) {
        EV << "Algorithm: " << alg_name << '\n';
        return new BFSChecker(network);
    } else if (strncmp(alg_name, "TwoHopMIS", 9) == 0) {
        EV << "Algorithm: " << alg_name << '\n';
        return new TwoHopMISChecker(network);
    }
    return nullptr;
}