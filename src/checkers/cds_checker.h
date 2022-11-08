#ifndef SCDS_CHECKERS_CHECKER_CDS_H_
#define SCDS_CHECKERS_CHECKER_CDS_H_

#include "checkers/ichecker.h"

class CDSChecker : public IChecker {
public:
    CDSChecker(Network *network);
    virtual bool check();
    virtual bool check_cover();
    virtual bool check_connected();
    virtual ~CDSChecker();
};

#endif //SCDS_CHECKERS_CHECKER_CDS_H_