#ifndef SCDS_CHECKERS_CHECKER_DUMMY_H_
#define SCDS_CHECKERS_CHECKER_DUMMY_H_

#include "checkers/ichecker.h"

class DummyChecker : public IChecker {
public:
    DummyChecker(Network *network) : IChecker(network) {}
    virtual bool check() { return true; }
};

#endif //SCDS_CHECKERS_CHECKER_DUMMY_H_