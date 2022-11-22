#include "checkers/checker_factory.h"

IChecker *CheckerFactory::create_checker(Network *network) {
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
    }
    return nullptr;
}