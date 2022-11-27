#ifndef SCDS_CHECKERS_CHECKER_MIS_H_
#define SCDS_CHECKERS_CHECKER_MIS_H_

#include "checkers/ichecker.h"
#include <vector>

class MISChecker : public IChecker
{
  public:
    MISChecker(Network *network);
    virtual bool check(bool is_final_check = true) const;
};

#endif // SCDS_CHECKERS_CHECKER_MIS_H_