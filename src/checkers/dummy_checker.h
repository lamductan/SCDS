#ifndef SCDS_CHECKERS_CHECKER_DUMMY_H_
#define SCDS_CHECKERS_CHECKER_DUMMY_H_

#include "checkers/ichecker.h"

class DummyChecker : public IChecker {
public:
    DummyChecker(Network *network) : IChecker(network) {}
    virtual bool check(bool is_final_check=true) { return true; }
    virtual ~DummyChecker() {}
};

#endif //SCDS_CHECKERS_CHECKER_DUMMY_H_