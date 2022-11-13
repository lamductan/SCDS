#ifndef SCDS_CHECKERS_CHECKER_CDS_H_
#define SCDS_CHECKERS_CHECKER_CDS_H_

#include "checkers/ichecker.h"

class CDSChecker : public IChecker {
public:
    CDSChecker(Network *network);
    virtual bool check(bool is_final_check=true);
    virtual bool check_cover(bool is_final_check);
    virtual bool check_connected(bool is_final_check);
};

#endif //SCDS_CHECKERS_CHECKER_CDS_H_