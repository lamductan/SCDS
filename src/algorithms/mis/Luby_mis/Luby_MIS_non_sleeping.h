#ifndef SCDS_ALGORITHMS_MIS_LUBY_MIS_NON_SLEEPING_H_
#define SCDS_ALGORITHMS_MIS_LUBY_MIS_NON_SLEEPING_H_

#include "algorithms/mis/Luby_mis/Luby_MIS.h"


class LubyMISNonSleepingAlg : public LubyMISAlg {
public:
    LubyMISNonSleepingAlg(Node *node, int starting_round=1);
    virtual bool is_awake() override;
};

#endif //SCDS_ALGORITHMS_MIS_LUBY_MIS_NON_SLEEPING_H_