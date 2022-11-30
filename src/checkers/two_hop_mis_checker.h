#ifndef SCDS_CHECKERS_CHECKER_TWO_HOP_MIS_H_
#define SCDS_CHECKERS_CHECKER_TWO_HOP_MIS_H_

#include "centralized/algorithms/checkers/two_hop_mis_checker.h"
#include "centralized/graph/graph.h"
#include "checkers/mis_checker.h"

class TwoHopMISChecker : public MISChecker
{
  private:
    centralized::TwoHopMISChecker *two_hop_mis_checker;

  public:
    TwoHopMISChecker(Network *network);
    virtual bool check(bool is_final_check = true) const;
    virtual ~TwoHopMISChecker();
};

#endif // SCDS_CHECKERS_CHECKER_TWO_HOP_MIS_H_