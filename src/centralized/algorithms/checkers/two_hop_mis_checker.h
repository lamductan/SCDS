#ifndef CENTRALIZED_CDS_ALGORITHMS_CHECKERS_TWO_HOP_MIS_CHECKER_H_
#define CENTRALIZED_CDS_ALGORITHMS_CHECKERS_TWO_HOP_MIS_CHECKER_H_

#include "centralized/algorithms/checkers/mis_checker.h"

namespace centralized {

class TwoHopMISChecker : public MISChecker
{
  private:
    Graph *construct_two_hop_mis_graph() const;

  public:
    TwoHopMISChecker(CDSSimpleAlg *alg, const std::vector<int> &selected_nodes);
    virtual bool check_two_hop_mis() const;
    virtual bool check() const;
}; // class TwoHopMISChecker

}; // namespace centralized

#endif // CENTRALIZED_CDS_ALGORITHMS_CHECKERS_TWO_HOP_MIS_CHECKER_H_