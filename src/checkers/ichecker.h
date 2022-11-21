#ifndef SCDS_CHECKERS_ICHECKER_H_
#define SCDS_CHECKERS_ICHECKER_H_

#include "networks/Network.h"

class IChecker {
public:
    std::vector<int> selected_nodes;
    Network *network;
    IChecker(Network *network) : network(network) {}
    virtual bool check_all_decided(bool is_final_check) const;
    virtual bool check_cover(bool is_final_check, std::vector<int> need_to_check_nodes={-1}) const;
    virtual bool check_independent(bool is_final_check) const;
    virtual bool check(bool is_final_check=true) const = 0;
    virtual ~IChecker() {}
};

#endif //SCDS_CHECKERS_CHECKER_H_