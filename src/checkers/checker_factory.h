#ifndef SCDS_CHECKERS_CHECKER_H_
#define SCDS_CHECKERS_CHECKER_H_

#include "dummy_checker.h"
#include "mis_checker.h"
#include "cds_checker.h"

class CheckerFactory {
public:
    static IChecker *create_checker(Network *network);
};

#endif //SCDS_CHECKERS_CHECKER_H_