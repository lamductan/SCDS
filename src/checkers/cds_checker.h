#ifndef SCDS_CHECKERS_CHECKER_CDS_H_
#define SCDS_CHECKERS_CHECKER_CDS_H_

#include "checkers/ichecker.h"

class CDSChecker : public IChecker {
public:
    CDSChecker(Network *network);
    virtual bool check(bool is_final_check=true) const;
    virtual bool check_connected(bool is_final_check) const;
};

#endif //SCDS_CHECKERS_CHECKER_CDS_H_