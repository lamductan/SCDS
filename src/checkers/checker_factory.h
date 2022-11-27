#ifndef SCDS_CHECKERS_CHECKER_H_
#define SCDS_CHECKERS_CHECKER_H_

#include "cds_checker.h"
#include "dummy_checker.h"
#include "mis_checker.h"
#include "mst_checker.h"
#include "two_rs_checker.h"

class CheckerFactory
{
  public:
    static IChecker *create_checker(Network *network);
};

#endif // SCDS_CHECKERS_CHECKER_H_