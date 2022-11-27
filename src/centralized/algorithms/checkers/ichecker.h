#ifndef CENTRALIZED_CDS_ALGORITHMS_CHECKERS_ICHECKER_H_
#define CENTRALIZED_CDS_ALGORITHMS_CHECKERS_ICHECKER_H_

#include "centralized/algorithms/cds_simple/CDS_simple.h"
#include <iostream>
#include <vector>

namespace centralized {

class IChecker
{
  public:
    CDSSimpleAlg *alg;
    std::vector<int> selected_nodes;
    std::vector<int> need_to_check_nodes;
    std::set<int> selected_nodes_set;
    std::map<int, Node *> selected_nodes_map;

    IChecker(CDSSimpleAlg *alg, const std::vector<int> &selected_nodes);
    virtual bool check_cover() const;
    virtual bool check() const;
}; // class IChecker

}; // namespace centralized

#endif // CENTRALIZED_CDS_ALGORITHMS_CHECKERS_ICHECKER_H_