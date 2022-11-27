#ifndef CENTRALIZED_CDS_ALGORITHMS_CHECKERS_CDS_CHECKER_H_
#define CENTRALIZED_CDS_ALGORITHMS_CHECKERS_CDS_CHECKER_H_

#include "centralized/algorithms/checkers/ichecker.h"
#include <map>

namespace centralized {

class CDSChecker : public IChecker
{
  private:
    Graph *construct_cds_subgraph() const;

  public:
    CDSChecker(CDSSimpleAlg *alg, const std::vector<int> &selected_nodes);
    virtual bool check_connected() const;
    virtual bool check() const;
}; // class MISChecker

}; // namespace centralized

#endif // CENTRALIZED_CDS_ALGORITHMS_CHECKERS_CDS_CHECKER_H_