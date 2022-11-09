#ifndef SCDS_CHECKERS_CHECKER_MIS_H_
#define SCDS_CHECKERS_CHECKER_MIS_H_

#include <vector>
#include "checkers/ichecker.h"

class MISChecker : public IChecker {
public:
    MISChecker(Network *network);

    virtual bool check(bool is_final_check=true);
    virtual bool check_cover(bool is_final_check);
    virtual bool check_independent(bool is_final_check);

    virtual ~MISChecker();
};

#endif //SCDS_CHECKERS_CHECKER_MIS_H_