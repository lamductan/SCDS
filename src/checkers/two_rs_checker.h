#ifndef SCDS_CHECKERS_CHECKER_TWO_RS_H_
#define SCDS_CHECKERS_CHECKER_TWO_RS_H_

#include <vector>
#include "checkers/ichecker.h"

class TwoRSChecker : public IChecker {
public:
    TwoRSChecker(Network *network);
    virtual bool check_cover(bool is_final_check, std::vector<int> need_to_check_nodes={-1}) const;
    virtual bool check(bool is_final_check=true) const;
};

#endif //SCDS_CHECKERS_CHECKER_TWO_RS_H_