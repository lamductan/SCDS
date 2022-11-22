#ifndef CENTRALIZED_CDS_ALGORITHMS_CHECKERS_MIS_CHECKER_H_
#define CENTRALIZED_CDS_ALGORITHMS_CHECKERS_MIS_CHECKER_H_

#include "centralized/algorithms/checkers/ichecker.h"

namespace centralized {

class MISChecker : public IChecker {
public:
    MISChecker(CDSSimpleAlg *alg, const std::vector<int> &selected_nodes);
    virtual bool check_independent() const;
    virtual bool check() const;
}; //class MISChecker

}; //namespace centralized

#endif //CENTRALIZED_CDS_ALGORITHMS_CHECKERS_MIS_CHECKER_H_