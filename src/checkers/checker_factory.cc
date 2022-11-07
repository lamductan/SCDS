#include "checkers/checker_factory.h"

IChecker * CheckerFactory::create_checker(Network *network) {
    const char *alg_name = network->alg_name;
    if (strcmp(alg_name, "dummy") == 0) {
        return new DummyChecker(network);
    } else {
        EV << "Algorithm: " << alg_name << '\n';
        return new DummyChecker(network);
    }
    return nullptr;
}