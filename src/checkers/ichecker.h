#ifndef SCDS_CHECKERS_ICHECKER_H_
#define SCDS_CHECKERS_ICHECKER_H_

#include "networks/Network.h"

class IChecker {
public:
    Network *network;
    IChecker(Network *network) : network(network) {}
    virtual bool check() = 0;
};

#endif //SCDS_CHECKERS_CHECKER_H_