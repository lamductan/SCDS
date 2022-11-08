#ifndef SCDS_CHECKERS_ICHECKER_H_
#define SCDS_CHECKERS_ICHECKER_H_

#include "networks/Network.h"

class IChecker {
public:
    std::vector<int> selected_nodes;
    Network *network;
    IChecker(Network *network) : network(network) {}
    virtual bool check() = 0;
    virtual ~IChecker() {}
};

#endif //SCDS_CHECKERS_CHECKER_H_